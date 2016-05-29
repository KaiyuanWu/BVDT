#ifndef L2SOLVER_H
#define L2SOLVER_H
#include "dlib/optimization.h"
class L2Solver
{
public:
    L2Solver(int nx_, double threshold_);
    int solve(double *H_, double *g_, double *x_);
private:
    dlib::matrix<double> B;
    dlib::matrix<double> g;
    dlib::matrix<double> p;
    int nx;
    double threshold;
};

#endif // L2SOLVER_H
