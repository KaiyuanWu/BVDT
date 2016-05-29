#include "nodemode_one_vs_all.h"

NodeMode_One_Vs_All::NodeMode_One_Vs_All(int nclass, bvdt::__NodeMode_ __node_mode_, float *param): NodeMode(nclass, __node_mode_)
{
    _nmodes = nclass;
    _g = new float[nclass];
    _h = new float[nclass];
    _effective_node_mode_id_array = new int[nclass];
    _dump_node_value = new float[nclass];
    for(int i=0; i<nclass; i++)
        _dump_node_value[i] = -1./_nclass;
}

void NodeMode_One_Vs_All::node_mode(float &gradient, float &hessian, const float *p, int label, int mode_id){
    gradient = p[mode_id];
    if(label == mode_id)
        gradient -= 1.;
    hessian = _nclass*(1-p[mode_id])*p[mode_id];
}

int NodeMode_One_Vs_All::node_mode(float &gradient, float &hessian, int mode_id){
    gradient = _g[mode_id];
    hessian = _h[mode_id];
    return _nsamples;
}

NodeMode_One_Vs_All::~NodeMode_One_Vs_All(){
    delete[] _g;
    delete[] _h;
    delete[] _effective_node_mode_id_array;
    delete[] _dump_node_value;
}

void NodeMode_One_Vs_All::reset(){
    memset(_g, 0, sizeof(float)*_nclass);
    memset(_h, 0, sizeof(float)*_nclass);
    _nsamples = 0;
}

void NodeMode_One_Vs_All::reset(int mode_id){
    _g[mode_id] = 0.;
    _h[mode_id] = 0.;
    _nsamples = 0;
}

void NodeMode_One_Vs_All::copy_mode(NodeMode *nodemode, int mode_id){
    _nsamples = nodemode->_nsamples;
    _g[mode_id] = ((NodeMode_One_Vs_All*)nodemode)->_g[mode_id];
    _h[mode_id] = ((NodeMode_One_Vs_All*)nodemode)->_h[mode_id];
}

void NodeMode_One_Vs_All::copy_all_mode(NodeMode *nodemode){
    _nsamples = nodemode->_nsamples;
    memcpy(_g, ((NodeMode_One_Vs_All*)nodemode)->_g, sizeof(float)*_nclass);
    memcpy(_h, ((NodeMode_One_Vs_All*)nodemode)->_h, sizeof(float)*_nclass);
}

void NodeMode_One_Vs_All::node_mode_value(float *value, int node_mode_id){
    memcpy(value, _dump_node_value, _nclass*sizeof(float));
    value[node_mode_id] += 1.;
}
void NodeMode_One_Vs_All::print(){
    for(int i=0; i<_n_effective_node_mode_id; i++){
        int mode = _effective_node_mode_id_array[i];
        float gradient, hessian;
        node_mode(gradient, hessian, mode);
        std::cout<<"Mode:\t"<<mode<<", gradient:\t"<<gradient<<", hessian:\t"<<hessian<<std::endl;
    }
}

void NodeMode_One_Vs_All::increment_all_node_mode(const float* p, int label){
//    std::cout<<"Add:\n";
    for(int i=0; i<_n_effective_node_mode_id; i++){
        int inode_mode_id = _effective_node_mode_id_array[i];
//        std::cout<<p[inode_mode_id]<<"\t";
        if(label == inode_mode_id)
            _g[inode_mode_id] +=(-1.+p[inode_mode_id]);
        else
            _g[inode_mode_id] += p[inode_mode_id];
        _h[inode_mode_id] += _nclass*(1.-p[inode_mode_id])*p[inode_mode_id];
    }
//    std::cout<<std::endl;
    ++_nsamples;
}

void NodeMode_One_Vs_All::increment_node_mode(const float *p, int label, int mode_id){
    if(label == mode_id)
        _g[mode_id] +=(-1.+p[mode_id]);
    else
        _g[mode_id] += p[mode_id];
    _h[mode_id] += _nclass*(1.-p[mode_id])*p[mode_id];
    ++_nsamples;
}

void NodeMode_One_Vs_All::decrement_all_node_mode(const float* p, int label){
    for(int i=0; i<_n_effective_node_mode_id; i++){
        int inode_mode_id = _effective_node_mode_id_array[i];
        if(label == inode_mode_id)
            _g[inode_mode_id] -=(-1.+p[inode_mode_id]);
        else
            _g[inode_mode_id] -= p[inode_mode_id];
        _h[inode_mode_id] -= _nclass*(1.-p[inode_mode_id])*p[inode_mode_id];
    }
    _nsamples--;
}

void NodeMode_One_Vs_All::decrement_node_mode(const float *p, int label, int mode_id){
    if(label == mode_id)
        _g[mode_id] -=(-1.+p[mode_id]);
    else
        _g[mode_id] -= p[mode_id];
    _h[mode_id] -= _nclass*(1.-p[mode_id])*p[mode_id];
    _nsamples--;
}

void NodeMode_One_Vs_All::set_effective_node_mode_id_array(const int *effective_node_mode_id_array, int nmode){
    _n_effective_node_mode_id = nmode;
    memcpy(_effective_node_mode_id_array, effective_node_mode_id_array, nmode*sizeof(int));
}
const int* NodeMode_One_Vs_All::get_effective_node_mode_id_array(int &n_effective_node_mode_id){
    n_effective_node_mode_id = _n_effective_node_mode_id;
    return _effective_node_mode_id_array;
}
