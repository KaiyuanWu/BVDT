#include "application.h"
using namespace google::protobuf::io;
Application::Application()
{
    _has_init = false;
    _tree.clear_root_nodes();
    _f = NULL;
    _node_mode = NULL;
    _nclasses = 0;
    _nvariables = 0;
}

Application::~Application(){
    if(_f)  delete[] _f;
    if(_node_mode) delete _node_mode;
}

bool Application::init(const string& solver_proto){
    int input_fd = open(solver_proto.c_str(), O_RDONLY);
    if(input_fd == -1){
        cout << "File not found: " << solver_proto;
        exit(-1);
    }
    FileInputStream* input = new FileInputStream(input_fd);
    google::protobuf::TextFormat::Parse(input, &_solver_parameter);
    delete input;
    close(input_fd);

    _solver_parameter.PrintDebugString();
    return init(_solver_parameter.snapshot (), _solver_parameter.snapshot_interval (), _solver_parameter.nmaximum_trees ());
}

bool Application::init(const string &snapshot, int interval, int maximum_iterations){
    bool ret = true;
    _snapshot = snapshot;
    _interval = interval;
    _maximum_iterations = maximum_iterations;
    if(_has_init)
        _tree.clear_root_nodes();
    int start;
    if(interval<maximum_iterations)
        start = interval;
    else
        start = maximum_iterations;
    for(int id = start; ; ){
        char name[64];
        sprintf(name, "/iter_%d.model",id );
        string file_name = snapshot+name;
        ifstream infile(file_name.c_str());
        if(!infile.good()){
            cout<<"Can not open "<<file_name<<endl;
            if(id == start){
                _has_init = false;
                ret = false;
            }
            else{
                _has_init = true;
                ret = true;
            }
            break;
        }
        if(id == start){
            if(!_tree.ParseFromIstream(&infile)){
                infile.close();
                _has_init = false;
                ret = false;
                break;
            }
            _has_init = true;
            _nclasses = _tree.nclass_label();
            _nvariables = _tree.nvariables();
            if(_f) delete[] _f;
            _f = new float[_nclasses];
            _node_mode_type = _tree.node_type();

            if(_node_mode) delete _node_mode;
            switch (_node_mode_type) {
            case bvdt::ONE_VS_ONE:
                _node_mode = new NodeMode_One_Vs_One(_nclasses, bvdt::ONE_VS_ONE);
                break;
            case bvdt::ONE_VS_ALL:
                _node_mode = new NodeMode_One_Vs_All(_nclasses, bvdt::ONE_VS_ALL);
                break;
            case bvdt::ONE_VS_ALL_R:
                _node_mode = new NodeMode_One_Vs_All_R(_nclasses, bvdt::ONE_VS_ALL_R);
                break;
            default:
                cout<<"Error NodeMode: "<<_node_mode<<" has not been implemented!"<<endl;
                exit(0);
                break;
            }
        }
        else{
            TreeParameter tree;
            if(!tree.ParseFromIstream(&infile)){
                infile.close();
                break;
            }
            else{
                for(int i=0; i<tree.root_nodes_size(); i++){
                    NodeParameter* node = _tree.add_root_nodes();
                    node->CopyFrom( tree.root_nodes(i));
                }
            }
        }
        infile.close();
        if(id + interval<=maximum_iterations)
            id += interval;
        else if(id < maximum_iterations)
            id = maximum_iterations;
        else
            break;
    }
    maximum_iterations = _tree.root_nodes_size();
//    _tree.PrintDebugString();
    return ret;
}

int Application::eval(const float* x, float* val, int itree, bool reset_val){
    if(!_has_init){
        cout<<"Error: Has not initialized!"<<endl;
        return 0;
    }
    if(itree<0 || itree>_tree.root_nodes_size()){
        cout<<"Error: Tree id exceed the maximum number of trees!"<<endl;
        return 0;
    }
    if(reset_val)
        memset(val, 0, sizeof(float)*_nclasses);
    const NodeParameter* node = &(_tree.root_nodes(itree));
    while(true){
        if(node->node_type() == NodeParameter::LEAF){
            _node_mode->node_mode_value(_f, node->mode_id());
            for(int i=0; i<_nclasses; i++)
                val[i] += node->value()*_f[i];
            break;
        }
        else{
            if(x[node->feature()]<= node->cut())
                node = &(node->left_child());
            else
                node = &(node->right_child());
        }
        if(!node){
            cout<<"Error: tree id "<<itree<<endl;
            return 0;
        }
    }
    float maxv = val[0];
    int   maxi = 0;
    for(int i=1; i<_nclasses; i++){
        if(val[i]>maxv){
            maxi = i;
            maxv = val[i];
        }
    }
    return maxi;
}

