#include "nodegain_l2.h"
#include <math.h>

NodeGain_L2::NodeGain_L2(SolverParameter::RegularizationType regularization_type, float C)
    :NodeGain(regularization_type){
    _C = C;
}

float NodeGain_L2::node_gain(float gradient, float hessian, int nsamples){
    node_value(gradient, hessian);
    return -(gradient + 0.5*(hessian+_C)*_node_value)*_node_value;
}

float NodeGain_L2::node_value(float gradient, float hessian, int nsamples){
    float value = -gradient/(hessian+_C);
    if(value != value)
        value = 0.;
    _node_value = value;
    return value;
}
