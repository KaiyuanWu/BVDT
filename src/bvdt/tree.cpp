#include "tree.h"
#include <fstream>

Tree::Tree(){

}

void Tree::init_tree(BVDT_Data *data,
                     NodeGain* node_gain_split, NodeGain* node_gain_leaf, SolverParameter* solver_parameter){
    _data = data;
    _node_gain_split = node_gain_split;
    _node_gain_leaf = node_gain_leaf;
    _tree_id = -1;

    _nleaves = solver_parameter->nleaves();
    //node mode
    _node_modes.resize(4*_nleaves);
    __node_mode = solver_parameter->node_type();
    switch(__node_mode){
    case bvdt::ONE_VS_ALL:
        for(int i=0; i<_node_modes.size(); i++)
            _node_modes[i] = new NodeMode_One_Vs_All(_data->_nclasses,
                                                         bvdt::ONE_VS_ALL);
        break;
    case bvdt::ONE_VS_ONE:
        for(int i=0; i<_node_modes.size(); i++)
            _node_modes[i] = new NodeMode_One_Vs_One(_data->_nclasses,
                                                         bvdt::ONE_VS_ONE);
        break;
    case bvdt::ONE_VS_ALL_R:
        for(int i=0; i<_node_modes.size(); i++){
            float param = 0.001;
            if(solver_parameter->node_mode_param_size()>0)
                param = solver_parameter->node_mode_param(0);
            _node_modes[i] = new NodeMode_One_Vs_All_R(_data->_nclasses,
                                                         bvdt::ONE_VS_ALL_R, &param);
        }
        break;

    default:
        cout<<"This node mode has not been implemented yet!"<<endl;
        exit(0);
    }

    _data_index.resize(_data->_ntrain_samples);
    for(int i=0; i<_data->_ntrain_samples; i++){
        _data_index[i] = i;
    }

    _feature_index.resize(_data->_nfeatures);
    for(int i=0; i<_data->_nfeatures; i++){
        _feature_index[i] = i;
    }

    _node_mode_index.resize(_node_modes[0]->_nmodes);
    for(int i=0; i<_node_modes[0]->_nmodes; i++){
        _node_mode_index[i] = i;
    }

    random_shuffle(_data_index.begin(), _data_index.end());
    random_shuffle(_feature_index.begin(), _feature_index.end());
    random_shuffle(_node_mode_index.begin(), _node_mode_index.end());

    _node_array.resize(4*_nleaves);
    for(int i=0; i<_node_array.size(); i++){
        _node_array[i] = new Node();
        _node_array[i]->_left_child = NULL;
        _node_array[i]->_right_child = NULL;
        _node_array[i]->_node_type = NodeParameter::UNDEF;
        _node_array[i]->_nodemode = _node_modes[i];
    }
    _root_node = NULL;
    _effective_data_number = (solver_parameter->event_sample_rate()*_data->_ntrain_samples<1?1:solver_parameter->event_sample_rate()*_data->_ntrain_samples);
    _effective_feature_number = (solver_parameter->feature_sample_rate()*_data->_nfeatures<1?1:solver_parameter->feature_sample_rate()*_data->_nfeatures);
    _effective_node_mode_number = (solver_parameter->node_mode_sample_rate()*_node_modes[0]->_nmodes<1?1:solver_parameter->node_mode_sample_rate()*_node_modes[0]->_nmodes);

    _effective_data_index.resize(_effective_data_number);
    _effective_node_mode_index.resize(_effective_node_mode_number);
    _effective_feature_index.resize(_effective_feature_number);

    _current_data_index = 0;
    _current_feature_index = 0;
    _current_node_mode_index = 0;

    _direction_type = solver_parameter->direction_type();
    _leaf_value_data = solver_parameter->leaf_value_data();
    _save_accuracy = solver_parameter->save_accuracy ();
    _shrinkage = solver_parameter->shrinkage ();

    _tree_parameter.set_nclass_label(solver_parameter->nclass_label());
    _tree_parameter.set_nleaves(_nleaves);
    _tree_parameter.set_node_type(solver_parameter->node_type());
    _tree_parameter.set_nvariables(solver_parameter->nfeatures());
    _tree_parameter.set_shrinkage (_shrinkage);
    _tree_parameter.clear_root_nodes();
    _delta_f = new float[_data->_nclasses];

    _H = new double[_nleaves];
    _g = new double[_nleaves];
    _val = new double[_nleaves];
    _l1solver = new L1Solver(_nleaves, solver_parameter->maxz());
    _l2solver = new L2Solver(_nleaves, solver_parameter->maxz());

    _refit_type = solver_parameter->refit_type();
}

