
#ifndef SLAMDYNAMICS_H_
#define SLAMDYNAMICS_H_


#include <Eigen/Dense>
#include "dynamicPose3d_NL.h"
#include "camera3d.h"
#include "FactorVariableNoise.h"
#include <isam/Node.h>
#include <isam/Factor.h>
#include <isam/Pose3d.h>
#include <isam/Point3d.h>
#include <isam/slam_stereo.h>
#include "NodeExmap.h"
#include "inertiaRatios.h"
#include "kinematicPose3d.h"



//#define DEFAULT_TIMESTEP	1.0

namespace isam{

typedef NodeExmapT<dynamicPose3d_NL> dynamicPose3d_NL_Node;
typedef NodeT<Point3d> Point3d_Node;

/**
 * Prior on dynamicPose3d.
 */
class dynamicPose3d_NL_Factor : public FactorT<dynamicPose3d_NL> {
public:
  dynamicPose3d_NL_Node* _pose;

  dynamicPose3d_NL_Factor(dynamicPose3d_NL_Node* pose, const dynamicPose3d_NL& prior, const Noise& noise)
    : FactorT<dynamicPose3d_NL>("dynamicPose3d_NL_Factor", 12, noise, prior), _pose(pose) {
    _nodes.resize(1);
    _nodes[0] = pose;
  }

  void initialize() {
    if (!_pose->initialized()) {
      dynamicPose3d_NL predict = _measure;
      _pose->init(predict);
    }
  }

  Eigen::VectorXd basic_error(Selector s = ESTIMATE) const {

	  dynamicPose3d_NL p1 = _pose->value(s);
	  Eigen::VectorXd err = p1.vectorFull() - _measure.vector();

	  Eigen::Vector4d p1qTot = p1.qTotal();
	  Eigen::Vector4d mqTot = _measure.qTotal();
	  Vector3d da = p1.getMRPdifference(p1qTot, mqTot);
	  err.segment<3>(6) = da;
/*
	  dynamicPose3d_NL p1 = _pose->value(s);
	  Eigen::VectorXd err = p1.vectorFull() - _measure.vectorFull();
*/
/*
    std::cout << "p1: " << p1.vectorFull().transpose() << "\n";
    std::cout << "measure: " << _measure.vectorFull().transpose() << "\n";
    std::cout << "Error: " << err.transpose() << "\n";
*/
    return err;
  }
};

/*
Eigen::VectorXd basic_error(Selector s = ESTIMATE) const {

	  kinematicPose3d p1 = _pose->value(s);
	  Eigen::VectorXd err = p1.vector() - _measure.vector();
	  Vector4d q1_tot = p1.qTotal();
	  Vector4d qm_tot = _measure.qTotal();
	  Vector4d dq = p1.quaternionDivision(q1_tot,qm_tot);
	  Vector3d da = p1.quaternion2mrp(dq);

	  err.segment<3>(3) = da;

	return err;
	*/


class dynamicPose3d_NL_odometry_Factor : public FactorT<dynamicPose3d_NL> {
  dynamicPose3d_NL_Node* _pose1;
  dynamicPose3d_NL_Node* _pose2;
//  double dt;

public:

  /**
   * Constructor.
   * @param pose1 The pose from which the measurement starts.
   * @param pose2 The pose to which the measurement extends.
   * @param measure DOES NOTHING - DON'T USE IT!!!! (could be extended in future release to add forces/torques
   * @param noise The 12x12 square root information matrix (upper triangular).
   */
  dynamicPose3d_NL_odometry_Factor(dynamicPose3d_NL_Node* pose1, dynamicPose3d_NL_Node* pose2,
	const dynamicPose3d_NL& measure, const Noise& noise)
    : FactorT<dynamicPose3d_NL>("dynamicPose3d_NL_odometry_Factor", 6, noise, measure),
    _pose1(pose1), _pose2(pose2) {
	_nodes.resize(2);
    _nodes[0] = pose1;
    _nodes[1] = pose2;
/*
    std::cout << "pose1 " << ": "<< _pose1->value().x().transpose() << " -- " << pose1->value().q().transpose() << std::endl;
    std::cout << _nodes[0] << std::endl;
    std::cout << "pose2 "  << ": "<< _pose2->value().x().transpose() << " -- " << pose2->value().q().transpose() << std::endl;
    std::cout << _nodes[1] << std::endl;
*/
//    dt = DEFAULT_TIMESTEP;
  }

