#ifndef APPLICATION_H
#define APPLICATION_H
#include "algo.pb.h"
#include "node.h"
#include "solver.h"
#include "google/protobuf/message.h"
#include "google/protobuf/text_format.h"
#include <google/protobuf/io/coded_stream.h>
#include <google/protobuf/io/zero_copy_stream_impl.h>
#include <iostream>
#include <fstream>
#include "nodemode.h"
#include "nodemode_one_vs_one.h"
#include "nodemode_one_vs_all.h"
#include "nodemode_one_vs_all_r.h"

class Application
{
public:
    Application();
    ~Application();
    bool init(const string& solver_proto);
    int eval(const float* x, float* val);
    int eval(const float* x, float* val, int itree, bool reset_val = true);
    int eval(const float *x, int *rlabel);
    float accuracy(const float* x, int* label, int nsamples, int *rlabel=NULL);
    float accuracy(const float* x, int* label, int nsamples, const char* outfile_prefix);
    const SolverParameter* solver_parameter();
    int nvariables();
    int nclasses();
    int maximum_iterations();
private:
    bool init(const string& snapshot, int interval, int maximum_iterations);
    string parameter_name(SolverParameter* solver_parameter);
    string _snapshot;
    int _interval;
    int _maximum_iterations;
    TreeParameter _tree;
    SolverParameter _solver_parameter;
    bool _has_init;
    int _nvariables;
    int _nclasses;
    float* _f;
    NodeMode* _node_mode;
    bvdt::__NodeMode_ _node_mode_type;
};

#endif // APPLICATION_H
