#include "node.h"

Node::Node():_left_child(NULL), _right_child(NULL), _node_type(NodeParameter::UNDEF)
{
}

void Node::fill_node_parameter(NodeParameter* node_parameter, int tree_id){
    node_parameter->set_node_type(_node_type);
//    node_parameter->set_tree_id(tree_id);
//    if(_is_root_node){
//        node_parameter->set_tree_id(_tree_id);
//    }
    if(_node_type == NodeParameter::INTERNAL){
        node_parameter->set_cut(_cut);
        node_parameter->set_feature(_feature);
        _left_child->fill_node_parameter(node_parameter->mutable_left_child(), tree_id);
        _right_child->fill_node_parameter(node_parameter->mutable_right_child(), tree_id);
    }
    if(_node_type == NodeParameter::LEAF){
        node_parameter->set_value(_value);
        node_parameter->set_mode_id(_mode_id);
    }
}

float Node::eval(const float *x, int& mode_id){
    if(_node_type == NodeParameter::LEAF){
//        std::cout<<"("<<_left_data_index<<", "<<_right_data_index<<")"<<std::endl;
        mode_id = _mode_id;
//        std::cout<<"["<<_value<<", "<<_mode_id<<"] at node("<<_left_data_index<<", "<<_right_data_index<<")"<<std::endl;
        return _value;
    }
    else if(_node_type == NodeParameter::INTERNAL){
        if(x[_feature] <= _cut)
            return _left_child->eval(x, mode_id);
        else
            return _right_child->eval(x, mode_id);
    }
    else{
        mode_id = 0;
        return 0.;
    }
}

void Node::best_gain_node(Node*& node, float& best_gain){
    if(_node_type == NodeParameter::LEAF){
        if(_node_split_gain>best_gain){
            node = this;
            best_gain = _node_split_gain;
        }
    }
    else{
        if(_left_child)
            _left_child->best_gain_node(node, best_gain);
        if(_right_child)
            _right_child->best_gain_node(node, best_gain);
    }
}

void  Node::reset(){
    if(_left_child){
        _left_child->reset();
        _left_child = NULL;
    }
    if(_right_child){
        _right_child->reset();
        _right_child = NULL;
    }
    _node_type = NodeParameter::UNDEF;
    _feature = 0;
    _value = 0;
    _cut = 0.;
    _node_gain = 0.;
    _node_split_gain = 0.;
    _left_data_index = -1;
    _right_data_index = -1;
}
void Node::reset_nodemode(){
    if(_node_type == NodeParameter::LEAF)
        _nodemode->reset(_mode_id);
    else if(_node_type == NodeParameter::INTERNAL){
        if(_left_child)
            _left_child->reset_nodemode();
        if(_right_child)
            _right_child->reset_nodemode();
    }
}

void Node::printInfo(const char* indent, bool last){
    char temp[1024];
    if(last){
        printf("%s|-(%d, %d) gain: %f split gain: %f value: %f cut: %f feature: %d mode_id: %d\n",indent, _left_data_index, _right_data_index,
               _node_gain, _node_split_gain,  _value, _cut, _feature, _mode_id);
        sprintf(temp,"%s|  ",indent);
        if(_left_child){
            if(_left_child->_node_type!=NodeParameter::UNDEF)
                _left_child->printInfo(temp, true);
        }
        if(_right_child){
            if(_right_child->_node_type!=NodeParameter::UNDEF)
                _right_child->printInfo(temp, false);
        }
    }
    else{
        printf("%s+ (%d, %d) gain: %f split gain: %f value: %f cut: %f feature: %d mode_id: %d\n",indent, _left_data_index, _right_data_index,
               _node_gain, _node_split_gain, _value, _cut, _feature, _mode_id);
        sprintf(temp,"%s  ",indent);
        if(_left_child){
            if(_left_child->_node_type!=NodeParameter::UNDEF)
                _left_child->printInfo(temp, true);
        }
        if(_right_child){
            if(_right_child->_node_type!=NodeParameter::UNDEF)
                _right_child->printInfo(temp, false);
        }
    }
}

void Node::add_sample(const float* x,const float* p, int label){
    if(_node_type == NodeParameter::LEAF){
        _nodemode->increment_node_mode(p, label, _mode_id);
    }
    else if(_node_type == NodeParameter::INTERNAL){
        if(x[_feature] <= _cut)
            _left_child->add_sample(x, p, label);
        else
            _right_child->add_sample(x, p, label);
    }
}
void Node::calculate_leaf_value(NodeGain* nodegain){
    if(_node_type == NodeParameter::LEAF){
        float gradient, hessian;
        int  nsamples;
        nsamples = _nodemode->node_mode(gradient, hessian, _mode_id);
        nodegain->node_gain(gradient, hessian, nsamples);
//        std::cout<<"gradient = "<<gradient<<",  hessian = "<<hessian<<", nsamples = "<<nsamples<<std::endl;
        _value = nodegain->_node_value;
    }
    else if(_node_type == NodeParameter::INTERNAL){
        if(_left_child)
            _left_child->calculate_leaf_value(nodegain);
        if(_right_child)
            _right_child->calculate_leaf_value(nodegain);
    }
}