  void initialize() {
    dynamicPose3d_NL_Node* pose1 = _pose1;
    dynamicPose3d_NL_Node* pose2 = _pose2;
    require(pose1->initialized() && pose2->initialized(),
        "Dynamic SLAM ODOM both poses must be initialized");

    if (!pose1->initialized() && pose2->initialized()) {
      // reverse constraint
    	std::cout << "No BACKWARDS PROPAGATE" << std::endl;
//      dynamicPose3d_NL a = pose2->value();
//    	dynamicPose3d_NL predict = a.backwardsPropagate();
//      pose1->init(predict);
    } else if (pose1->initialized() && !pose2->initialized()) {
    }
	//std::cout << "ODOM PROPAGATE" << std::endl;
	dynamicPose3d_NL prev = pose1->value();
	dynamicPose3d_NL measured_pose = _measure;

	pose2->init(measured_pose.applyOdometry(prev));

  }

  Eigen::VectorXd basic_error(Selector s = ESTIMATE) const {
//    dynamicPose3d_NL p1(_nodes[0]->vector(s));
//    dynamicPose3d_NL p2(_nodes[1]->vector(s));

    dynamicPose3d_NL p1 = _pose1->value(s);
    dynamicPose3d_NL p2 = _pose2->value(s);
/*
    std::cout << "p1 " << ": "<< p1.x().transpose() << " -- " << p1.q().transpose() << std::endl;
    std::cout << _nodes[0] << std::endl;
    std::cout << "p2 "  << ": "<< p2.x().transpose() << " -- " << p2.q().transpose() << std::endl;
    std::cout << _nodes[1] << std::endl;
*/

    //MEASUREMENT METHOD
    //********************************************************
    Eigen::VectorXd est_odometry = p2.getOdometry(p1);
    dynamicPose3d_NL measured_pose = _measure;
    Eigen::VectorXd measured_odometry = measured_pose.getOdometry();
    Eigen::VectorXd err = est_odometry - measured_odometry;
/*
	std::cout << "Measured Odometry: " <<  measured_odometry.transpose() << std::endl;
	std::cout << "Estimated Odometry: " << est_odometry.transpose() << std::endl;
	std::cout << "Error: " << err.transpose() << std::endl;
*/
    return err;
  }

  void write(std::ostream &out) const {
	  FactorT<dynamicPose3d_NL>::write(out);
  }
};






class dynamicPose3d_NL_dynamicPose3d_NL_Factor : public FactorVarNoiseT<dynamicPose3d_NL > {
  dynamicPose3d_NL_Node* _pose1;
  dynamicPose3d_NL_Node* _pose2;
  inertiaRatios_Node* _ir_node;
  double dt;

public:

  /**
   * Constructor.
   * @param pose1 The pose from which the measurement starts.
   * @param pose2 The pose to which the measurement extends.
   * @param measure DOES NOTHING - DON'T USE IT!!!! (could be extended in future release to add forces/torques
   * @param noise The 12x12 square root information matrix (upper triangular).
   */
  dynamicPose3d_NL_dynamicPose3d_NL_Factor(dynamicPose3d_NL_Node* pose1, dynamicPose3d_NL_Node* pose2, inertiaRatios_Node* ir_node,
	const dynamicPose3d_NL& measure, const Noise& noise, double timestep)
    : FactorVarNoiseT<dynamicPose3d_NL>("dp3dNL_dp3dNL_IR_Factor", 12, noise, measure),
    _pose1(pose1), _pose2(pose2), _ir_node(ir_node), dt(timestep) {
	_nodes.resize(3);
    _nodes[0] = pose1;
    _nodes[1] = pose2;
    _nodes[2] = ir_node;
//    dt = DEFAULT_TIMESTEP;
  }

