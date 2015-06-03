#include "nodemode_one_vs_all_r.h"
#include "math.h"
NodeMode_One_Vs_All_R::NodeMode_One_Vs_All_R(int nclass, bvdt::__NodeMode_ __node_mode_, float *param): NodeMode(nclass, __node_mode_)
{
    _nmodes = nclass;
    _g = new float[nclass];
    _h = new float[nclass];
    _effective_node_mode_id_array = new int[nclass];
    _dump_node_value = new float[nclass];
    for(int i=0; i<nclass; i++)
        _dump_node_value[i] = -1./_nclass;
    if(param)
        _r = param[0];
    else
        _r = 0.001;
}


void NodeMode_One_Vs_All_R::node_mode(float &gradient, float &hessian, const float *f, int label, int mode_id){
    float p, sump = 0., d, l, sumpd = 0.;
    d = exp(f[mode_id]);
    l = exp(f[label]);
    for(int i=0; i<_nclass; i++){
        float t = exp(f[i]);
        if(i != label){
            float s = t/l;
            if(s>2.)    s = 2.;
            sumpd += (i-label)*(i-label)*s;
        }
        sump += t;
    }
    p = d/sump;
    gradient = p;
    hessian = ((1-p)*p+_r);
    if(mode_id == label){
        gradient -= 1.;
        float tmp = _r*sumpd;
        gradient -= tmp;
        hessian += tmp;
    }
    else{
        float tmp = d/l;
        if(tmp>2.)
            tmp = 2.;
        tmp *= _r*(mode_id-label)*(mode_id-label);
        gradient += tmp;
        hessian += tmp;
    }
    hessian *= _nclass;
}

int NodeMode_One_Vs_All_R::node_mode(float &gradient, float &hessian, int mode_id){
    gradient = _g[mode_id];
    hessian = _h[mode_id];
    return _nsamples;
}

NodeMode_One_Vs_All_R::~NodeMode_One_Vs_All_R(){
    delete[] _g;
    delete[] _h;
    delete[] _effective_node_mode_id_array;
    delete[] _dump_node_value;
}

void NodeMode_One_Vs_All_R::reset(){
    memset(_g, 0, sizeof(float)*_nclass);
    memset(_h, 0, sizeof(float)*_nclass);
    _nsamples = 0;
}

void NodeMode_One_Vs_All_R::reset(int mode_id){
    _g[mode_id] = 0.;
    _h[mode_id];
    _nsamples = 0;
}

void NodeMode_One_Vs_All_R::copy_mode(NodeMode *nodemode, int mode_id){
    _nsamples = nodemode->_nsamples;
    _g[mode_id] = ((NodeMode_One_Vs_All_R*)nodemode)->_g[mode_id];
    _h[mode_id] = ((NodeMode_One_Vs_All_R*)nodemode)->_h[mode_id];
}

void NodeMode_One_Vs_All_R::copy_all_mode(NodeMode *nodemode){
    _nsamples = nodemode->_nsamples;
    memcpy(_g, ((NodeMode_One_Vs_All_R*)nodemode)->_g, sizeof(float)*_nclass);
    memcpy(_h, ((NodeMode_One_Vs_All_R*)nodemode)->_h, sizeof(float)*_nclass);
}

void NodeMode_One_Vs_All_R::node_mode_value(float *value, int node_mode_id){
    memcpy(value, _dump_node_value, _nclass*sizeof(float));
    value[node_mode_id] += 1.;
}
void NodeMode_One_Vs_All_R::print(){
    for(int i=0; i<_n_effective_node_mode_id; i++){
        int mode = _effective_node_mode_id_array[i];
        float gradient, hessian;
        node_mode(gradient, hessian, mode);
        std::cout<<"Mode:\t"<<mode<<", gradient:\t"<<gradient<<", hessian:\t"<<hessian<<std::endl;
    }
}

void NodeMode_One_Vs_All_R::increment_all_node_mode(const float* f, int label){
//    std::cout<<"Add:\n";
    for(int i=0; i<_n_effective_node_mode_id; i++){
        int inode_mode_id = _effective_node_mode_id_array[i];
        float gradient, hessian;
        node_mode(gradient, hessian, f, label, inode_mode_id);
        _g[inode_mode_id] += gradient;
        _h[inode_mode_id] += hessian;
    }
//    std::cout<<std::endl;
    ++_nsamples;
}

void NodeMode_One_Vs_All_R::increment_node_mode(const float *f, int label, int mode_id){
    float gradient, hessian;
    node_mode(gradient, hessian, f, label, mode_id);
    _g[mode_id] += gradient;
    _h[mode_id] += hessian;
    ++_nsamples;
}

void NodeMode_One_Vs_All_R::decrement_all_node_mode(const float* f, int label){
    for(int i=0; i<_n_effective_node_mode_id; i++){
        int inode_mode_id = _effective_node_mode_id_array[i];
        float gradient, hessian;
        node_mode(gradient, hessian, f, label, inode_mode_id);
        _g[inode_mode_id] -= gradient;
        _h[inode_mode_id] -= hessian;
    }
    _nsamples--;
}

void NodeMode_One_Vs_All_R::decrement_node_mode(const float *f, int label, int mode_id){
    float gradient, hessian;
    node_mode(gradient, hessian, f, label, mode_id);
    _g[mode_id] -= gradient;
    _h[mode_id] -= hessian;
    _nsamples--;
}

void NodeMode_One_Vs_All_R::set_effective_node_mode_id_array(const int *effective_node_mode_id_array, int nmode){
    _n_effective_node_mode_id = nmode;
    memcpy(_effective_node_mode_id_array, effective_node_mode_id_array, nmode*sizeof(int));
}
const int* NodeMode_One_Vs_All_R::get_effective_node_mode_id_array(int &n_effective_node_mode_id){
    n_effective_node_mode_id = _n_effective_node_mode_id;
    return _effective_node_mode_id_array;
}
