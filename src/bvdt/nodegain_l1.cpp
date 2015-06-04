#include "nodegain_l1.h"
#include <math.h>

NodeGain_L1::NodeGain_L1(SolverParameter::RegularizationType regularization_type, float C)
    :NodeGain(regularization_type){
    _C = C;
}

float NodeGain_L1::node_gain(float gradient, float hessian, int nsamples){
    node_value(gradient, hessian);
    _node_gain = - (gradient + 0.5*hessian*_node_value)*_node_value - _C*fabs(_node_value);
    return _node_gain;
}

float NodeGain_L1::node_value(float gradient, float hessian, int nsamples){
    float u = -(gradient+_C)/hessian;
    if(!(u>0)) u=0.;
    float v = (gradient-_C)/hessian;
    if(!(v>0)) v=0.;
    float value = u-v;
    _node_value = value;
    return value;
}