  void initialize() {
    dynamicPose3d_NL_Node* pose1 = _pose1;
    dynamicPose3d_NL_Node* pose2 = _pose2;
    inertiaRatios_Node* ir_node = _ir_node;
    require(pose1->initialized() || pose2->initialized(),
        "dynamicSLAM: dynamicPose3d_NL_dynamicPose3d_NL_Factor requires pose1 or pose2 to be initialized");

//    require(ir_node->initialized(),
//        "dynamicSLAM: dynamicPose3d_NL_dynamicPose3d_NL_Factor requires inertias to be initialized");
    if(!_ir_node->initialized()) {
    	inertiaRatios init_ir;
    	_ir_node->init(init_ir);
    }

    if (!pose1->initialized() && pose2->initialized()) {
    	std::cout << "No BACKWARDS PROPAGATE" << std::endl;
    } else if (pose1->initialized() && !pose2->initialized()) {
    	inertiaRatios ir = _ir_node->value();
      dynamicPose3d_NL a = pose1->value();
      dynamicPose3d_NL predict = a.propagate(dt, ir);
      pose2->init(predict);
    }
  }

  Eigen::VectorXd basic_error(Selector s = ESTIMATE) const {

    dynamicPose3d_NL p1 = _pose1->value(s);
    dynamicPose3d_NL p2 = _pose2->value(s);
    inertiaRatios ir = _ir_node->value(s);

    //std::cout << ir << std::endl;

    Eigen::VectorXd err = p2.computeStateChange(p1, dt, ir);

    return err;
  }

  Eigen::MatrixXd get_sqrtinf() const {
	  inertiaRatios ir = _ir_node->value();
	  Eigen::MatrixXd new_sqrtinf = _pose1->value().getProcessNoise(dt,ir)._sqrtinf;
	  return new_sqrtinf;
  }

  bool checkPose1(dynamicPose3d_NL_Node* poseRef) {
	  if(_pose1 == poseRef) {
		  return true;
	  } else {
		  return false;
	  }
  }

  bool checkPose1(inertiaRatios_Node* ir_node) {
	  if(_ir_node == ir_node) {
		  return true;
	  } else {
		  return false;
	  }
  }

  bool checkPose1(kinematicPose3d_Node* poseRef) {
	  return false;
  }

  double get_dt() { return dt;}

  void write(std::ostream &out) const {
	  FactorVarNoiseT<dynamicPose3d_NL >::write(out);
  }
};





typedef NodeT<Point3dh> Point3dh_Node;


class StereoCameraDebug { // for now, camera and robot are identical
  double _f;
  Eigen::Vector2d _pp;
  double _b;

public:
  EIGEN_MAKE_ALIGNED_OPERATOR_NEW

  StereoCameraDebug() : _f(1), _pp(Eigen::Vector2d(0.5,0.5)), _b(0.1) {}
  StereoCameraDebug(double f, const Eigen::Vector2d& pp, double b) : _f(f), _pp(pp), _b(b) {}

  inline double focalLength() const {return _f;}

  inline Eigen::Vector2d principalPoint() const {return _pp;}

  inline double baseline() const {return _b;}

  StereoMeasurement project(const Pose3d& pose, const Point3dh& Xw) const {
    Point3dh X = pose.transform_to(Xw);
    // camera system has z pointing forward, instead of x
/*    double x = X.y();
    double y = X.z();
    double z = X.x();
*/
    double x = -X.y();
    double y = -X.z();
    double z = X.x();

//    double x = X.x();
//    double y = X.y();
//	double z = X.z();

//    double w = X.w();
    // left camera
    double fz = _f / z;
    double u = x * fz + _pp(0);
    double v = y * fz + _pp(1);
    // right camera
    //double u2 = (x - w*_b) * fz + _pp(0);
    double u2 = u -_b*fz;
    //bool valid = ((w==0&&z>0) || (z/w) > 0.); // infront of camera?
    bool valid = ( z > 0.0); // infront of camera?
/*
    std::cout << "Xw: " << Xw << std::endl;
    std::cout << "Pose: " << pose << std::endl;
    std::cout << "[x, y, z, w]: "  << x << ", " << y << ", " << z << ", " << w << std::endl;
    std::cout << "[_f, _pp(0), _pp(1)]: "  << _f << ", " << _pp(0) << ", " << _pp(1) << std::endl;
    std::cout << "[fz, u, v, u2]: " << fz << ", " << u << ", " << v << ", " << u2 << std::endl;
    std::cout << "valid: " << valid << std::endl << std::endl;
*/
    if (valid == false) {
    	std::cout << "invalid." << std::endl;
    }

    return StereoMeasurement(u, v, u2, valid);
  }

