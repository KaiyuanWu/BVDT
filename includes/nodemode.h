#ifndef NODEMODE_H
#define NODEMODE_H
#include "algo.pb.h"
#include "log.h"
#include <vector>
#include <iostream>
class NodeMode
{
public:
    NodeMode(int nclass, bvdt::__NodeMode_ __node_mode_, float* param=NULL);
    virtual void node_mode(float& gradient, float& hessian, const float* p, int label, int mode_id) = 0;
    virtual int node_mode(float &gradient, float &hessian, int mode_id) = 0;
    virtual void node_mode_value(float* value, int node_mode_id) = 0;
    virtual void increment_all_node_mode(const float* p, int label) = 0;
    virtual void increment_node_mode(const float* p, int label, int mode_id) = 0;
    virtual void decrement_all_node_mode(const float* p, int label) = 0;
    virtual void decrement_node_mode(const float* p, int label, int mode_id) = 0;
    virtual void print() = 0;

    virtual void set_effective_node_mode_id_array(const int* effective_node_mode_id_array, int nmode) = 0;
    virtual const int* get_effective_node_mode_id_array(int& n_effective_node_mode_id) = 0;
    virtual void reset() =0;
    virtual void reset(int mode_id) =0;
    virtual void copy_mode(NodeMode* nodemode, int mode_id) = 0;
    virtual void copy_all_mode(NodeMode* nodemode) = 0;

    int  _nclass;
    int  _nmodes;
    int  _nsamples;

    int* _effective_node_mode_id_array;
    int  _n_effective_node_mode_id;
    bvdt::__NodeMode_ ___node_mode_;
};

#endif // NODEMODE_H
