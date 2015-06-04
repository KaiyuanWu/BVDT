#include "nodemode_one_vs_one.h"

NodeMode_One_Vs_One::NodeMode_One_Vs_One(int nclass, bvdt::__NodeMode_ __node_mode_, float *param): NodeMode(nclass, __node_mode_)
{
    _nmodes = nclass*(nclass-1)/2;
    _nclass = nclass;
    _g = new float[_nclass];
    _h1 = new float[_nclass];
    _h2 = new float[_nclass*_nclass];
    _effective_node_mode_id_array = new int[_nclass*_nclass];
    _effective_class = new int[_nclass];
    _dummy_r = new int[_nmodes];
    _dummy_s = new int[_nmodes];
    int c=0;
    for(int i=0; i<_nclass; i++){
        for(int j=0; j<i; j++){
            _dummy_r[c] = i;
            _dummy_s[c] = j;
            c++;
        }
    }
}

void NodeMode_One_Vs_One::node_mode(float &gradient, float &hessian, const float *p, int label, int mode_id){
    int r = _dummy_r[mode_id];
    int s = _dummy_s[mode_id];
    gradient = p[r] - p[s];
    if(label == r)
        gradient -= 1.;
    else if(label == s)
        gradient += 1.;
    hessian = (1-p[r])*p[r] + (1-p[s])*p[s] + 2*p[r]*p[s];
}

int NodeMode_One_Vs_One::node_mode(float &gradient, float &hessian, int mode_id){
    int r = _dummy_r[mode_id];
    int s = _dummy_s[mode_id];
//    std::cout<<"r = "<<r<<", s= "<<s<<",  mode_id = "<<mode_id<<std::endl;
    gradient = _g[r] - _g[s];
    hessian = _h1[r] + _h1[s] + 2*_h2[mode_id];
    return _nsamples;
}

NodeMode_One_Vs_One::~NodeMode_One_Vs_One(){
    delete[] _g;
    delete[] _h1;
    delete[] _h2;
    delete[] _effective_node_mode_id_array;
    delete[] _effective_class;
    delete[] _dummy_r;
    delete[] _dummy_s;
}

void NodeMode_One_Vs_One::reset(){
    _nsamples = 0;
    memset(_g, 0, sizeof(float)*_nclass);
    memset(_h1, 0, sizeof(float)*_nclass);
    memset(_h2, 0, sizeof(float)*_nclass*_nclass);
}

void NodeMode_One_Vs_One::reset(int mode_id){
    _nsamples = 0;
    int r = _dummy_r[mode_id];
    int s = _dummy_s[mode_id];
    _g[r] = 0.;
    _g[s] = 0.;
    _h1[r] = 0.;
    _h1[s] = 0.;
    _h2[mode_id] = 0.;
}
void NodeMode_One_Vs_One::copy_mode(NodeMode *nodemode, int mode_id){
    _nsamples = nodemode->_nsamples;
    _g[mode_id] = ((NodeMode_One_Vs_One*)nodemode)->_g[mode_id];
    _h1[mode_id] = ((NodeMode_One_Vs_One*)nodemode)->_h1[mode_id];
    _h2[mode_id] = ((NodeMode_One_Vs_One*)nodemode)->_h2[mode_id];
}

void NodeMode_One_Vs_One::copy_all_mode(NodeMode *nodemode){
    _nsamples = nodemode->_nsamples;
    memcpy(_g, ((NodeMode_One_Vs_One*)nodemode)->_g, sizeof(float)*_nclass);
    memcpy(_h1, ((NodeMode_One_Vs_One*)nodemode)->_h1, sizeof(float)*_nclass);
    memcpy(_h2, ((NodeMode_One_Vs_One*)nodemode)->_h2, sizeof(float)*_nclass*_nclass);
}

void NodeMode_One_Vs_One::node_mode_value(float *value, int node_mode_id){
    int r = _dummy_r[node_mode_id];
    int s = _dummy_s[node_mode_id];
    memset(value, 0, sizeof(float)*_nclass);
    value[r] = 1.;
    value[s] = -1.;
}
void NodeMode_One_Vs_One::increment_all_node_mode(const float* p, int label){
    for(int i=0; i<_n_effective_class; i++){
        if(label == _effective_class[i])
            _g[_effective_class[i]] +=(-1.+p[_effective_class[i]]);
        else
            _g[_effective_class[i]] += p[_effective_class[i]];
        _h1[_effective_class[i]] += (1.-p[_effective_class[i]])*p[_effective_class[i]];
    }
    for(int i=0; i<_n_effective_node_mode_id; i++){
        int id = _effective_node_mode_id_array[i];
        int r = _dummy_r[id];
        int s = _dummy_s[id];
        _h2[id] +=p[r]*p[s];
    }
    ++_nsamples;
}

