#ifndef DATA_H
#define DATA_H
#include <algorithm>
#include <iostream>
#include <map>
#include <string>
#include <fstream>
#include <vector>
#include "bitArray.h"
#include "log.h"
#include "string.h"

#define SQ(x) x*x

using namespace std;
class BVDT_Data
{
public:
    BVDT_Data();
    BVDT_Data(int nfeatures, int ntrain_samples,  int nclasses,int ntest_samples=0);
    ~BVDT_Data();
    void init(int nfeatures, int ntrain_samples, int nclasses, int effective_data_number, int effective_feature_number, int ntest_samples = 0);


    int _nfeatures;
    int _ntrain_samples;
    int _ntest_samples;
    int _nclasses;
    bool _has_test_data;

    void read_uci_data(string train_data_file);
    void read_uci_data(string train_data_file, string test_data_file);

    const float* train_data(int index);
    int train_label(int index);
    const float* test_data(int index);
    int test_label(int index);
    const float* train_f(int index);
    const float* test_f(int index);
    const float* train_p(int index);
    const float* test_p(int index);
    const float* train_p_d(int index);
    const float* test_p_d(int index);


    int data_order(int index, int ifeature);
    int data_reverse_order(int index, int ifeature);

    float train_loss();
    float test_loss();
    float train_accuracy();
    float test_accuracy();

    float train_loss0();
    float test_loss0();
    float train_accuracy0();
    float test_accuracy0();

    void update_train_f(const float* delta_f, float step, int index, bool dist=false);
    void update_test_f(const float* delta_f, float step, int index, bool dist=false);
    void update_performance();
    void preinit_data_order();
    void init_data_order(vector<int>& effective_data, vector<int>& effective_feature);
    void rearrange(int left_index, int right_index, int cut, int feature_index);

    void print_data();
    void print_classifier(int nbins);
private:
    float* _train_data;
    float* _test_data;
    int* _train_labels;
    int* _test_labels;
    float* _train_f;
    float* _test_f;
    float* _train_p;
    float* _test_p;
    float* _train_p_d;
    float* _test_p_d;
    float _train_accuracy;
    float _test_accuracy;
    float _train_loss;
    float _test_loss;

    float _train_accuracy0;
    float _test_accuracy0;
    float _train_loss0;
    float _test_loss0;

    map<int, int> _label_map;

    int** _data_order_0;
    int** _data_reverse_order_0;

    int _effective_feature_number;
    int _effective_data_number;
    vector<int>* _effective_data;
    vector<int>* _effective_feature;
    int** _data_order;
    int** _data_reverse_order;
    int* _data_reverse_order_temp;
    bitArray*  _data_order_flag;
    void allocate_data_space();
    void reset_data();
public:
    static bool compair(const pair<float, int>& x, const pair<float, int>&y){
        return x.first<y.first;
    }
};

#endif // DATA_H