  StereoMeasurement project(const cameraPose3d& pose, const Point3dh& Xw) const {
    Point3dh X = pose.transform_to(Xw);
    // camera system has z pointing forward, instead of x
/*    double x = X.y();
    double y = X.z();
    double z = X.x();
*/
    double x = -X.y();
    double y = -X.z();
    double z = X.x();

//    double x = X.x();
//    double y = X.y();
//	double z = X.z();

//    double w = X.w();
    // left camera
    double fz = _f / z;
    double u = x * fz + _pp(0);
    double v = y * fz + _pp(1);
    // right camera
    //double u2 = (x - w*_b) * fz + _pp(0);
    double u2 = u -_b*fz;
    //bool valid = ((w==0&&z>0) || (z/w) > 0.); // infront of camera?
    bool valid = ( z > 0.0); // infront of camera?
/*
    std::cout << "Xw: " << Xw << std::endl;
    std::cout << "Pose: " << pose << std::endl;
    std::cout << "[x, y, z]: "  << x << ", " << y << ", " << z << ", " << std::endl;
    std::cout << "[_f, _pp(0), _pp(1)]: "  << _f << ", " << _pp(0) << ", " << _pp(1) << std::endl;
    std::cout << "[fz, u, v, u2]: " << fz << ", " << u << ", " << v << ", " << u2 << std::endl;
    std::cout << "valid: " << valid << std::endl << std::endl;
*/
    if (valid == false) {
    	std::cout << "invalid." << std::endl;
    }

    return StereoMeasurement(u, v, u2, valid);
  }


  Point3dh backproject(const Pose3d& pose, const StereoMeasurement& measure) const {
	 /*
    double lx = (measure.u-_pp(0))*_b;
    double ly = (measure.v-_pp(1))*_b;
    double lz = _f*_b;
    double lw = measure.u - measure.u2;
    if (lw<0.) {
      std::cout << "Warning: StereoCameraDebug.backproject called with negative disparity\n";
    }
    Point3dh X(lz/lw, lx/lw, ly/lw, lw/lw);
*/
	double disparity = measure.u - measure.u2;
	double lz = _f*_b / disparity;
	double lx = (measure.u-_pp(0))*lz / _f;
	double ly = (measure.v-_pp(1))*lz / _f;
	if (disparity<0.) {
	  std::cout << "Warning: StereoCameraDebug.backproject called with negative disparity\n";
	}
//	Point3dh X(lz, lx, ly, 1.0);
	Point3dh X(lz, -lx, -ly, 1.0);
//	Point3dh X(lx, ly, lz, 1.0);

    return pose.transform_from(X);
  }

  Point3dh backproject(const cameraPose3d& pose, const StereoMeasurement& measure) const {
	 /*
    double lx = (measure.u-_pp(0))*_b;
    double ly = (measure.v-_pp(1))*_b;
    double lz = _f*_b;
    double lw = measure.u - measure.u2;
    if (lw<0.) {
      std::cout << "Warning: StereoCameraDebug.backproject called with negative disparity\n";
    }
    Point3dh X(lz/lw, lx/lw, ly/lw, lw/lw);
*/
	double disparity = measure.u - measure.u2;
	double lz = _f*_b / disparity;
	double lx = (measure.u-_pp(0))*lz / _f;
	double ly = (measure.v-_pp(1))*lz / _f;
	if (disparity<0.) {
	  std::cout << "Warning: StereoCameraDebug.backproject called with negative disparity\n";
	}
//	Point3dh X(lz, lx, ly, 1.0);
	Point3dh X(lz, -lx, -ly, 1.0);
//	Point3dh X(lx, ly, lz, 1.0);

    return pose.transform_from(X);
  }

};

/**
 * Stereo observation of a 3D homogeneous point;
 * projective or Euclidean geometry depending on constructor used.
 */
class dynamicStereo_Factor : public FactorT<StereoMeasurement> {
  dynamicPose3d_NL_Node* _pose;
  Point3d_Node* _point;
  Point3dh_Node* _point_h;
  StereoCameraDebug* _camera;

public:

