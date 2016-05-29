#include "l2solver.h"

L2Solver::L2Solver(int nx_, double threshold_)
{
    nx = nx_;
    threshold = threshold_;

    B.set_size(nx, nx);
    p.set_size(nx,1);
    g.set_size(nx,1);

    for(int i = 0; i < nx; i ++){
        for(int j = 0;  j < nx; j ++){
            B(i,j) = 0.;
        }
    }
}

int L2Solver::solve(double *H_, double *g_, double *x_){
    for(int i = 0; i < nx; i++){
        B(i,i) = H_[i];
        g(i) = g_[i];
    }
    dlib::solve_trust_region_subproblem(B, g, threshold, p, 0.000001, 20);
    for(int i=0; i<nx; i++)
        x_[i] = p(i);
    return 0;
}
