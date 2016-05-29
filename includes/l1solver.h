#ifndef L1SOLVER_H
#define L1SOLVER_H
#include <iostream>

#include "QpGenVars.h"
#include "QpGenResiduals.h"
#include "GondzioSolver.h"
#include "cQpGen.h"
using namespace std;
class L1Solver
{
public:
    L1Solver(int nx, double threshold);
    ~L1Solver();
    int solve(double *H, double *g, double &objectiveValue, double *x);
private:
    int nx_;
    double * c;
    int nx;
    double * Q;
    double * xlow;
    char   * ixlow;
    double * xupp;
    char   * ixupp;
    double * A;
    int my;
    double * b;
    double * C;
    int mz;
    double * clow;
    char   * iclow;
    double * cupp;
    char   * icupp;
    int  ierr;
    QpGenVars      * vars   ;
    QpGenResiduals * resid ;
    QpGenContext   ctx;
};

#endif // L1SOLVER_H