  // constructor for projective geometry
  dynamicStereo_Factor(dynamicPose3d_NL_Node* pose, Point3dh_Node* point, StereoCameraDebug* camera,
                         const StereoMeasurement& measure, const Noise& noise)
    : FactorT<StereoMeasurement>("Stereo_Factor", 3, noise, measure),
      _pose(pose), _point(NULL), _point_h(point), _camera(camera) {
    // StereoCameraDebug could also be a node later (either with 0 variables,
    // or with calibration as variables)
    _nodes.resize(2);
    _nodes[0] = pose;
    _nodes[1] = point;
  }

  // constructor for Euclidean geometry
  // WARNING: only use for points at short range
  dynamicStereo_Factor(dynamicPose3d_NL_Node* pose, Point3d_Node* point, StereoCameraDebug* camera,
                         const StereoMeasurement& measure, const Noise& noise)
    : FactorT<StereoMeasurement>("Stereo_Factor", 3, noise, measure),
      _pose(pose), _point(point), _point_h(NULL), _camera(camera) {
    _nodes.resize(2);
    _nodes[0] = pose;
    _nodes[1] = point;
  }

  void initialize() {
    require(_pose->initialized(), "dynamic Stereo_Factor requires pose to be initialized");
    bool initialized = (_point_h!=NULL) ? _point_h->initialized() : _point->initialized();
    if (!initialized) {
      Point3dh predict = _camera->backproject(_pose->value().getPose3d(), _measure);
      // normalize homogeneous vector
      predict = Point3dh(predict.vector()).normalize();
      if (_point_h!=NULL) {
        _point_h->init(predict);
      } else {
        _point->init(predict.to_point3d());
      }
    }
  }

  Eigen::VectorXd basic_error(Selector s = ESTIMATE) const {
    Point3dh point = (_point_h!=NULL) ? _point_h->value(s) : _point->value(s);
    StereoMeasurement predicted = _camera->project(_pose->value(s).getPose3d(), point);
    if (_point_h!=NULL || predicted.valid == true) {
      return (predicted.vector() - _measure.vector());
    } else {
      std::cout << "Warning - dynamicStereo_Factor.basic_error: point behind camera, dropping term.\n";
      return Eigen::Vector3d::Zero();
    }
  }

};


/**
 * Stereo observation of a 3D homogeneous point;
 * Assumed that the point is on a map that is moving
 * projective or Euclidean geometry depending on constructor used.
 */
class dynamicStereo_MovingMap_Factor : public FactorT<StereoMeasurement> {
  dynamicPose3d_NL_Node* _pose;
  Point3d_Node* _point;
  Point3dh_Node* _point_h;
  StereoCameraDebug* _camera;
  cameraPose3d_Node* _camera_pose3d;

  Point3dh predict_inertial_stored;

public:

  // constructor for projective geometry
  dynamicStereo_MovingMap_Factor(dynamicPose3d_NL_Node* pose, Point3dh_Node* point, StereoCameraDebug* camera, cameraPose3d_Node* camera_pose3d,
                         const StereoMeasurement& measure, const Noise& noise)
    : FactorT<StereoMeasurement>("Stereo_Factor", 3, noise, measure),
      _pose(pose), _point(NULL), _point_h(point), _camera(camera), _camera_pose3d(camera_pose3d) {
    // StereoCameraDebug could also be a node later (either with 0 variables,
    // or with calibration as variables)
    _nodes.resize(3);
    _nodes[0] = pose;
    _nodes[1] = point;
    _nodes[2] = camera_pose3d;
//    _origin_pose3d = Pose3d( -0.65, 0.0, 0.0, 0.0, 0.0, 0.0);
  }

