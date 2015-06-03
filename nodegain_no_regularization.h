#ifndef NODEGAIN_NO_REGULARIZATION_H
#define NODEGAIN_NO_REGULARIZATION_H
#include "nodegain.h"
class NodeGain_No_Regularization : public NodeGain
{
public:
    NodeGain_No_Regularization(SolverParameter::RegularizationType regularization_type, float maxz=0);
    float node_gain(float gradient, float hessian, int nsamples = 0);
    float node_value(float gradient, float hessian, int nsamples = 0);

private:
    float _maxz;
    bool  _has_maxz;
};

#endif // NODEGAIN_NO_REGULARIZATION_H
