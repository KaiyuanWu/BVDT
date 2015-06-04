#ifndef NODEGAIN_L1L2_H
#define NODEGAIN_L1L2_H
#include "nodegain.h"
#include "math.h"
class NodeGain_L1L2:public NodeGain
{
public:
    NodeGain_L1L2(SolverParameter::RegularizationType regularization_type, float C, float ratio);
    float node_gain(float gradient, float hessian, int nsamples = 0);
    float node_value(float gradient, float hessian,int nsamples = 0);

private:
    float _C;
    float _C1;
    float _C2;
    float _ratio;
};

#endif // NODEGAIN_L1L2_H
