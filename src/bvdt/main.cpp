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
    BVDT_Solver solver(proto);
    solver.init();
    solver.start();
}

void test(string proto, string data, string output){
    int nfeatures, nclasses;
    float* x;
    int label;
    float* result;
    int predict_label;
    int correct, wrong;

    Application app;
    if(!app.init(proto)){
        cout<<"Fail init!"<<endl;
        return;
    }

    nfeatures = app.nvariables();
    nclasses = app.nclasses();

    x = new float[nfeatures];
    result = new float[nclasses];

    ifstream infile(data.c_str(), ios::in);
    ofstream outfile(output.c_str(), ios::out);

    correct = 0;
    wrong = 0;
    cout<<"testing .... "<<endl;
    while(1){
        infile>>label;
        for(int i=0; i<nfeatures; i++){
            infile>>x[i];
        }
        if(!infile.good())
            break;
        predict_label = app.eval(x, result);
        if(predict_label == label)
            correct ++;
        else
            wrong ++;
        outfile<<predict_label<<"\t";
        for(int i=0; i<nclasses; i++){
            outfile<<result[i]<<"\t";
        }
        outfile<<endl;

    }

    cout<<"correctly classify "<<correct<<" out of "<<correct+wrong<<", accuracy = "<<100*float(correct)/(correct+wrong)<<"%"<<endl;


    infile.close();
    outfile.close();
    delete[] x;
    delete[] result;
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
    cout<<"        -out (output of prediction)"<<endl;
    exit(-1);
}

int main(int argc, char *argv[]){
    string commands;
    string model;
    string data;
    string output;
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
            if(argc == 5){
                output = argv[3];
                index = 0;
                while(output[index]!='='){
                    if(index<output.length()-1)
                        index ++;
                    else
                        break;
                }
                if(output[index]!='='){
                    print_help();
                }
                else
                    output = output.substr(index+1, output.length()-index-1);
            }
            else{
                output = "output.txt";
            }
            test(model, data, output);
        }
        else
            print_help();
    }

    return 0;
}
