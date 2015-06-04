#include "nodegain_no_regularization.h"
#include <math.h>

NodeGain_No_Regularization::NodeGain_No_Regularization(SolverParameter::RegularizationType regularization_type, float maxz): NodeGain(regularization_type)
{
    if(maxz>0){
        _has_maxz = true;
        _maxz = maxz;
    }
    else
        _has_maxz = false;
}

float NodeGain_No_Regularization::node_gain(float gradient, float hessian, int nsamples){
    node_value(gradient, hessian);
    _node_gain = -(gradient + 0.5*hessian*_node_value)*_node_value;
    return _node_gain;
}

float NodeGain_No_Regularization::node_value(float gradient, float hessian, int nsamples){
    if(hessian <= 0 ){
        if(!_has_maxz){
            if(gradient==0){
                _node_value = 0;
                return 0;
            }
            _node_value = std::numeric_limits<float>::max();
            return _node_value;
        }
        else{
            if(gradient<0){
                _node_value = _maxz;
                return _maxz;
            }
            else{
                _node_value = -_maxz;
                return -_maxz;
            }
        }
    }
    float value = - gradient/hessian;
    if(!(fabs(value)<_maxz)){
        if(gradient<0){
            _node_value = _maxz;
            return _maxz;
        }
        else{
            _node_value = -_maxz;
            return _node_value;
        }
    }
    else{
        _node_value = value;
        return value;
    }
}