  // constructor for Euclidean geometry
  // WARNING: only use for points at short range
  dynamicStereo_MovingMap_Factor(dynamicPose3d_NL_Node* pose, Point3d_Node* point, StereoCameraDebug* camera, cameraPose3d_Node* camera_pose3d,
                         const StereoMeasurement& measure, const Noise& noise)
    : FactorT<StereoMeasurement>("Stereo_Factor", 3, noise, measure),
      _pose(pose), _point(point), _point_h(NULL), _camera(camera), _camera_pose3d(camera_pose3d) {
    _nodes.resize(3);
    _nodes[0] = pose;
    _nodes[1] = point;
    _nodes[2] = camera_pose3d;
//    _origin_pose3d = Pose3d( -0.65, 0.0, 0.0, 0.0, 0.0, 0.0);
  }

  void initialize() {
/*      if (this->_id == 12) {
    	  std::cout << "found 127\n";
      }
*/
    require(_pose->initialized(), "dynamic Stereo_Factor requires pose to be initialized");
    bool initialized = (_point_h!=NULL) ? _point_h->initialized() : _point->initialized();
    if (!initialized) {
      Point3dh predict_inertial = _camera->backproject(_camera_pose3d->value(), _measure);
      predict_inertial_stored = predict_inertial;

//      std::cout << "predict_inertial_stored: " <<  predict_inertial_stored << std::endl;

//      predict_inertial = Point3dh(predict_inertial.vector()).normalize();
      Point3dh predict_body = _pose->value().transform_to_body(predict_inertial);
      // normalize homogeneous vector
//      predict_body = Point3dh(predict_body.vector()).normalize();
/*
      std::cout << "predict_inertial: " << predict_inertial << std::endl;
      std::cout << "predict_body: " << predict_body << std::endl;
      std::cout << "_measure.vector().transpose(): " << _measure.vector().transpose() << std::endl;
      std::cout << " _point->unique_id(): " <<  _point->unique_id() << std::endl;
      std::cout << "_pose->unique_id: " << _pose->unique_id() << std::endl;
      std::cout << "this->unique_id(): " << this->unique_id() << std::endl << std::endl;
*/

      if (_point_h!=NULL) {
        _point_h->init(predict_body);
      } else {
        _point->init(predict_body.to_point3d());
      }
    }
  }

  Eigen::VectorXd basic_error(Selector s = ESTIMATE) const {
    Point3dh point = (_point_h!=NULL) ? _point_h->value(s) : _point->value(s);
    Point3dh inertialPoint = _pose->value(s).transform_to_inertial(point);
    StereoMeasurement predicted = _camera->project(_camera_pose3d->value(s), inertialPoint);
/*
    std::cout << "point: " << point << std::endl;
    std::cout << "_pose->value(s).getPose3d(): " << _pose->value(s).getPose3d() << std::endl;
    std::cout << "inertialPoint: " << inertialPoint << std::endl;
    std::cout << "_camera_pose3d->value(s): " << _camera_pose3d->value(s) << std::endl;
    std::cout << "predicted.vector(): " << predicted.vector() << std::endl;
    std::cout << "_measure.vector(): " << _measure.vector() << std::endl;
    std::cout << "predict_inertial_stored: " <<  predict_inertial_stored << std::endl;
    std::cout << " _point->initialized(): " <<  _point->initialized() << std::endl;
    std::cout << " _point->unique_id(): " <<  _point->unique_id() << std::endl;
    std::cout << "_pose->unique_id: " << _pose->unique_id() << std::endl;
    std::cout << "this->_id: " << this->_id << std::endl << std::endl;
*/
    if (_point_h!=NULL || predicted.valid == true) {
      return (predicted.vector() - _measure.vector());
    } else {
      std::cout << "Warning - dynamicStereo_MovingMap_Factor.basic_error: point behind camera, dropping term.\n";
      std::cout << "_camera_pose3d->value(s): " << _camera_pose3d->value(s) << std::endl;
      std::cout << "_pose->value(s): " << _pose->value(s) << std::endl;
      std::cout << "inertialPoint: " << inertialPoint << std::endl << std::endl;
      return Eigen::Vector3d::Zero();
    }
  }

};


#define _CAM_VARIABLE_ 0


class dStereo_MovingMap_CoM_Factor : public FactorT<StereoMeasurement> {
  dynamicPose3d_NL_Node* _pose;
  Point3d_Node* _point;
  Point3dh_Node* _point_h;
  StereoCameraDebug* _camera;
  cameraPose3d_Node* _camera_pose3d;
  kinematicPose3d_Node* _centerOfMass_princAxes;

