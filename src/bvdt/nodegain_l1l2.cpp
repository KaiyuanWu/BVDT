#include "nodegain_l1l2.h"
NodeGain_L1L2::NodeGain_L1L2(SolverParameter::RegularizationType regularization_type, float C, float ratio)
    :NodeGain(regularization_type){
    _C = C;
    _ratio = ratio;
    _C1 = C*_ratio;
    _C2 = C*(1.-_ratio);
}

float NodeGain_L1L2::node_gain(float gradient, float hessian, int nsamples){
    node_value(gradient, hessian);
    //(gradient + 0.5*(hessian+_C)*value)*value
    _node_gain = - (gradient + 0.5*(hessian+_C2)*_node_value)*_node_value - _C1*fabs(_node_value);
    return _node_gain;
}

float NodeGain_L1L2::node_value(float gradient, float hessian, int nsamples){
    float u = -(gradient+_C1)/(hessian+_C2);
    if(!(u>0)) u=0.;
    float v = (gradient-_C1)/(hessian+_C2);
    if(!(v>0)) v=0.;
    float value = u-v;
    _node_value = value;
    return value;
}