int Application:: eval(const float *x, int *rlabel){
    float* val = new float[_nclasses];
    memset(val, 0, sizeof(float)*_nclasses);
    for(int iter=0; iter<_maximum_iterations; iter++){
        rlabel[iter] = eval(x, val, iter, false);
    }
}
int Application::nvariables(){
    return _nvariables;
}

int Application::nclasses(){
    return _nclasses;
}

int Application::maximum_iterations(){
    return _maximum_iterations;
}

int Application::eval(const float *x, float *val){
    if(!_has_init){
        cout<<"Error: Has not initialized!"<<endl;
        return 0;
    }
    memset(val, 0, sizeof(float)*_nclasses);

    for(int i=0; i<_tree.root_nodes_size(); i++){
        const NodeParameter* node = &(_tree.root_nodes(i));
        while(true){
            if(node->node_type() == NodeParameter::LEAF){
                _node_mode->node_mode_value(_f, node->mode_id());
                for(int i=0; i<_nclasses; i++)
                    val[i] += node->value()*_f[i]*_solver_parameter.shrinkage();
                break;
            }
            else{
                if(x[node->feature()]<= node->cut())
                    node = &(node->left_child());
                else
                    node = &(node->right_child());
            }
            if(!node){
                cout<<"Error: tree id "<<i<<endl;
                return 0;
            }
        }
    }
    float maxv = val[0];
    int   maxi = 0;
    for(int i=1; i<_nclasses; i++){
        if(val[i]>maxv){
            maxi = i;
            maxv = val[i];
        }
    }
    return maxi;
}
float Application::accuracy(const float* x, int* label, int nsamples, const char* outfile_prefix){
    float correct = 0.;
    float* val = new float[_nclasses];
    string outfile_name = parameter_name(&_solver_parameter);
    outfile_name = outfile_prefix + outfile_name;
    std::ofstream file(outfile_name.c_str(),ios::out);
    if(!file.good()){
        cout<<"Can not open "<<outfile_name<<endl;
        return 0.;
    }
    for(int iter = 0; iter<_tree.root_nodes_size(); iter++){
        correct = 0;
        for(int i=0; i<nsamples; i++){
            int r = eval(x+i*_nvariables, val, iter, false);
            if(r == label[i])
                correct++;
        }
        file<<iter<<"\t"<<correct/nsamples<<endl;
    }
    delete[] val;
    file.close();
    return correct/nsamples;
}
const SolverParameter* Application::solver_parameter(){
    return &_solver_parameter;
}

float Application::accuracy(const float* x, int* label, int nsamples, int* rlabel){
    float correct = 0.;
    float* val = new float[_nclasses];
    int* class_count = new int[_nclasses*_nclasses];
    memset(class_count, 0, sizeof(int)*_nclasses*_nclasses);
    for(int i=0; i<nsamples; i++){
        int r = eval(x+i*_nvariables, val);
        if(r == label[i]){
            correct++;
        }
//        cout<<r<<", "<<label[i]<<endl;
        class_count[label[i]*_nclasses+r]++;
        if(rlabel)
            rlabel[i] = r;
    }

    cout<<"Correct classify:\t"<<correct<<" of "<<nsamples<<endl;
    for(int i=0; i<_nclasses; i++){
        cout<<"class "<<i<<":\t";
        for(int j=0; j<_nclasses; j++){
            cout<<class_count[i*_nclasses+j]<<"\t";
        }
        cout<<endl;
    }
    delete[] class_count;
    delete[] val;
    return correct/nsamples;
}
string Application::parameter_name(SolverParameter* solver_parameter){
    string ret="";
    string snapshot = solver_parameter->snapshot();
    int last_slash = snapshot.length()-1;
    if(snapshot[last_slash] == '/'){
        last_slash--;
        while(snapshot[last_slash]!='/')
            last_slash--;
        last_slash++;
        ret = snapshot.substr(last_slash, snapshot.length()-last_slash-1);
    }
    else{
        while(snapshot[last_slash]!='/')
            last_slash--;
        last_slash++;
        ret = snapshot.substr(last_slash, snapshot.length()-last_slash);
    }
    return ret;
}
