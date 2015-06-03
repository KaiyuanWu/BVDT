#ifndef NODEGAIN_GRADIENT_H
#define NODEGAIN_GRADIENT_H
#include "nodegain.h"
class NodeGain_Gradient : public NodeGain
{
public:
    NodeGain_Gradient(SolverParameter::RegularizationType regularization_type);
    float node_gain(float gradient, float hessian, int nsamples = 0);
    float node_value(float gradient, float hessian,int nsamples = 0);
};

#endif // NODEGAIN_GRADIENT_H
