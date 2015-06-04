#include "data.h"
#include "math.h"

Data::Data():_train_data(NULL), _test_data(NULL), _train_labels(NULL), _test_labels(NULL),
    _train_f(NULL), _test_f(NULL), _train_p(NULL), _test_p(NULL),  _data_order_0(NULL),
    _data_reverse_order_0(NULL), _data_order(NULL), _data_reverse_order(NULL), _data_order_flag(NULL),
    _data_reverse_order_temp(NULL),
    _effective_data_number(0), _effective_feature_number(0)
{
}

Data::Data(int nfeatures, int ntrain_samples, int nclasses, int ntest_samples):_train_data(NULL), _test_data(NULL), _train_labels(NULL), _test_labels(NULL),
_train_f(NULL), _test_f(NULL), _train_p(NULL), _test_p(NULL),  _data_order_0(NULL),
_data_reverse_order_0(NULL), _data_order(NULL), _data_reverse_order(NULL), _data_order_flag(NULL),
  _data_reverse_order_temp(NULL),
  _effective_data_number(0), _effective_feature_number(0){
    _nfeatures = nfeatures;
    _ntrain_samples = ntrain_samples;
    _ntest_samples = ntest_samples;
    _nclasses = nclasses;
    if(_ntest_samples == 0)
        _has_test_data = false;
    else
        _has_test_data = true;
    allocate_data_space();
}

void Data::read_uci_data(string train_data_file){
    ifstream infile(train_data_file.c_str(), ios::in);
    if(!infile.good()){
        cout<<"Can not open "<<train_data_file<<endl;
        exit(-1);
    }
    int has_read = 0;
    for(has_read = 0; has_read<_ntrain_samples; has_read++){
        infile>>_train_labels[has_read];
        int index = has_read*_nfeatures;
        for(int i=0; i<_nfeatures; i++)
            infile>>_train_data[index+i];
        if(!infile.good())
            break;
    }
    if(has_read!=_ntrain_samples){
        cout<<"Data is not enough: ntrain_samples= "<<_ntrain_samples<<", has read "<<has_read<<endl;
        _ntrain_samples = has_read;
    }
    infile.close();
}

void Data::read_uci_data(string train_data_file, string test_data_file){
    if(!_has_test_data){
        cout<<"Not support test data! Please clearify the solver setting!"<<endl;
        exit(-1);
    }
    ifstream infile(train_data_file.c_str(), ios::in);
    if(!infile.good()){
        cout<<"Can not open "<<train_data_file<<endl;
        exit(-1);
    }
    int has_read = 0;
    for(has_read = 0; has_read<_ntrain_samples; has_read++){
        infile>>_train_labels[has_read];
        int index = has_read*_nfeatures;
        for(int i=0; i<_nfeatures; i++)
            infile>>_train_data[index+i];
        if(!infile.good())
            break;
    }
    if(has_read!=_ntrain_samples){
        cout<<"Data is not enough: ntrain_samples= "<<_ntrain_samples<<", has read "<<has_read<<endl;
        _ntrain_samples = has_read;
    }
    infile.close();

    ifstream infile2(test_data_file.c_str(), ios::in);
    if(!infile2.good()){
        cout<<"Can not open "<<test_data_file<<endl;
        exit(-1);
    }
    has_read = 0;
    for(has_read = 0; has_read<_ntest_samples; has_read++){
        infile2>>_test_labels[has_read];
        int index = has_read*_nfeatures;
        for(int i=0; i<_nfeatures; i++)
            infile2>>_test_data[index+i];
        if(!infile2.good())
            break;
    }
    if(has_read!=_ntest_samples){
        cout<<"Data is not enough: ntest_samples= "<<_ntest_samples<<", has read "<<has_read<<endl;
        _ntest_samples = has_read;
    }
    infile2.close();
}

