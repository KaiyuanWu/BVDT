#ifndef NODEMODE_ONE_VS_ALL_H
#define NODEMODE_ONE_VS_ALL_H
#include "nodemode.h"
class NodeMode_One_Vs_All : public NodeMode
{
public:
    NodeMode_One_Vs_All(int nclass, bvdt::__NodeMode_ __node_mode_, float* param=NULL);
    ~NodeMode_One_Vs_All();
    void node_mode(float& gradient, float& hessian, const float* p, int label, int mode_id);
    int node_mode(float &gradient, float &hessian, int mode_id);
    void node_mode_value(float* value, int node_mode_id);
    void increment_all_node_mode(const float* p, int label);
    void increment_node_mode(const float* p, int label, int mode_id);
    void decrement_all_node_mode(const float* p, int label);
    void decrement_node_mode(const float* p, int label, int mode_id);
    void update_children(const float* p, int label, int mode_id = -1);
    void set_effective_node_mode_id_array(const int* effective_node_mode_id_array, int nmode);
    const int* get_effective_node_mode_id_array(int& n_effective_node_mode_id);
    void reset();
    void reset(int mode_id);
    void copy_mode(NodeMode *nodemode, int mode_id);
    void copy_all_mode(NodeMode *nodemode);
    void print();
private:
    float* _g;
    float* _h;
    float* _dump_node_value;
};

#endif // NODEMODE_ONE_VS_ALL_H