void Tree::refit_leaf_values_l1(){
    Node* pnode = _root_node;
    int index = 0;
    float gradient, hessian;
    vector<Node*> stack(_nleaves+1);
    vector<Node*> leaf_stack(_nleaves);
    stack[0]=_root_node;
    int point = 1;
    while(point>0){
        pnode = stack[point-1];
        point --;
        if(pnode->_node_type ==NodeParameter::LEAF){
            pnode->_nodemode->node_mode(gradient, hessian, pnode->_mode_id);
            _H[index] = hessian;
            _g[index] = gradient;
            leaf_stack[index] = pnode;
            index++;
        }
        else{
            stack[point] = pnode->_left_child;
            point ++;
            stack[point] = pnode->_right_child;
            point ++;
        }
    }
    double obj;
    for(int i = index; i < _nleaves; i++){
        _H[i] =1;
        _g[i] = 0;
    }
    _l1solver->solve(_H,_g, obj, _val);
    for(int i=0; i<index; i++){
//        cout<<leaf_stack[i]->_value<<",\t"<<_val[i]<<endl;
        leaf_stack[i]->_value = _val[i];
    }
}

void Tree::refit_leaf_values_l2(){
    Node* pnode = _root_node;
    int index = 0;
    float gradient, hessian;
    vector<Node*> stack(_nleaves+1);
    vector<Node*> leaf_stack(_nleaves);
    stack[0]=_root_node;
    int point = 1;
    while(point>0){
        pnode = stack[point-1];
        point --;
        if(pnode->_node_type ==NodeParameter::LEAF){
            pnode->_nodemode->node_mode(gradient, hessian, pnode->_mode_id);
            _H[index] = hessian;
            _g[index] = gradient;
            leaf_stack[index] = pnode;
            index++;
        }
        else{
            stack[point] = pnode->_left_child;
            point ++;
            stack[point] = pnode->_right_child;
            point ++;
        }
    }
    if(index>_nleaves){
        cout<<"here here"<<endl;
    }
    for(int i = index; i < _nleaves; i++){
        _H[i] =1;
        _g[i] = 0;
    }
    _l2solver->solve(_H,_g, _val);
    for(int i=0; i<index; i++){
        leaf_stack[i]->_value = _val[i];
    }
//    for(int i=0; i<_nleaves; i++)
//         cout<<_val[i]<<endl;
//    exit(0);
}


int Tree::nleaves_1 (){
    return _nleaves_1;
}

int Tree::nleaves (){
    return _nleaves;
}

void Tree::reshuffle(){
    _current_data_index = 0;
    _current_feature_index = 0;
    _current_node_mode_index = 0;
    random_shuffle(_data_index.begin(), _data_index.end());
    random_shuffle(_feature_index.begin(), _feature_index.end());
    random_shuffle(_node_mode_index.begin(), _node_mode_index.end());
}
void Tree::init_node(Node* node, int left, int right){
    node->_left_data_index = left;
    node->_right_data_index = right;
    NodeMode* nodemode = node->_nodemode;
    nodemode->reset();
    for(int i=left; i<=right; i++){
        int index = _data->data_reverse_order(i, 0);
        if(nodemode->___node_mode_ != bvdt::ONE_VS_ALL_R)
            nodemode->increment_all_node_mode(_data->train_p(index), _data->train_label(index));
        else
            nodemode->increment_all_node_mode(_data->train_f(index), _data->train_label(index));
    }
//    cout<<"init_node("<<left<<", "<<right<<")\t"<<endl;
//    nodemode->print();
    find_best_mode(node, nodemode, _node_gain_leaf);
//    cout<<"best_mode = "<<node->_mode_id<<", best_gain = "<<node->_node_gain<<", value = "<<node->_value<<endl;
//    exit(0);
}
void Tree::init_node(Node* node, int left, int right, int imode){
    node->_left_data_index = left;
    node->_right_data_index = right;
    NodeMode* nodemode = node->_nodemode;
    node->reset();
    for(int i=left; i<right; i++){
        int index = _data->data_reverse_order(i, 0);
        if(nodemode->___node_mode_ != bvdt::ONE_VS_ALL_R)
            nodemode->increment_node_mode(_data->train_p(index), _data->train_label(index), imode);
        else
            nodemode->increment_node_mode(_data->train_f(index), _data->train_label(index), imode);
    }
}

