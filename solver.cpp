#include "solver.h"
using namespace google::protobuf::io;
Solver::Solver(string solver_proto)
{
    _solver_proto = solver_proto;
    int input_fd = open(_solver_proto.c_str(), O_RDONLY);
    if(input_fd == -1){
        cout << "File not found: " << solver_proto;
        exit(-1);
    }
    FileInputStream* input = new FileInputStream(input_fd);
    google::protobuf::TextFormat::Parse(input, &_solver_parameter);
//    if(_solver_parameter.snapshot().find("abalone")!=_solver_parameter.snapshot().npos)
//        _solver_parameter.set_nfeatures(3);
    _solver_parameter.PrintDebugString();
    delete input;
    close(input_fd);
}

void Solver::init(){
    //init data
    _data = new Data();
    _data->init(_solver_parameter.nfeatures(),_solver_parameter.ntrain_samples(),_solver_parameter.nclass_label(),_solver_parameter.ntrain_samples()*_solver_parameter.event_sample_rate()<1?1:_solver_parameter.ntrain_samples()*_solver_parameter.event_sample_rate(),
                _solver_parameter.nfeatures()*_solver_parameter.feature_sample_rate()<1?1:_solver_parameter.nfeatures()*_solver_parameter.feature_sample_rate(), _solver_parameter.ntest_samples());
    if(_solver_parameter.has_test_data())
        _data->read_uci_data(_solver_parameter.train_data(), _solver_parameter.test_data());
    else
        _data->read_uci_data(_solver_parameter.train_data());
    _data->preinit_data_order();

    //split
    if(_solver_parameter.node_split_type() == SolverParameter::NEWTON){
        switch (_solver_parameter.regularization_type_split()) {
        case SolverParameter::L1_REGULARIZATION:
            _node_gain_split = new NodeGain_L1(SolverParameter::L1_REGULARIZATION,
                                               _solver_parameter.regularization_split());
            break;
        case SolverParameter::L2_REGULARIZATION:
            _node_gain_split = new NodeGain_L2(SolverParameter::L2_REGULARIZATION,
                                               _solver_parameter.regularization_split());
            break;
        case SolverParameter::L1L2_REGULARIZATION:
            _node_gain_split = new NodeGain_L1L2(SolverParameter::L1L2_REGULARIZATION,
                                               _solver_parameter.regularization_split(),
                                               _solver_parameter.regularization_ratio_split ());
            break;
        case SolverParameter::NO_REGULARIZATION:
            _node_gain_split = new NodeGain_No_Regularization(SolverParameter::NO_REGULARIZATION,
                                                              _solver_parameter.maxz());
            break;
        default:
            cout<<"This node split type has not been implemented yet!"<<endl;
            exit(0);
        }
    }
    else{
        _node_gain_split = new NodeGain_Gradient(SolverParameter::NO_REGULARIZATION);
    }

    //leaf value
    if(_solver_parameter.leaf_value_type() == SolverParameter::NEWTON){
        switch (_solver_parameter.regularization_type_leaf()) {
        case SolverParameter::L1_REGULARIZATION:
            _node_gain_leaf = new NodeGain_L1(SolverParameter::L1_REGULARIZATION,
                                               _solver_parameter.regularization_leaf());
            break;
        case SolverParameter::L2_REGULARIZATION:
            _node_gain_leaf = new NodeGain_L2(SolverParameter::L2_REGULARIZATION,
                                               _solver_parameter.regularization_leaf());
            break;
        case SolverParameter::L1L2_REGULARIZATION:
            _node_gain_leaf = new NodeGain_L1L2(SolverParameter::L1L2_REGULARIZATION,
                                               _solver_parameter.regularization_leaf(),
                                               _solver_parameter.regularization_ratio_leaf ());
            break;
        case SolverParameter::NO_REGULARIZATION:
            _node_gain_leaf = new NodeGain_No_Regularization(SolverParameter::NO_REGULARIZATION,
                                                              _solver_parameter.maxz());
            break;
        default:
            cout<<"This node split type has not been implemented yet!"<<endl;
            exit(0);
        }
    }
    else{
        _node_gain_leaf = new NodeGain_Gradient(SolverParameter::NO_REGULARIZATION);
    }

    if(_solver_parameter.has_convergence_rate()){
        _has_convergence_rate = true;
        _convergence_rate = _solver_parameter.convergence_rate();
    }
    else{
        _has_convergence_rate = false;
    }

    _tree = new Tree();
    _tree->init_tree(_data, _node_gain_split, _node_gain_leaf, &_solver_parameter);

    //mkdir
    char cmd[1024];
    sprintf (cmd, "mkdir -p %s", _solver_parameter.snapshot ().c_str ());
    system(cmd);
//    vector<int> effective_data_number ;
//    vector<int> effective_feature_number;
//    for(int i=0; i<_data->_ntrain_samples; i++)
//        effective_data_number.push_back(i);
//    for(int i=0; i<_data->_nfeatures; i++)
//        effective_feature_number.push_back(i);

//    _data->init_data_order(effective_data_number, effective_feature_number);
//    //test rearrange
//    _data->rearrange(0, _data->_ntrain_samples-1, 5, 0);
//    cout<<"After Rearrange: "<<endl;
//    for(int i=0; i<effective_feature_number.size(); i++){
//        for(int j=0; j<effective_data_number.size(); j++){
//            cout<<_data->train_data(_data->data_reverse_order(j,i))[i]<<"\t";
//        }
//        cout<<endl;
//    }
}

void Solver::start(){
    int i=0;
//    _data->print_data();
//    return;
    for(i=0; i<_solver_parameter.nmaximum_trees(); i++){
//        //DEBUG
//        cout<<"Iteration: "<<i<<endl;
//        _data->print_classifier(50);
        _tree->build_tree();
        _tree->update_data();
        _data->update_performance();
        if(i%_solver_parameter.display() == _solver_parameter.display() - 1){
            LOG<<"=========================="<<endl;
            LOG<<"Iteration: "<<i<<" nleaves = "<<_tree->nleaves_1()<<endl;
            LOG<<"Train Accuracy: "<<_data->train_accuracy()<<"\t Train Loss: "<<_data->train_loss()<<endl;
            if(_data->_has_test_data){
                LOG<<"Test Accuracy: "<<_data->test_accuracy()<<"\t Test Loss: "<<_data->test_loss()<<endl;
            }
        }

        if(_solver_parameter.reshuffle_interval ()!=0){
            if(i%_solver_parameter.reshuffle_interval () == _solver_parameter.reshuffle_interval ()-1)
                _tree->reshuffle ();
        }

        if(i%_solver_parameter.snapshot_interval() == 0 && i!=0){
            char count[64];
            sprintf(count, "/iter_%d.model", i);
            string output_name = _solver_parameter.snapshot() + count;
            _tree->write_tree_parameter(output_name);
        }
        if(i>1&&_has_convergence_rate){
            float convergence_rate = _data->train_loss()/_data->train_loss0();
            if(convergence_rate>_convergence_rate)
                _tree->adjust_nleaves(1);
            else
                _tree->adjust_nleaves(-1);
            cout<<convergence_rate<<", "<<_convergence_rate<<", nleaves = "<<_tree->nleaves()<<endl;
        }
//        //DEBUG
//        cout<<"Solver Iteration: "<<i<<"Ok"<<endl;
    }
    if((i-1)%_solver_parameter.snapshot_interval() != 0){
        char count[64];
        sprintf(count, "/iter_%d.model", i);
        string output_name = _solver_parameter.snapshot() + count;
        _tree->write_tree_parameter(output_name);
    }
}
