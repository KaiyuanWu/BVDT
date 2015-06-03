#ifndef NODEGAIN_H
#define NODEGAIN_H
#include "algo.pb.h"
#include "log.h"
#include <limits>
using namespace bvdt;
class NodeGain
{
public:
    NodeGain(SolverParameter::RegularizationType regularization_type);
    virtual float node_gain(float gradient, float hessian = 0 , int nsamples = 0) = 0;
    virtual float node_value(float gradient, float hessian = 0 ,int nsamples = 0) = 0;

    SolverParameter::RegularizationType _regularization_type;
    float _node_gain;
    float _node_value;
};

#endif // NODEGAIN_H