void Tree::find_best_mode(Node* node, NodeMode* nodemode, NodeGain* nodegain){
    float gradient, hessian;
    int nsamples;
    float best_gain = 0., best_value = 0.; int best_mode = 0;
    for(int i1 = 0; i1<_effective_node_mode_number; i1++){
        int imode = _effective_node_mode_index[i1];
        nsamples = nodemode->node_mode(gradient, hessian, imode);
//        if(gradient==0&&hessian==0){
//            nodemode->print();
//            cout<<node->_left_data_index<<", "<<node->_right_data_index<<endl;
//            cout<<nsamples<<endl;
//        }
        float gain = nodegain->node_gain(gradient, hessian, nsamples);
        if(gain>=best_gain){
            best_gain = gain;
            best_mode = imode;
            best_value = nodegain->_node_value;
        }
    }

    node->_node_gain = best_gain;
    node->_mode_id = best_mode;
    node->_value = best_value;
}

void Tree::calculate_node(Node* node, NodeMode* nodemode, NodeGain *nodegain, int imode){
    float gradient, hessian;
    int nsamples;
    nsamples = nodemode->node_mode(gradient, hessian, imode);
    nodegain->node_gain(gradient, hessian, nsamples);
    node->_node_gain = nodegain->_node_gain;
    node->_value = nodegain->_node_value;
    node->_mode_id = imode;
}

void Tree::build_tree_full_search(){
//    //DEBUG
//    cout<<"Tree build_tree_full_search!"<<endl;

    for(int i=0; i<4*_nleaves; i++)
        _node_modes[i]->set_effective_node_mode_id_array(_effective_node_mode_index.data(), _effective_node_mode_number);
    if(_root_node)
        _root_node->reset();
    _node_allocation_index = 0;
    _root_node = _node_array[_node_allocation_index++];
    init_node(_root_node, 0, _effective_data_number-1);
    split_node_full_search(_root_node);
    for(int i=1; i<_nleaves; i++){
        float best_gain = 0.;
        Node* node = NULL;
        _root_node->best_gain_node(node, best_gain);
        if(!node)
            break;
        node->_node_type =  NodeParameter::INTERNAL;
        _nleaves_1--;
        split_node_full_search(node->_left_child);
        split_node_full_search(node->_right_child);
    }
//    //DEBUG
//    cout<<"Tree build_tree_full_search! Ok"<<endl;
}

void Tree::build_tree_alternative(){
    for(int i=0; i<4*_nleaves; i++)
        _node_modes[i]->set_effective_node_mode_id_array(_effective_node_mode_index.data(), _effective_node_mode_number);
    if(_root_node)
        _root_node->reset();
    _node_allocation_index = 0;
    _root_node = _node_array[_node_allocation_index++];
    init_node(_root_node, 0, _effective_data_number-1);
    split_node_alternative(_root_node);
    for(int i=1; i<_nleaves; i++){
        float best_gain = 0.;
        Node* node = NULL;
        _root_node->best_gain_node(node, best_gain);
        if(!node)
            break;
        node->_node_type =  NodeParameter::INTERNAL;
        _nleaves_1 --;
        split_node_alternative(node->_left_child);
        split_node_alternative(node->_right_child);
    }

}

