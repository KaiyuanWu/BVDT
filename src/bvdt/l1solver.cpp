#include "l1solver.h"
#include "SimpleVector.h"
#include "cQpGenDense.h"
#include "QpGenDense.h"
#include "QpGenData.h"
L1Solver::L1Solver(int nx_, double threshold)
{
    nx =  2*nx_;
    newQpGenDense(&c, nx, &Q, &xlow, &ixlow, &xupp,  &ixupp, &A,  0,  &b, &C, 1,
                  &clow,   &iclow, &cupp, &icupp, &ierr );
    for(int i=0; i<nx; i++){
        xlow[i] = 0;
        ixlow[i] = 1;
        xupp[i] = threshold;
        ixupp[i] = 1;

        C[i] = 1;
    }
    cupp[0] = threshold;
    icupp[0] = 1;
    clow[0] = 0;
    iclow[0] = 1;

    memset(Q, 0, sizeof(double)*nx*nx);
    QpGenDenseGondzioSetup( c,  nx,  Q,    xlow,   ixlow,  xupp,  ixupp,
                            A,  0,  b,
                            C,  1,  clow,  iclow,  cupp,   icupp,
                            &ctx, &ierr );
    QpGenDense * factory  =(QpGenDense *) ctx.factory;
    Data * prob = (Data *) ctx.prob;
    vars  = (QpGenVars * )     factory->makeVariables(prob);
    resid = (QpGenResiduals *) factory->makeResiduals( prob);
}

L1Solver::~L1Solver(){
    freeQpGenDense(&c, &Q, &xlow, &ixlow, &xupp, &ixupp, &A, &b, &C, &clow,  &iclow, &cupp,  &icupp );
    QpGenCleanup(&ctx );
    delete vars;
    delete resid;
}

int L1Solver::solve(double *H, double *g,  double &objectiveValue, double* x){
    int status_code=0;
    QpGenData    * prob    = (QpGenData *)    ctx.prob;
    Solver       * solver  = (Solver *)       ctx.solver;
    for(int i = 0; i < nx/2; i++){
        Q[(2*i)*nx + (2*i)] =H[i];
        Q[(2*i)*nx + (2*i+1)] = -H[i];
        Q[(2*i+1)*nx + (2*i)] = -H[i];
        Q[(2*i+1)*nx + (2*i+1)] =H[i];
        c[2*i] = g[i];
        c[2*i+1] = -g[i];
    }

    try {
        solver->solve(prob,vars,resid);
        SimpleVector & varsX = dynamic_cast<SimpleVector &>(*(vars->x));
        for(int i=0; i<nx/2; i++){
            x[i] = varsX[2*i] - varsX[2*i+1];
        }
        objectiveValue = prob->objectiveValue( vars );
    }
    catch( ... ) {
        status_code = -1;
    }
    return status_code;
}


