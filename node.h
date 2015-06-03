#ifndef NODE_H
#define NODE_H
#include "stdio.h"
#include "stdlib.h"
#include "algo.pb.h"
#include "nodemode.h"
#include "nodegain.h"
#include "log.h"
using namespace bvdt;
class Node
{
public:
    Node();
    float _cut;
    int   _feature;
    NodeParameter::NodeType _node_type;
    float _value;
    int   _mode_id;
    bool   _is_root_node;
    int   _tree_id;

    Node* _left_child;
    Node* _right_child;
    void add_sample(const float *x, const float *p, int label);
    void calculate_leaf_value(NodeGain* nodegain);
    float eval(const float *x, int &mode_id);
    void fill_node_parameter(NodeParameter* node_parameter, int tree_id);
    void best_gain_node(Node *&node, float& best_gain);

    void   reset();
    void   reset_nodemode();
    float  _node_gain;
    float  _node_split_gain;
    void printInfo(const char* indent, bool last);

public:
    int _left_data_index;
    int _right_data_index;
public:
    NodeMode* _nodemode;
};

#endif // NODE_H