void Tree::split_node_full_search(Node* node){
    int left = node->_left_data_index;
    int right = node->_right_data_index;
    float gain, best_split_gain = -1.;
    int best_feature, best_feature_index, best_split;
    float val0, val1, best_cut;

    node->_left_child = _node_array[_node_allocation_index++];
    node->_right_child = _node_array[_node_allocation_index++];

    Node* node_left = node->_left_child;
    Node* node_right = node->_right_child;

    NodeMode* node_mode = node->_nodemode;
    NodeMode* node_mode_left = node_left->_nodemode;
    NodeMode* node_mode_right = node_right->_nodemode;
    gain = node->_node_gain;
    for(int i1 = 0; i1<_effective_feature_number; i1++){
        int ifeature = _effective_feature_index[i1];
        int ipoint = _data->data_reverse_order(left, i1);
        val1 = _data->train_data(ipoint)[ifeature];
        node_mode_left->copy_all_mode(node_mode);
        node_mode_right->reset();
        for(int i2= left; i2< right; i2++){
            if(node_mode_left->___node_mode_ != bvdt::ONE_VS_ALL_R){
                node_mode_left->decrement_all_node_mode(_data->train_p(ipoint), _data->train_label(ipoint));
                node_mode_right->increment_all_node_mode(_data->train_p(ipoint), _data->train_label(ipoint));
            }
            else{
                node_mode_left->decrement_all_node_mode(_data->train_f(ipoint), _data->train_label(ipoint));
                node_mode_right->increment_all_node_mode(_data->train_f(ipoint), _data->train_label(ipoint));
            }
            val0 = val1;
            ipoint = _data->data_reverse_order(i2+1, i1);
            val1 = _data->train_data(ipoint)[ifeature];
            if(val0 == val1)
                continue;
            find_best_mode(node_left, node_mode_left, _node_gain_leaf);
            find_best_mode(node_right, node_mode_right, _node_gain_leaf);
            float gain_left = node_left->_node_gain;
            float gain_right = node_right->_node_gain;
            float split_gain = gain_left + gain_right - gain;
            if(split_gain > best_split_gain){
                best_split_gain = split_gain;
                best_feature = ifeature;
                best_feature_index = i1;
                best_split = i2;
                best_cut = 0.5*(val0+val1);
            }
        }
    }
    if(best_split_gain!=-1.){
        node->_cut = best_cut;
        node->_feature = best_feature;
        _data->rearrange(left, right, best_split, best_feature_index);
        init_node(node->_left_child, left, best_split);
        init_node(node->_right_child, best_split+1, right);
    }
    node->_node_split_gain = best_split_gain;
    node->_node_type = NodeParameter::LEAF;
    _nleaves_1 ++;
}

void Tree::split_node_alternative(Node* node){
    int left = node->_left_data_index;
    int right = node->_right_data_index;
    float gain, best_split_gain = -1.;
    int best_feature, best_feature_index=-1, best_split;
    int ifeature, ipoint, imode0;
    float val0, val1, best_cut;

    node->_left_child = _node_array[_node_allocation_index++];
    node->_right_child = _node_array[_node_allocation_index++];

    Node* node_left = node->_left_child;
    Node* node_right = node->_right_child;

    NodeMode* node_mode = node->_nodemode;
    NodeMode* node_mode_left = node_left->_nodemode;
    NodeMode* node_mode_right = node_right->_nodemode;
    gain = node->_node_gain;
    imode0 = node->_mode_id;

    //fix node mode
    for(int i1 = 0; i1<_effective_feature_number; i1++){
        ifeature = _effective_feature_index[i1];
        ipoint = _data->data_reverse_order(left, i1);
        val1 = _data->train_data(ipoint)[ifeature];
        node_mode_left->copy_mode(node_mode, imode0);
        node_mode_right->reset(imode0);
        for(int i2= left; i2< right; i2++){
            if(node_mode_left->___node_mode_ != bvdt::ONE_VS_ALL_R){
                node_mode_left->decrement_node_mode(_data->train_p(ipoint), _data->train_label(ipoint), imode0);
                node_mode_right->increment_node_mode(_data->train_p(ipoint), _data->train_label(ipoint), imode0);
            }
            else{
                node_mode_left->decrement_node_mode(_data->train_f(ipoint), _data->train_label(ipoint), imode0);
                node_mode_right->increment_node_mode(_data->train_f(ipoint), _data->train_label(ipoint), imode0);
            }
            val0 = val1;
            ipoint = _data->data_reverse_order(i2+1, i1);
            val1 = _data->train_data(ipoint)[ifeature];
            if(val0 == val1)
                continue;
            calculate_node(node_left, node_mode_left, _node_gain_leaf, imode0);
            calculate_node(node_right, node_mode_right, _node_gain_leaf, imode0);
            float gain_left = node_left->_node_gain;
            float gain_right = node_right->_node_gain;
            float split_gain = gain_left + gain_right - gain;
            if(split_gain > best_split_gain){
                best_split_gain = split_gain;
                best_feature = ifeature;
                best_feature_index = i1;
                best_split = i2;
                best_cut = 0.5*(val0+val1);
            }
        }
    }

    //fix feature
    if(best_feature_index!=-1){
        ifeature = _effective_feature_index[best_feature_index];
        ipoint = _data->data_reverse_order(left, best_feature_index);
        val1 = _data->train_data(ipoint)[ifeature];
        node_mode_left->copy_all_mode(node_mode);
        node_mode_right->reset();
        for(int i2= left; i2< right; i2++){
            if(node_mode_left->___node_mode_ != bvdt::ONE_VS_ALL_R){
                node_mode_left->decrement_all_node_mode(_data->train_p(ipoint), _data->train_label(ipoint));
                node_mode_right->increment_all_node_mode(_data->train_p(ipoint), _data->train_label(ipoint));
            }
            else{
                node_mode_left->decrement_all_node_mode(_data->train_f(ipoint), _data->train_label(ipoint));
                node_mode_right->increment_all_node_mode(_data->train_f(ipoint), _data->train_label(ipoint));
            }
            val0 = val1;
            ipoint = _data->data_reverse_order(i2+1, best_feature_index);
            val1 = _data->train_data(ipoint)[ifeature];
            if(val0 == val1)
                continue;
            find_best_mode(node_left, node_mode_left, _node_gain_leaf);
            find_best_mode(node_right, node_mode_right, _node_gain_leaf);
            float gain_left = node_left->_node_gain;
            float gain_right = node_right->_node_gain;
            float split_gain = gain_left + gain_right - gain;
            if(split_gain > best_split_gain){
                best_split_gain = split_gain;
                best_split = i2;
                best_cut = 0.5*(val0+val1);
            }
        }
    }

    if(best_split_gain!=-1.){
        node->_cut = best_cut;
        node->_feature = best_feature;
        _data->rearrange(left, right, best_split, best_feature_index);
        init_node(node->_left_child, left, best_split);
        init_node(node->_right_child, best_split+1, right);
    }
    node->_node_split_gain = best_split_gain;
    node->_node_type = NodeParameter::LEAF;
    _nleaves_1++;
}

