#ifndef NODEGAIN_L1_H
#define NODEGAIN_L1_H
#include "nodegain.h"
class NodeGain_L1: public NodeGain
{
public:
    NodeGain_L1(SolverParameter::RegularizationType regularization_type, float C);
    float node_gain(float gradient, float hessian, int nsamples = 0);
    float node_value(float gradient, float hessian, int nsamples = 0);

private:
    float _C;
};

#endif // NODEGAIN_L1_H
