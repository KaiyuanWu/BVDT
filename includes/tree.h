#ifndef TREE_H
#define TREE_H
#include "stdio.h"
#include "stdlib.h"
#include <unistd.h>
#include "algo.pb.h"
#include "node.h"
#include "nodegain.h"
#include "nodemode.h"
#include "nodemode_one_vs_one.h"
#include "nodemode_one_vs_all.h"
#include "nodemode_one_vs_all_r.h"
#include "data.h"
#include "fcntl.h"
#include <string>
#include <iostream>
#include <algorithm>

using namespace std;
class Tree
{
public:
    Tree();
    void build_tree();
    void update_data();
    void init_tree(Data* data,
                   NodeGain* node_gain_split, NodeGain* node_gain_leaf, SolverParameter *solver_parameter);
    void write_tree_parameter(const string output_file_name);
    void reshuffle();
    void adjust_nleaves(int delta);
    int  nleaves_1();
    int  nleaves();
private:
    NodeGain* _node_gain_split;
    NodeGain* _node_gain_leaf;
    vector<NodeMode*> _node_modes;
    Data* _data;
    Node* _root_node;
    vector<Node*> _node_array;
    int _node_allocation_index;
    int    _nleaves;
    int    _nleaves_1;
    int    _tree_id;
    bool   _save_accuracy;
    float  _shrinkage;
    float* _delta_f;
    SolverParameter::DirectionType _direction_type;
    SolverParameter::LeafValueData _leaf_value_data;
    bvdt::__NodeMode_   __node_mode;

    void get_effective_setting();
    vector<int> _data_index;
    vector<int> _node_mode_index;
    vector<int> _feature_index;

    vector<int> _effective_data_index;
    vector<int> _effective_node_mode_index;
    vector<int> _effective_feature_index;

    int _effective_data_number;
    int _effective_node_mode_number;
    int _effective_feature_number;

    int _current_data_index;
    int _current_node_mode_index;
    int _current_feature_index;
    TreeParameter _tree_parameter;

    inline void init_node(Node* node, int left, int right);
    inline void find_best_mode(Node* node, NodeMode* nodemode, NodeGain *nodegain);
    inline void calculate_node(Node* node, NodeMode* nodemode, NodeGain *nodegain, int imode);
    inline void init_node(Node* node, int left, int right, int imode);
    void build_tree_full_search();
    void build_tree_alternative();

    void split_node_full_search(Node* node);
    void split_node_alternative(Node* node);

    void calculate_leaf_value_all_data();
};

#endif // TREE_H