void Data::init(int nfeatures, int ntrain_samples, int nclasses, int effective_data_number, int effective_feature_number, int ntest_samples){
    _nfeatures = nfeatures;
    _ntrain_samples = ntrain_samples;
    _ntest_samples = ntest_samples;
    _nclasses = nclasses;
    _effective_data_number = 0;
    _effective_feature_number = 0;
    if(_ntest_samples == 0)
        _has_test_data = false;
    else
        _has_test_data = true;
    _effective_data_number = effective_data_number;
    _effective_feature_number = effective_feature_number;
    reset_data();
    allocate_data_space();
}
void Data::allocate_data_space(){
    _train_data = new float[_nfeatures*_ntrain_samples];
    _train_labels = new int[_ntrain_samples];
    _train_f = new float[_nclasses*_ntrain_samples];
    _train_p = new float[_nclasses*_ntrain_samples];
    _train_p_d = new float[_nclasses*_ntrain_samples];
    if(_nclasses>2){
        memset(_train_f, 0, sizeof(float)*_nclasses*_ntrain_samples);
        for(int i=0; i<_nclasses*_ntrain_samples; i++){
            _train_p[i] = 1./_nclasses;
            _train_p_d[i] = 0.;
        }
    }
    else{
        for(int i=0; i<_ntrain_samples; i++){
            _train_f[2*i] = 1.;
            _train_f[2*i+1] = 0.;
            _train_p[2*i] = 7.31e-01;
            _train_p[2*i+1] = 2.69e-01;
        }
    }

    if(_has_test_data){
        _test_data = new float[_nfeatures*_ntest_samples];
        _test_labels = new int[_ntest_samples];
        _test_f = new float[_nclasses*_ntest_samples];
        _test_p = new float[_nclasses*_ntest_samples];
        _test_p_d = new float[_nclasses*_ntest_samples];
        if(_nclasses>2){
            memset(_test_f, 0, sizeof(float)*_nclasses*_ntest_samples);
            for(int i=0; i<_nclasses*_ntest_samples; i++){
                _test_p[i] = 1./_nclasses;
                _test_p_d[i] = 0.;
            }
        }
        else{
            for(int i=0; i<_ntest_samples; i++){
                _test_f[2*i] = 1.;
                _test_f[2*i+1] = 0.;
                _test_p[2*i] = 7.31e-01;
                _test_p[2*i+1] = 2.69e-01;
            }
        }
    }


    _data_order_0 = new int*[_nfeatures];
    for(int i=0; i<_nfeatures; i++)
        _data_order_0[i] = new int[_ntrain_samples];

    _data_reverse_order_0 = new int*[_nfeatures*_ntrain_samples];
    for(int i=0; i<_nfeatures; i++)
        _data_reverse_order_0[i] = new int[_ntrain_samples];

    _data_reverse_order_temp = new int[_ntrain_samples];
    _data_order_flag = new bitArray(_ntrain_samples);

    _data_order = new int*[_effective_feature_number];
    _data_reverse_order = new int*[_effective_feature_number];
    for(int i=0; i<_effective_feature_number; i++){
        _data_order[i] = new int[_ntrain_samples];
        _data_reverse_order[i] = new int[_effective_data_number];
    }
}
Data::~Data(){
    reset_data();
}

void Data::reset_data(){
    if(_train_data) delete[] _train_data;
    if(_test_data)  delete[] _test_data;
    if(_train_labels) delete[] _train_labels;
    if(_test_labels) delete[] _test_labels;
    if(_train_f) delete[] _train_f;
    if(_test_f) delete[] _test_f;
    if(_train_p) delete[] _train_p;
    if(_test_p) delete[] _test_p;
    if(_data_order_0) {
        for(int i=0; i<_nfeatures; i++)
            delete[] _data_order_0[i];
        delete[] _data_order_0;
    }
    if(_data_reverse_order_0) {
        for(int i=0; i<_effective_feature_number; i++)
            delete[] _data_reverse_order_0[i];
        delete[] _data_reverse_order_0;
    }
    if(_data_order) {
        for(int i=0; i<_nfeatures; i++)
            delete[] _data_order[i];
        delete[] _data_order;
    }
    if(_data_reverse_order) {
        for(int i=0; i<_effective_feature_number; i++)
            delete[] _data_reverse_order[i];
        delete[] _data_reverse_order;
    }
    if(_data_reverse_order_temp) delete[] _data_reverse_order_temp;
    if(_data_order_flag) delete _data_order_flag;

    if(_data_order) {
        for(int i=0; i<_effective_feature_number; i++)
            delete [] _data_order[i];
        delete[] _data_order;
    }
    if(_data_reverse_order) {
        for(int i=0; i<_effective_feature_number; i++)
            delete []  _data_reverse_order[i];
        delete[]  _data_reverse_order;
    }
}

void Data::preinit_data_order(){
    vector<pair<float,int> > temp;
    temp.resize(_ntrain_samples);
    for(int i=0; i<_nfeatures; i++){
        for(int j=0; j<_ntrain_samples; j++){
            temp[j].first = _train_data[j*_nfeatures + i];
            temp[j].second = j;
        }
        sort(temp.begin(), temp.end(), Data::compair);
        for(int j=0; j<_ntrain_samples; j++){
            _data_order_0[i][temp[j].second] = j;
            _data_reverse_order_0[i][j] = temp[j].second;
        }
    }

//    //print sorted data
//    cout<<"Before sort: "<<endl;
//    print_data();
//    cout<<"After sort: "<<endl;
//    print_data();
}
void Data::print_data(){
    for(int i=0; i<_ntrain_samples; i++){
        cout<<_train_labels[i]<<":";
        for(int j=0; j<_nfeatures; j++){
            cout<<_train_data[i*_nfeatures+j]<<"\t";
        }
        cout<<endl;
    }
}

