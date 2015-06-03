#include "nodegain_gradient.h"

NodeGain_Gradient::NodeGain_Gradient(SolverParameter::RegularizationType regularization_type):NodeGain(regularization_type)
{
}
float NodeGain_Gradient::node_gain(float gradient, float hessian, int nsamples){
    _node_value = -gradient/nsamples;
    _node_gain = -_node_value*gradient;
    return _node_gain;
}

float NodeGain_Gradient::node_value(float gradient, float hessian,int nsamples){
    _node_value = -gradient/nsamples;
    return _node_value;
}