  Point3dh predict_inertial_stored;

public:

  // constructor for projective geometry
  dStereo_MovingMap_CoM_Factor(dynamicPose3d_NL_Node* pose, Point3dh_Node* point, StereoCameraDebug* camera, cameraPose3d_Node* camera_pose3d, kinematicPose3d_Node* centerOfMass_princAxes,
                         const StereoMeasurement& measure, const Noise& noise)
    : FactorT<StereoMeasurement>("Stereo_Factor COM", 3, noise, measure),
      _pose(pose), _point(NULL), _point_h(point), _camera(camera), _camera_pose3d(camera_pose3d), _centerOfMass_princAxes(centerOfMass_princAxes) {
    // StereoCameraDebug could also be a node later (either with 0 variables,
    // or with calibration as variables)
#if _CAM_VARIABLE_
    _nodes.resize(4);
    _nodes[0] = pose;
    _nodes[1] = com_offset;
    _nodes[2] = camera_pose3d;
    _nodes[3] = point;
#else
    _nodes.resize(3);
    _nodes[0] = pose;
    _nodes[1] = _centerOfMass_princAxes;
    _nodes[2] = point;
#endif

  }

  // constructor for Euclidean geometry
  // WARNING: only use for points at short range
  dStereo_MovingMap_CoM_Factor(dynamicPose3d_NL_Node* pose, Point3d_Node* point, StereoCameraDebug* camera, cameraPose3d_Node* camera_pose3d, kinematicPose3d_Node* centerOfMass_princAxes,
                         const StereoMeasurement& measure, const Noise& noise)
    : FactorT<StereoMeasurement>("Stereo_Factor COM", 3, noise, measure),
      _pose(pose), _point(point), _point_h(NULL), _camera(camera), _camera_pose3d(camera_pose3d), _centerOfMass_princAxes(centerOfMass_princAxes) {
#if _CAM_VARIABLE_
    _nodes.resize(4);
    _nodes[0] = pose;
    _nodes[1] = com_offset;
    _nodes[2] = camera_pose3d;
    _nodes[3] = point;
#else
    _nodes.resize(3);
    _nodes[0] = pose;
    _nodes[1] = _centerOfMass_princAxes;
    _nodes[2] = point;
#endif
    }

  void initialize() {
    require(_pose->initialized(), "dynamic Stereo_Factor requires pose to be initialized");
    if(!_centerOfMass_princAxes->initialized()) {
    	kinematicPose3d com_pa_init;
    	_centerOfMass_princAxes->init(com_pa_init);
    }
//    std::cout << "_com_offset: " << _com_offset->value() << std::endl;
    bool initialized = (_point_h!=NULL) ? _point_h->initialized() : _point->initialized();
    if (!initialized) {
      Point3dh predict_inertial = _camera->backproject(_camera_pose3d->value(), _measure);
      predict_inertial_stored = predict_inertial;

//      std::cout << "predict_inertial_stored: " << predict_inertial_stored << std::endl;

      Point3dh predict_body = _pose->value().transform_to_body(predict_inertial);
//      std::cout << "predict_body: " << predict_body << std::endl;

      Point3dh predict_feature(_centerOfMass_princAxes->value().oTw() * predict_body.vector());

      //subtract Center of mass offset
      Vector3d vec_point_feat_frame = predict_feature.vector().head(3);// - _com_offset->value().vector();
      Point3dh point_com = Point3dh(Point3d(vec_point_feat_frame));

//      std::cout << "vec_point_feat_frame: " << vec_point_feat_frame.transpose() << std::endl;
//      std::cout << "point_com: " << point_com << std::endl;

      if (_point_h!=NULL) {
        _point_h->init(point_com);
      } else {
        _point->init(point_com.to_point3d());
      }
    }
  }