void Data::print_classifier(int nbins){
    cout<<"Train:\t"<<endl;
    for(int i=0; i<_ntrain_samples; i+=nbins){
        cout<<i<<"("<<_train_labels[i]<<"):\t";
        for(int j=0; j<_nclasses; j++){
            cout<<_train_f[j]<<"\t";
        }
        cout<<endl;
    }
    if(_has_test_data){
        cout<<"Test:\t"<<endl;
        for(int i=0; i<_ntest_samples; i+=nbins){
            cout<<i<<"("<<_test_labels[i]<<"):\t";
            for(int j=0; j<_nclasses; j++){
                cout<<_test_f[j]<<"\t";
            }
            cout<<endl;
        }
    }
}

void Data::init_data_order(vector<int> &effective_data, vector<int> &effective_feature){
    _effective_feature_number = effective_feature.size();
    _effective_data_number = effective_data.size();
    _effective_data = &(effective_data);
    _effective_feature = &(effective_feature);
    for(int i=0; i<_effective_feature_number; i++){
        _data_order_flag->reset();
        for(int j=0; j<_effective_data_number; j++)
            _data_order_flag->set(_data_order_0[effective_feature[i]][effective_data[j]]);
        int order = 0;
        for(int j=0; j<_ntrain_samples; j++){
            if(_data_order_flag->test(j)){
                _data_reverse_order[i][order] = _data_reverse_order_0[effective_feature[i]][j];
                _data_order[i][_data_reverse_order[i][order]] = order;
                order++;
            }
        }
    }
}
void Data::rearrange(int left_index, int right_index, int cut, int feature_index){
//    cout<<"before rearrange ... "<<endl;
//    //before rearrange ...
//    for(int i=0; i<_effective_feature_number; i++){
//        cout<<"Feature "<<i<<": "<<endl;
//        for(int j=left_index; j<= right_index; j++){
//            int index = _data_reverse_order[i][j];
//            cout<<train_data(index)[(*_effective_feature)[i]]<<" ";
//        }
//        cout<<endl;
//    }


    int left, right;
    for(int i=0; i<_effective_feature_number; i++){
        if(i == feature_index)
            continue;
        _data_order_flag->reset(left_index, right_index);
        for(int j=left_index; j<= cut; j++){
            _data_order_flag->set(_data_order[i][_data_reverse_order[feature_index][j]]);
        }
        left = left_index;
        right = cut+1;
        for(int j=left_index; j<= right_index; j++){
            if(_data_order_flag->test(j)){
                _data_reverse_order_temp[left] = _data_reverse_order[i][j];
                left++;
            }
            else{
                _data_reverse_order_temp[right] = _data_reverse_order[i][j];
                right++;
            }
        }
        memcpy(_data_reverse_order[i]+left_index,_data_reverse_order_temp+left_index, (right_index-left_index+1)*sizeof(int) );
        for(int j=left_index; j<= right_index; j++)
            _data_order[i][_data_reverse_order_temp[j]] = j;
    }

//    //after rearrange ...
//    cout<<"after rearrange ..." <<endl;
//    for(int i=0; i<_effective_feature_number; i++){
//        cout<<"Feature "<<i<<": "<<endl;
//        for(int j=left_index; j<= right_index; j++){
//            int index = _data_reverse_order[i][j];
//            cout<<train_data(index)[(*_effective_feature)[i]]<<" ";
//            if(j==cut){
//                cout<<"||";
//            }
//        }
//        cout<<endl;
//    }
//    cout<<"======================================"<<endl;
}

const float* Data::train_data(int index){
    return _train_data+index*_nfeatures;
}

int Data::train_label(int index){
    return _train_labels[index];
}

const float* Data::test_data(int index){
    return _test_data+index*_nfeatures;
}

int Data::test_label(int index){
    return _test_labels[index];
}

const float* Data::train_f(int index){
    return _train_f+index*_nclasses;
}

const float* Data::test_f(int index){
    return _test_f+index*_nclasses;
}

const float* Data::train_p(int index){
    return _train_p+index*_nclasses;
}

const float* Data::test_p(int index){
    return _test_p+index*_nclasses;
}

const float* Data::train_p_d(int index){
    return _train_p_d+index*_nclasses;
}

const float* Data::test_p_d(int index){
    return _test_p_d+index*_nclasses;
}

