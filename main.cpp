#include "algo.pb.h"
#include "tree.h"
#include "solver.h"
#include "google/protobuf/message.h"
#include "google/protobuf/text_format.h"
#include <google/protobuf/io/coded_stream.h>
#include <google/protobuf/io/zero_copy_stream_impl.h>
#include <iostream>
#include <fstream>
#include <algorithm>
#include <string>
#include <map>
#include <fcntl.h>
#include "log.h"
#include "application.h"
#include "runtest.h"

using namespace std;
void train(){
    string proto = "/Users/kaiwu/BVDT_BoostedVectorDecisionTree/check_speration/circle_data.proto";
    Solver solver(proto);
    solver.init();
    solver.start();
}

void test(){
    const char* proto_format = "/Users/kaiwu/BVDT_BoostedVectorDecisionTree/experiments/abalone_distance2/protos_ana/proto_%d";
    const char*  validate_data_format = "/Users/kaiwu/BVDT_BoostedVectorDecisionTree/experiments/abalone_distance2/fold_%d/abalone_fold_%d_val.txt";
    const char*  test_data_format = "/Users/kaiwu/BVDT_BoostedVectorDecisionTree/experiments/abalone_distance2/fold_%d/abalone_fold_%d_test.txt";
    const char*  train_data_format = "/Users/kaiwu/BVDT_BoostedVectorDecisionTree/experiments/abalone_distance2/fold_%d/abalone_fold_%d_train.txt";
    const char*  validate_output_format = "/Users/kaiwu/BVDT_BoostedVectorDecisionTree/experiments/abalone_distance2/result/valid_%d";
    const char*  train_output_format = "/Users/kaiwu/BVDT_BoostedVectorDecisionTree/experiments/abalone_distance2/result/train_%d";
    const char*  test_output_format = "/Users/kaiwu/BVDT_BoostedVectorDecisionTree/experiments/abalone_distance2/result/test_%d";

    char validate_data[256];
    char test_data[256];
    char train_data[256];
    char proto[256];
    char test_output[256];
    char train_output[256];
    char validate_output[256];

    for(int i=0; i<36; i++){
        sprintf(proto, proto_format, i);
        sprintf(test_data, test_data_format, i%4, i%4);
        sprintf(train_data, train_data_format, i%4, i%4);
        sprintf(validate_data, validate_data_format, i%4, i%4);
        sprintf(train_output, train_output_format, i);
        sprintf(test_output, test_output_format, i);
        sprintf(validate_output, validate_output_format, i);

        RunTest run_test(proto);
        run_test.classify_test_data(train_output, train_data, 970);
        run_test.classify_test_data(test_output, test_data, 970);
        run_test.classify_test_data(validate_output, validate_data, 970);
    }
}

int main(int argc, char *argv[]){
    srand(time(0));
    test();
    return 0;
//    Application app;
//    app.init(argv[1]);
//    return 0;

//    TreeParameter tree;
//    ifstream infile("/Users/kaiwu/BVDT_BoostedVectorDecisionTree/build-bvdt-Debug/snapshot_20.model");
//    tree.ParseFromIstream(&infile);
//    tree.PrintDebugString();
//    infile.close();
//    return 0;

    string protos[] ={"/Users/kaiwu/BVDT_BoostedVectorDecisionTree/experiments/abalone_distance/protos_ana/proto_12",
                      "/Users/kaiwu/BVDT_BoostedVectorDecisionTree/experiments/abalone_distance/protos_ana/proto_13",
                      "/Users/kaiwu/BVDT_BoostedVectorDecisionTree/experiments/abalone_distance/protos_ana/proto_14",
                      "/Users/kaiwu/BVDT_BoostedVectorDecisionTree/experiments/abalone_distance/protos_ana/proto_15"};
    const char* output_file_name[] = {"abalone0",   "abalone1", "abalone2", "abalone3"};
    const char* test_data[] = {"/Users/kaiwu/BVDT_BoostedVectorDecisionTree/experiments/abalone_distance/fold_0/abalone_fold_0_val.txt",
                               "/Users/kaiwu/BVDT_BoostedVectorDecisionTree/experiments/abalone_distance/fold_1/abalone_fold_1_val.txt",
                               "/Users/kaiwu/BVDT_BoostedVectorDecisionTree/experiments/abalone_distance/fold_2/abalone_fold_2_val.txt",
                               "/Users/kaiwu/BVDT_BoostedVectorDecisionTree/experiments/abalone_distance/fold_3/abalone_fold_3_val.txt"};
    int niterations = 8077;
    int ifold = atoi(argv[1]);
    RunTest run_test(protos[ifold]);
    run_test.classify_test_data(output_file_name[ifold], test_data[ifold], niterations);

//    string proto = argv[1];
//    Solver solver(proto);
//    solver.init();
//    solver.start();
    return 0;
}