void Tree::calculate_leaf_value_all_data(){
    _root_node->reset_nodemode();
    for(int i=0; i<_data->_ntrain_samples; i++){
        _root_node->add_sample(_data->train_data(i), _data->train_p(i), _data->train_label(i));
    }
    _root_node->calculate_leaf_value(_node_gain_leaf);
}

void Tree::adjust_nleaves(int delta){
    int nleaves = _nleaves + delta;
    if(nleaves<2)
        nleaves = 2;
    int nk = 4*nleaves - int(_node_array.size());
    for(int k=0; k<nk; k++){
        Node* node = new Node();
        NodeMode* nodemode;
        switch(__node_mode){
        case bvdt::ONE_VS_ALL:
            nodemode = new NodeMode_One_Vs_All(_data->_nclasses,
                                                         bvdt::ONE_VS_ALL);
            break;
        case bvdt::ONE_VS_ONE:
            nodemode = new NodeMode_One_Vs_One(_data->_nclasses,
                                                         bvdt::ONE_VS_ONE);
            break;
        default:
            cout<<"This node mode has not been implemented yet!"<<endl;
            exit(0);
        }
        node->_left_child = NULL;
        node->_right_child = NULL;
        node->_node_type = NodeParameter::UNDEF;
        node->_nodemode = nodemode;

        _node_modes.push_back(nodemode);
        _node_array.push_back(node);
    }
    _nleaves = nleaves;
}

void Tree::build_tree(){
//    //DEBUG
//    cout<<"Tree: build_tree!"<<endl;

    _tree_id++;
    for(int i=0; i<_effective_data_number; i++){
        int index = ((i+_current_data_index)%_data_index.size());
        _effective_data_index[i] = _data_index[index];
    }
    _current_data_index += _effective_data_number;
    _current_data_index = (_current_data_index%_data_index.size());

    for(int i=0; i<_effective_feature_number; i++){
        int index = ((i+_current_feature_index)%_feature_index.size());
        _effective_feature_index[i] = _feature_index[index];
    }
    _current_feature_index += _effective_feature_number;
    _current_feature_index = (_current_feature_index%_feature_index.size());

    for(int i=0; i<_effective_node_mode_number; i++){
        int index = ((i+_current_node_mode_index)%_node_mode_index.size());
        _effective_node_mode_index[i] = _node_mode_index[index];
    }
    _current_node_mode_index += _effective_node_mode_number;
    _current_node_mode_index = (_current_node_mode_index%_node_mode_index.size());

    _data->init_data_order(_effective_data_index, _effective_feature_index);
    //build tree
    _nleaves_1 = 0;
    switch(_direction_type){
    case SolverParameter::FULL_SEARCH:
        build_tree_full_search();
        break;

    case SolverParameter::ALTERNATIVE:
        build_tree_alternative();
        break;

    default:
        cout<<"This direction type has not been implemented!"<<endl;
        exit(-1);
    }
    //calculate leaf value
    switch(_leaf_value_data){
    case SolverParameter::ALL_DATA:
        calculate_leaf_value_all_data();
        break;

    case SolverParameter::SAMPLE_DATA:
        break;

    default:
        cout<<"This leaf value type has not been implemented!"<<endl;
        exit(-1);
    }
    //Refit the leaf values
    if(_refit_type == SolverParameter_RefitType_L1)
        refit_leaf_values_l1();
    else if(_refit_type == SolverParameter_RefitType_L2)
        refit_leaf_values_l2();
}

