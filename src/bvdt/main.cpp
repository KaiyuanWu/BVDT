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
//#include "runtest.h"

using namespace std;
void train(string proto){
//    string proto = "/Users/kaiwu/BVDT_BoostedVectorDecisionTree/check_speration/circle_data.proto";
    Solver solver(proto);
    solver.init();
    solver.start();
}

void test(string proto, string data){

}

void print_help(){
    cout<<"usage: bvdt <command> <args>"<<endl;
    cout<<endl;
    cout<<"commands:"<<endl;
    cout<<"    train                    train a model"<<endl;
    cout<<"    test                     predict data with a model"<<endl;

    cout<<endl;
    cout<<"    Flags:\t"<<endl;
    cout<<"        -model (The model definition protocol buffer text file.)"<<endl;
    cout<<"        -data (input test dataset file name)"<<endl;
    exit(-1);
}

int main(int argc, char *argv[]){
    string commands;
    string model;
    string data;
    if(argc<3){
        print_help();
    }
    else{
        commands = argv[1];
        if(commands.compare("train") == 0){
            model = argv[2];
            int index = 0;
            while(model[index]!='='){
                if(index<model.length()-1)
                    index ++;
                else
                    break;
            }
            if(model[index]!='='){
                print_help();
            }
            else
                model = model.substr(index+1, model.length()-index-1);
            train(model);
        }
        else if(commands.compare("test") == 0){
            if(argc<4){
                print_help();
            }
            model = argv[2];
            int index = 0;
            while(model[index]!='='){
                if(index<model.length()-1)
                    index ++;
                else
                    break;
            }
            if(model[index]!='='){
                print_help();
            }
            else
                model = model.substr(index+1, model.length()-index-1);

            data = argv[3];
            index = 0;
            while(data[index]!='='){
                if(index<data.length()-1)
                    index ++;
                else
                    break;
            }
            if(data[index]!='='){
                print_help();
            }
            else
                data = data.substr(index+1, data.length()-index-1);
            test(model, data);
        }
        else
            print_help();
    }

    return 0;
}