void NodeMode_One_Vs_One::increment_node_mode(const float *p, int label, int mode_id){
    int r = _dummy_r[mode_id];
    int s = _dummy_s[mode_id];
    if(label == r)
        _g[r] +=(-1.+p[r]);
    else
        _g[r] += p[r];

    if(label == s)
        _g[s] +=(-1.+p[s]);
    else
        _g[s] += p[s];

    _h1[r] += (1.-p[r])*p[r];
    _h1[s] += (1.-p[s])*p[s];
    _h2[mode_id] += p[r]*p[s];
    ++_nsamples;
}

void NodeMode_One_Vs_One::decrement_all_node_mode(const float* p, int label){
    for(int i=0; i<_n_effective_class; i++){
        if(label == _effective_class[i])
            _g[_effective_class[i]] -=(-1.+p[_effective_class[i]]);
        else
            _g[_effective_class[i]] -= p[_effective_class[i]];
        _h1[_effective_class[i]] -= (1.-p[_effective_class[i]])*p[_effective_class[i]];
    }
    for(int i=0; i<_n_effective_node_mode_id; i++){
        int id = _effective_node_mode_id_array[i];
        int r = _dummy_r[id];
        int s = _dummy_s[id];
        _h2[id] -=p[r]*p[s];
    }
    _nsamples--;
}

void NodeMode_One_Vs_One::decrement_node_mode(const float *p, int label, int mode_id){
    int r = _dummy_r[mode_id];
    int s = _dummy_s[mode_id];
    if(label == r)
        _g[r] -=(-1.+p[r]);
    else
        _g[r] -= p[r];

    if(label == s)
        _g[s] -=(1.-p[s]);
    else
        _g[s] -= p[s];

    _h1[r] -= (1.-p[r])*p[r];
    _h1[s] -= (1.-p[s])*p[s];
    _h2[mode_id] -= p[r]*p[s];
    _nsamples--;
}
void NodeMode_One_Vs_One::print(){
    for(int i=0; i<_n_effective_node_mode_id; i++){
        int mode = _effective_node_mode_id_array[i];
        float gradient, hessian;
        node_mode(gradient, hessian, mode);
        std::cout<<"Mode:\t"<<mode<<"("<<_dummy_r[mode]<<", "<<_dummy_s[mode]<<"), gradient:\t"<<gradient<<", hessian:\t"<<hessian<<std::endl;
    }

//    float gradient, hessian;
//    node_mode(gradient, hessian, mode);
//    std::cout<<"Mode:\t"<<mode<<"("<<_dummy_r[mode]<<", "<<_dummy_s[mode]<<"), gradient:\t"<<gradient<<", hessian:\t"<<hessian<<std::endl;

}

void NodeMode_One_Vs_One::set_effective_node_mode_id_array(const int *effective_node_mode_id_array, int nmode){
    _n_effective_node_mode_id = nmode;
    memset(_effective_class, 0, sizeof(int)*_nclass);
    for(int i=0; i<nmode; i++){
//        std::cout<<effective_node_mode_id_array[i]<<", ";
        int r = _dummy_r[effective_node_mode_id_array[i]];
        int s = _dummy_s[effective_node_mode_id_array[i]];
        _effective_node_mode_id_array[i] = effective_node_mode_id_array[i];
        _effective_class[r] = 1;
        _effective_class[s] = 1;
    }
//    std::cout<<std::endl;
//    for(int i=0; i<_nclass; i++)
//        std::cout<<_effective_class[i]<<", ";
//    std::cout<<std::endl;
    int i=0, j=_nclass-1;
    while(j>=i){
        if(_effective_class[i] == 1 ){
            _effective_class[i] = i;
            i++;
        }
        else{
            if(_effective_class[j] == 0){
                j--;
            }
            else{
                _effective_class[i] = j;
                i++;
                j--;
            }
        }
    }
    _n_effective_class = i;
//    for(int i=0; i<_n_effective_class; i++){
//        std::cout<<_effective_class[i]<<", ";
//    }
//    std::cout<<std::endl;
}
const int* NodeMode_One_Vs_One::get_effective_node_mode_id_array(int &n_effective_node_mode_id){
    n_effective_node_mode_id = _n_effective_node_mode_id;
    return _effective_node_mode_id_array;
}
