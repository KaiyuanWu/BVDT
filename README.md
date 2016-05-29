# BVDT
Boosted Vector Decision Tree (BVDT) is a general boosted vector decision tree 

Contents
--------

* [Installation](#install)
* [Demonstration](#demo)

Installation
============
<a name="install"/>

Prequisites

     - Google Protocol Buffers

protobuf is used to easily configure the program and store the output model file. See https://developers.google.com/protocol-buffers/docs/tutorials for tutorials of protobuf. The parameters of this program are specified in [src/proto/algo.proto].

     - Dlib

Dlib is a modern C++ toolkit containing machine learning algorithms and tools for creating complex software in C++ to solve real world problems. See http://dlib.net for more informations. We use the ''solve_trust_region_subproblem'' to solve the trust region subproblem of the L2-BVDT algorithm.

     - Object-Oriented Software for Quadratic Programming (OOQP)

OOQP is an object-oriented C++ package, based on a primal-dual interior-point method, for solving convex quadratic programming problems (QPs). See http://pages.cs.wisc.edu/~swright/ooqp/ for more detations. We use the OOQP package to solve the quardratic optimization subproblem of the L1-BVDT algorithm. 
 

To compile the program, first edit the Makefile: modify the DLIB_DIR and OOQP_DIR to your dlib and OOQP package locations; make sure the EXTRA_LDFLAG correct. Then type "make" in the source root directory. The main excutable program is named by "bvdt".



Demonstration
============
<a name="demo"/>
An example is given in the [examples] directory.
To train a model:

> ../bvdt  train -model=handwritten_fold_0.proto

output:

> nleaves: 8

> nclass_label: 10

> maxz: 0.1

> shrinkage: 1

> ... ...

> 14:56:12 ==========================

> 14:56:12 Iteration: 19 nleaves = 8

> 14:56:12 Train Accuracy: 0.697509	 Train Loss: 1.96437

> 14:56:12 Test Accuracy: 0.684698	 Test Loss: 1.97507

> ... ...

> 15:13:28 Iteration: 4999 nleaves = 8

> 15:13:28 Train Accuracy: 1	 Train Loss: 8.69676e-10

> 15:13:28 Test Accuracy: 0.9879	 Test Loss: 0.0572106

To test a model:

> ../bvdt  test -model=handwritten_fold_0.proto data=handwritten_fold_0_test.txt

output:

> ... ...

> testing ....

> correctly classify 1387 out of 1405, accuracy = 98.7189%
