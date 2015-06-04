#ifndef SOLVER_H
#define SOLVER_H
#include "node.h"
#include "nodegain.h"
#include "nodegain_gradient.h"
#include "nodegain_l1.h"
#include "nodegain_l2.h"
#include "nodegain_l1l2.h"
#include "nodegain_no_regularization.h"
#include "nodemode.h"
#include "nodemode_one_vs_all.h"
#include "nodemode_one_vs_one.h"
#include "data.h"
#include "tree.h"
#include "algo.pb.h"
#include <google/protobuf/text_format.h>
#include <fcntl.h>
#include <google/protobuf/io/coded_stream.h>
#include <google/protobuf/io/zero_copy_stream_impl.h>
#include "log.h"

class Solver
{
public:
    Solver(string solver_proto);
    void init();
    void start();
private:
    NodeGain* _node_gain_split;
    NodeGain* _node_gain_leaf;
    Data* _data;
    Tree* _tree;
    float _convergence_rate;
    bool  _has_convergence_rate;

    SolverParameter _solver_parameter;
    string _solver_proto;
};

#endif // SOLVER_H
