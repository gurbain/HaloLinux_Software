//modified from Node.h - tweddle

#pragma once

#include <list>
#include <Eigen/Dense>

#include <isam/Element.h>
#include <isam/Noise.h>
#include <isam/Node.h>
//#include "slam_dynamic3d_NL.h"

namespace isam {

//class dynamicPose3d_dynamicPose3d_Factor;

// Generic template for easy instantiation of the multiple node types.
template <class T>
class NodeExmapT : public Node {

 protected:
  T* _value;  // current estimate
  T* _value0; // linearization point

public:

  NodeExmapT() : Node(T::name(), T::dim) {
    _value = NULL;
    _value0 = NULL;
  }

  NodeExmapT(const char* name) : Node(name, T::dim) {
    _value = NULL;
    _value0 = NULL;
  }

  virtual ~NodeExmapT() {
    delete _value;
    delete _value0;
  }

  void init(const T& t) {
    delete _value; delete _value0;
    _value = new T(t); _value0 = new T(t);
  }

  bool initialized() const {return _value != NULL;}

  T value(Selector s = ESTIMATE) const {return (s==ESTIMATE)?*_value:*_value0;}
  T value0() const {return *_value0;}

  Eigen::VectorXd vector(Selector s = ESTIMATE) const {return (s==ESTIMATE)?_value->vector():_value0->vector();}
  Eigen::VectorXd vector0() const {return _value0->vector();}

  void update(const Eigen::VectorXd& v) {_value->set(v);}
  void update0(const Eigen::VectorXd& v) {_value0->set(v);}

  void linpoint_to_estimate() {*_value = *_value0;}
  void estimate_to_linpoint() {*_value0 = *_value;}
  void swap_estimates() {T tmp = *_value; *_value = *_value0; *_value0 = tmp;}

//  void apply_exmap(const Eigen::VectorXd& v) {*_value = _value0->exmap(v);}
// void self_exmap(const Eigen::VectorXd& v) {*_value0 = _value0->exmap(v);}

  void apply_exmap(const Eigen::VectorXd& v);
  void self_exmap(const Eigen::VectorXd& v) {*_value0 = _value0->exmap(v);}

  void rezero() {
	  _value->rezero();
	  _value0->rezero();
  }

  void write(std::ostream &out) const {
    out << name() << "_Node " << _id;
    if (_value != NULL) {
      out << " " << value();
    }
  }
};

}