void Tree::save_tree_parameter(){
    //save parameter
    NodeParameter* node_parameter = _tree_parameter.add_root_nodes();
    node_parameter->set_tree_id(_tree_id);
    node_parameter->set_nleaves (_nleaves_1);
    if(_save_accuracy){
        node_parameter->set_train_accuracy (_data->train_accuracy ());
        node_parameter->set_train_loss (_data->train_loss ());

        if(_data->_has_test_data){
            node_parameter->set_test_accuracy (_data->test_accuracy ());
            node_parameter->set_test_loss (_data->test_loss ());
        }
    }

    _root_node->fill_node_parameter(node_parameter, _tree_id);
}

void Tree::write_tree_parameter(const string output_file_name){
//    //DEBUG
//    cout<<"Tree write_tree_parameter"<<endl;

    FILE* fin = fopen(output_file_name.c_str(), "r");
    if(fin){
        fclose(fin);
        string cmd = "rm -fr ";
        cmd += output_file_name;
        system(cmd.c_str());
    }


    int output_fd = open(output_file_name.c_str(), O_WRONLY | O_CREAT | O_TRUNC, 0644);
    if(output_fd == -1){
        cout << "File : " << output_file_name<<"\t can not be open!"<<endl;
        exit(-1);
    }
    _tree_parameter.SerializeToFileDescriptor(output_fd);
    close(output_fd);
    _tree_parameter.clear_root_nodes();
}

void Tree::update_data(){
//    //DEBUG
//    cout<<"Tree update_data!"<<endl;
    int mode_id;
    float value;
//    float* delta_f = (float*)malloc(_data->_nclasses*sizeof(float));//new float[_data->_nclasses];
//    //DEBUG
//    cout<<"new float: "<<_data->_nclasses<<", "<<delta_f<<endl;
    for(int i=0; i<_data->_ntrain_samples; i++){
        value = _root_node->eval(_data->train_data(i), mode_id);
//        //DEBUG
//        cout<<"("<<mode_id<<", "<<value<<"),";
        _node_modes[0]->node_mode_value(_delta_f, mode_id);
//        if(_node_modes[0]->___node_mode_ != bvdt::ONE_VS_ALL_R)
            _data->update_train_f(_delta_f, value*_shrinkage, i);
//        else
//            _data->update_train_f(_delta_f, value*_shrinkage, i, true);
//        for(int j=0; j<_data->_nclasses; j++){
//            cout<<delta_f[j]<<", ";
//        }
//        cout<<":\t"<<value*_shrinkage<<endl;
    }

//    //DEBUG
//    cout<<endl;
//    cout<<"Tree update_data train Ok!"<<endl;
//    cin>>mode_id;
//    exit(0);
    for(int i=0; i<_data->_ntest_samples; i++){
        value = _root_node->eval(_data->test_data(i), mode_id);
//        //DEBUG
//        cout<<"("<<mode_id<<", "<<value<<"), ";
        _node_modes[0]->node_mode_value(_delta_f,  mode_id);
//        if(_node_modes[0]->___node_mode_ != bvdt::ONE_VS_ALL_R)
            _data->update_test_f(_delta_f, value*_shrinkage, i);
//        else
//            _data->update_test_f(_delta_f, value*_shrinkage, i, true);
//        for(int j=0; j<_data->_nclasses; j++){
//            cout<<delta_f[j]<<", ";
//        }
//        cout<<":\t"<<value*_shrinkage<<endl;
    }
//    //DEBUG
//    cout<<endl;
//    cout<<"Tree update_data test Ok! "<<delta_f<<endl;
//    free(delta_f);
//    exit(0);
}