  Eigen::VectorXd basic_error(Selector s = ESTIMATE) const {
	//point in body feature frame
    Point3dh point = (_point_h!=NULL) ? _point_h->value(s) : _point->value(s);

    //add center of mass offset
    Vector4d vec_point_feat_frame;
    vec_point_feat_frame << point.vector().head(3), 1.0;
    Vector3d vec_point_com_frame =  (_centerOfMass_princAxes->value(s).wTo()*vec_point_feat_frame).head(3);
    Point3dh point_com = Point3dh(Point3d(vec_point_com_frame));
/*
    std::cout << "point: " << point << std::endl;
    std::cout << "_centerOfMass_princAxes->value(s): " << _centerOfMass_princAxes->value(s) << std::endl;
    std::cout << "vec_point_feat_frame: " << vec_point_feat_frame << std::endl;
    std::cout << "vec_point_feat_frame: " << vec_point_feat_frame << std::endl;
    std::cout << "vec_point_com_frame: " << vec_point_com_frame.transpose() << std::endl;
    std::cout << "point_com: " << point_com << std::endl;
*/
    //transform from body frame to inertial frame
    Point3dh inertialPoint = _pose->value(s).transform_to_inertial(point_com);

//    std::cout << "inertialPoint: " << inertialPoint << std::endl;

    //project into camera
    StereoMeasurement predicted = _camera->project(_camera_pose3d->value(s), inertialPoint);

//    std::cout << "predicted: " << predicted << std::endl;

    //create error measurement
    if (_point_h!=NULL || predicted.valid == true) {
      return (predicted.vector() - _measure.vector());
    } else {
      std::cout << "Warning - dynamicStereo_MovingMap_Factor.basic_error: point behind camera, dropping term.\n";
      std::cout << "_camera_pose3d->value(s): " << _camera_pose3d->value(s) << std::endl;
      std::cout << "_pose->value(s): " << _pose->value(s) << std::endl;
      std::cout << "inertialPoint: " << inertialPoint << std::endl << std::endl;
      return Eigen::Vector3d::Zero();
    }
  }

};

class Point3d_Factor : public FactorT<Point3d> {
  Point3d_Node* _point;

public:

  Point3d_Factor(Point3d_Node* point, const Point3d& prior, const Noise& noise)
    : FactorT<Point3d>("Point3d_Factor", 3, noise, prior), _point(point) {
    _nodes.resize(1);
    _nodes[0] = point;
  }

  void initialize() {
    if (!_point->initialized()) {
      Point3d predict = _measure;
      _point->init(predict);
    }
  }

  Eigen::VectorXd basic_error(Selector s = ESTIMATE) const {
    return (_point->vector(s) - _measure.vector());
  }

};


template <class T> void NodeExmapT<T>::apply_exmap(const Eigen::VectorXd& v) {
	*_value = _value0->exmap_reset(v);

	//update factor noise
	std::list<Factor*> factor_list= this->factors();
	for (std::list<Factor*>::iterator it = factor_list.begin(); it != factor_list.end(); it++) {
		Factor* factor = *it;
		dynamicPose3d_NL_dynamicPose3d_NL_Factor * dynamic_factor;
		dynamic_factor = dynamic_cast<dynamicPose3d_NL_dynamicPose3d_NL_Factor *>(factor);
		if (dynamic_factor !=0) {
			if (dynamic_factor->checkPose1(this)) {
				//std::cout << "Found Dynamic Factor in apply_exmap(), adjusting noise" << std::endl;
				Eigen::MatrixXd sqrtinf = dynamic_factor->get_sqrtinf();
				Noise newnoise = isam::SqrtInformation(sqrtinf);
				dynamic_factor->setNoise(newnoise);
			}
		}

	}
}

}

#endif
