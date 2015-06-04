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

To compile the program, type "make" in the source root directory. The main excutable program is named by "bvdt".

Demonstration
============
<a name="demo"/>
An example is given in the [examples] directory.
To train a model:

> ../bvdt  train -model=handwritten_fold_0.proto

output:

nleaves: 8
nclass_label: 10
event_sample_rate: 1
node_mode_sample_rate: 1
... ...
17:05:39 ==========================
17:05:39 Iteration: 4999 nleaves = 8
17:05:39 Train Accuracy: 1	 Train Loss: 2.33328e-10
17:05:39 Test Accuracy: 0.9879	 Test Loss: 0.054503


To test a model:

../bvdt  test -model=handwritten_fold_0.proto data=handwritten_fold_0_val.txt

output:

testing ....
correctly classify 1387 out of 1405, accuracy = 98.7189%