float Data::train_loss(){
    return _train_loss;
}

float Data::test_loss(){
    return _test_loss;
}

float Data::train_accuracy(){
    return _train_accuracy;
}

float Data::test_accuracy(){
    return _test_accuracy;
}

float Data::train_loss0(){
    return _train_loss0;
}

float Data::test_loss0(){
    return _test_loss0;
}

float Data::train_accuracy0(){
    return _train_accuracy0;
}

float Data::test_accuracy0(){
    return _test_accuracy0;
}

int Data::data_order(int index, int ifeature){
    return _data_order[ifeature][index];
}

int Data::data_reverse_order(int index, int ifeature){
    return _data_reverse_order[ifeature][index];
}

void Data::update_train_f(const float* delta_f, float step, int index, bool dist){
    float maxf=0.;
    float sum_exp = 0.;
    index *= _nclasses;
    for(int i=0; i<_nclasses; i++){
        _train_f[index+i] += step*delta_f[i];
        if(maxf<_train_f[index+i]){
            maxf = _train_f[index+i];
        }
    }

    for(int i=0; i<_nclasses; i++){
        _train_f[index+i] -= maxf;
        _train_p[index+i] = exp(_train_f[index+i]);
        sum_exp += _train_p[index+i];
    }
    for(int i=0; i<_nclasses; i++)
        _train_p[index+i]/=sum_exp;

    if(dist){
        int label = _train_labels[index];
        for(int i=0; i<_nclasses; i++){
            float t = exp(_train_f[index+i]-_train_f[index+label]);
            if(t>1.) t=1.;
            _train_p_d[index+i] = SQ(i-_train_labels[index])*t;
        }
    }
}

void Data::update_test_f(const float* delta_f, float step, int index, bool dist){
    float maxf=0.;
    float sum_exp = 0.;
    index *= _nclasses;
    for(int i=0; i<_nclasses; i++){
        _test_f[index+i] += step*delta_f[i];
        if(maxf<_test_f[index+i]){
            maxf = _test_f[index+i];
        }
    }
    for(int i=0; i<_nclasses; i++){
        _test_f[index+i] -= maxf;
        _test_p[index+i] = exp(_test_f[index+i]);
        sum_exp += _test_p[index+i];
    }
    for(int i=0; i<_nclasses; i++)
         _test_p[index+i]/=sum_exp;
    if(dist){
        int label = _test_labels[index];
        for(int i=0; i<_nclasses; i++){
            float t = exp(_test_f[index+i]-_test_f[index+label]);
            if(t>1.) t=1.;
            _test_p_d[index+i] = SQ(i-_test_labels[index])*t;
        }
    }
}

void Data::update_performance(){
//    //DEBUG
//    cout<<"Data update_performance"<<endl;

    float maxp = 0.;
    int   maxi = 0, index;
    _train_accuracy0 = _train_accuracy;
    _train_loss0 = _train_loss;

    _train_accuracy = 0.;
    _train_loss = 0.;

    for(int i=0; i<_ntrain_samples; i++){
        maxp = 0.;
        maxi = 0;
        index = i*_nclasses;

        for(int j=0; j<_nclasses; j++){
            if(maxp < _train_p[index + j]){
                maxi = j;
                maxp = _train_p[index + j];
            }
        }
        if(maxi == _train_labels[i])
            _train_accuracy ++;
        if(_train_p[index+_train_labels[i]]>0)
            _train_loss += -log(_train_p[index+_train_labels[i]]);
        else
            _train_loss += 1.032789e+02;
    }

    _train_accuracy /= _ntrain_samples;
    _train_loss /= _ntrain_samples;
//    //DEBUG
//    cout<<"Data train update_performance OK!"<<endl;

    if(_has_test_data){
        _test_accuracy0 = _test_accuracy;
        _test_loss0 = _test_loss;

        _test_accuracy = 0.;
        _test_loss = 0.;


        for(int i=0; i<_ntest_samples; i++){
            maxp = 0.;
            maxi = 0;
            index = i*_nclasses;
            for(int j=0; j<_nclasses; j++){
                if(maxp < _test_p[index + j]){
                    maxi = j;
                    maxp = _test_p[index + j];
                }
            }
            if(maxi == _test_labels[i])
                _test_accuracy ++;

            if(_test_p[index+_test_labels[i]]>0)
                _test_loss += -log(_test_p[index+_test_labels[i]]);
            else
                _test_loss += 1.032789e+02;
        }
        _test_accuracy /= _ntest_samples;
        _test_loss /= _ntest_samples;
    }
//    //DEBUG
//    cout<<"Data test update_performance OK!"<<endl;
//    exit(0);
}
