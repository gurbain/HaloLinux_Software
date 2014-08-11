
#pragma once

#include <ostream>
#include <Eigen/Dense>
#include "isam/isam.h"
#include "rigidBodyDynamics.h"
#include "FactorVariableNoise.h"


using namespace Eigen;
namespace isam{

class dynamicPose3d_NL {
	friend std::ostream& operator<<(std::ostream& out, const dynamicPose3d_NL& p)
	{
		p.write(out);
		return out;
	}

	//rigidBodyDynamics* rbd;
	rigidBodyDynamics rbd;
public:
  EIGEN_MAKE_ALIGNED_OPERATOR_NEW
  // assignment operator and copy constructor implicitly created, which is ok
  static const int dim = 12;
  static const char* name() {
	return "dynamicPose3d_NL";
  }

  Noise* factor_noise;		//check if this is ever used

  dynamicPose3d_NL(inertiaRatios ir, double sigma_v, double sigma_w) : rbd(ir, sigma_v, sigma_w) {
	  //rbd = ;
  }

  //copy constructor
  dynamicPose3d_NL(const dynamicPose3d_NL& cSource) :
	  rbd(cSource.rbd.getIR(), cSource.rbd.getSigmaV(), cSource.rbd.getSigmaW() )
  {
	  rbd.setState(cSource.rbd.x(), cSource.rbd.qref());
  }

  dynamicPose3d_NL& operator= (const dynamicPose3d_NL& cSource) {
	  rbd = rigidBodyDynamics(cSource.rbd.getIR(), cSource.rbd.getSigmaV(), cSource.rbd.getSigmaW() );
	  rbd.setState(cSource.rbd.x(), cSource.rbd.qref());
	  return *this;
  }

  dynamicPose3d_NL(VectorXd x, inertiaRatios ir, double sigma_v, double sigma_w)
  : rbd(ir, sigma_v, sigma_w)
  {
	  Vector4d qref;
	  qref << 0, 0, 0, 1;
	  if (x.size() == 12) {
		  rbd.setState(x,qref);
	  }
  }

  dynamicPose3d_NL(VectorXd x, Vector4d qref, inertiaRatios ir, double sigma_v, double sigma_w)
  : rbd(ir, sigma_v, sigma_w)
  {
	  if (x.size() == 12) {
		  rbd.setState(x,qref);
	  }
  }

/*
  ~dynamicPose3d_NL() {
	  delete rbd;
	  rbd = NULL;
  }
*/

  dynamicPose3d_NL(const Matrix4d& hm, bool initVelocities, double dt, inertiaRatios ir, double sigma_v, double sigma_w)
  : rbd(ir, sigma_v, sigma_w)
  {
      Matrix<double,12,1> x;
      Vector3d r;
      Vector3d v;
      Vector3d a;
      Vector4d q;
      Vector3d w;

      //Convert matrix to R,T
      Matrix4d HM = hm / hm(3,3); // enforce T(3,3)=1
      Matrix3d R = HM.topLeftCorner(3,3);
      Vector3d T = HM.col(3).head(3);

      //compute quaternion
      q = rbd.quaternionFromRot(R);
      a = Vector3d::Zero();

//      std::cout << "R: " << std::endl << R.transpose() << std::endl;
//      std::cout << "q: " << q.transpose() << std::endl;

      if (initVelocities) {
	      //differentiate linear velocity
	      v = T / dt;

	      /* Differentiate quaternion:
	       * dq/dt = (q[k] - q[k-1])/dt = 0.5 O(w[k-1]) q[k-1]
	       * where O(w[k-1]) is an orthonormal quaternion mult matrix for [w1; w2; w3; 0] (i.e. quaternionMultiplication())
	       * set q[k-1] = [0;0;0;1] by definition (from a refererence frame) and solve for w[k-1] gives
	       * w[k-1] = 2 [q1[k]; q2[k]; q3[k]] / dt
	       */
	      w = 2*q.head(3) / dt;
      } else {
    	  v = Vector3d::Zero();
    	  w = Vector3d::Zero();
      }

      x.block<3,1>(0,0) = T;
      x.block<3,1>(3,0) = v;
      x.block<3,1>(6,0) = a;
      x.block<3,1>(9,0) = w;
/*
      std::cout << "T: " << T.transpose() << std::endl;
      std::cout << "v: " << v.transpose() << std::endl;
      std::cout << "a: " << a.transpose() << std::endl;
      std::cout << "w: " << w.transpose() << std::endl;
*/
      rbd.setState(x,q);
  }

  VectorXd x() { return rbd.x();};
  Vector4d q() { return rbd.qref();};
  Vector4d qTotal() const { return rbd.qTotal(); };
//  Vector4d addQuat(Vector4d& q) { return rbd.quaternionMultiplication(q,rbd.qTotal());}

  dynamicPose3d_NL exmap(const Matrix<double,12,1>& delta) const {
	  dynamicPose3d_NL res = *this;
	  //std::cout << "delta: " << delta.transpose() << std::endl;
	  res.rbd.setState(res.rbd.x() + delta);
	  return res;
	}

  dynamicPose3d_NL exmap_reset(const Matrix<double,12,1>& delta)  {
	  dynamicPose3d_NL res = *this;
	  res.rbd.setState(res.rbd.x() + delta);
	  res.rbd.reset_qref();

	/* We should REALLY, REALLY update Factor::_sqrtinf at this location
	 * in the code. However it is a const variable, and there is no way
	 * to do callbacks to the Factor class. So I will leave this for future
	 * work. Now, the value that is created on initialization is the final
	 * version, even after many relinearizations.
	 */

	//NOTE - THIS IS NOW DONE in NodeExmapT->apply_reset();

//    std::cout << "exmap_reset: " << delta << std::endl << "    res: " << res << std::endl;

	return res;
  }

  void set(const VectorXd& v) {
	  rbd.setState(v);
  }

  void set_qref(const Vector4d& qset) {
	  rbd.setState(rbd.x(), qset);
  }

  void rezero() {
	  VectorXd x = VectorXd::Zero(12);
	  Vector4d q;
	  q << 0 , 0, 0, 1;
	  rbd.setState(x,q);
  }


  dynamicPose3d_NL propagate(double dt, inertiaRatios& ir) {
	  VectorXd x0 = VectorXd::Zero(90);
	  x0.head(12) = rbd.x();
	  rbd.setIR(ir);
//	  std::cout << "x0: " << x0.head(12).transpose() << std::endl;
	  VectorXd newX = rbd.propagateRK4_adaptive(dt, x0).head(12);

//	  std::cout << "dt: " << dt << std::endl;
//	  std::cout << "newX: " << newX.transpose() << std::endl;

	  dynamicPose3d_NL xNew(newX, this->rbd.qref(), this->rbd.getIR(), this->rbd.getSigmaV(), this->rbd.getSigmaW());
	  xNew.exmap(Matrix<double,12,1>::Zero());
	  return xNew;
  }

  Vector3d getMRPdifference(Vector4d qtot1, Vector4d qtot2)  {
	  Vector4d dq = rbd.quaternionDivision(qtot1,qtot2);
	  Vector3d da = rbd.quaternion2mrp(dq);
	  return da;
  }

  //compute the control input using w_t = x_{t+1} - \int_t^{t+1}f(x_t)
  VectorXd computeStateChange(dynamicPose3d_NL& prev, double dt, inertiaRatios& ir) {
	  VectorXd w;

//		  std::cout << "x_prev: " << prev.x().transpose() << std::endl;
	  dynamicPose3d_NL predicted = prev.propagate(dt, ir);
/*
	  Vector4d q, dq, qprev, qprev_inv;
	  Vector3d mrp, omega;

	  VectorXd xprev = predicted.x();

	  w = this->x() - xprev;

//		  std::cout << "x: " << this->x().transpose() << std::endl;

		  //		  std::cout << "predicted x: " << predicted.x().transpose() << std::endl;

	  //add mrp terms
	  //subtract quaternion
	  qprev = prev.q();
	  qprev_inv << -qprev(0), -qprev(1), -qprev(2), qprev(3);
	  q = this->q();
//	  	std::cout << "q: " << q.transpose() << std::endl;
//	  	std::cout << "qprev: " << qprev.transpose() << std::endl;
	  dq = rbd.quaternionMultiplication(q, qprev_inv);
//		  std::cout << "dq: " << dq.transpose() << std::endl;
	  mrp = rbd.quaternion2mrp(dq);
//		  std::cout << "mrp: " << mrp.transpose() << std::endl;
	  omega = rbd.diffQuaternion(q, qprev, dt);
//		  std::cout << "omega: " << omega.transpose() << std::endl;

//		  std::cout << "w pre: " << w.transpose() << std::endl;
	  //add to w
	  w(6) += mrp(0);
	  w(7) += mrp(1);
	  w(8) += mrp(2);
//	  w(9) += omega(0);
//	  w(10) += omega(1);
//	  w(11) += omega(2);

//		  std::cout << "w post: " << w.transpose() << std::endl;
*/

/*
	  VectorXd xpred = predicted.x();
	  Vector3d apred = xpred.segment<3>(6);
	  Vector4d qpred = rbd.mrp2quaternion(apred);
*/
/*
	  std::cout << "Predicted: " << predicted << std::endl;
	  std::cout << "xpred: " << xpred.transpose() << std::endl;
	  std::cout << "apred: " << apred.transpose() << std::endl;
	  std::cout << "qpred: " << qpred.transpose() << std::endl;
*/
/*
	  Vector4d qcurr_ref = this->q();
	  Vector4d qcurr_inv;
	  qcurr_inv << -qcurr_ref(0), -qcurr_ref(1), -qcurr_ref(2), qcurr_ref(3);
	  Vector4d dq = rbd.quaternionMultiplication(qpred, qcurr_inv);
	  Vector3d da = rbd.quaternion2mrp(dq);

	  xpred.segment<3>(6) = da;

	  w = this->x() - xpred;
*/
/*
	  std::cout << "qcurr_inv: " << qcurr_inv.transpose() << std::endl;
	  std::cout << "dq:        " << dq.transpose() << std::endl;
	  std::cout << "da:        " << da.transpose() << std::endl;
	  std::cout << "predicted.q(): " << predicted.q().transpose() << std::endl;
	  std::cout << "this->q(): " << this->q().transpose() << std::endl;
	  std::cout << "xpred:     " << xpred.transpose() << std::endl;
	  std::cout << "this->x(): " << this->x().transpose() << std::endl;
	  std::cout << "w:         " << w.transpose() << std::endl;
*/
	  /*
	  Vector4d qcurrTotal = this->qTotal();
	  Vector4d qprevTotal = predicted.qTotal();
	  Vector4d dq = rbd.quaternionDivision(qcurrTotal,qprevTotal);
	  Vector3d da = rbd.quaternion2mrp(dq);
	  */
	  Vector4d qTot = this->qTotal();
	  Vector4d qTotpred = predicted.qTotal();
	  Vector3d da = getMRPdifference(qTot,qTotpred);

	  w = this->x() - predicted.x();
	  w.segment<3>(6) = da;

	  return w;
  }

  Vector6d getOdometry() {
	  Vector6d odo;
	  VectorXd x = rbd.x();
	  Vector4d qref = rbd.qref();
	  Vector3d a = x.segment<3>(6);
//		  std::cout << "qref: " << qref.transpose() << std::endl;
//		  std::cout << "a: " << a.transpose() << std::endl;
	  odo.head(3) = x.segment<3>(0);
	  Vector4d qnew = rbd.addQuaternionError(a,qref);
//		  std::cout << "qnew: " << qnew.transpose() << std::endl;
	  odo.tail(3) = rbd.quaternion2mrp(qnew);
//		  std::cout << "odo: " << odo.transpose() << std::endl;
	  return odo;
  }

  Vector6d getOdometry(dynamicPose3d_NL& prev) {
	  Vector6d odo;
	  VectorXd x, xprev;
	  Vector4d q, qprev;
	  Vector3d a, aprev;

	  x = rbd.x();
	  xprev = prev.x();
	  a = x.segment<3>(6);
//	  std::cout << "a: " << a.transpose() << std::endl;

	  q = rbd.qref();
//	  std::cout << "q: " << q.transpose() << std::endl;

	  qprev = prev.q();
//	  std::cout << "qprev: " << qprev.transpose() << std::endl;

	  aprev = xprev.segment<3>(6);
//	  std::cout << "aprev: " << aprev.transpose() << std::endl;

	  Vector3d dr = x.segment<3>(0) - xprev.segment<3>(0);
//		  std::cout << "r: " << x.segment<3>(0).transpose() << std::endl;
//		  std::cout << "rprev: " << xprev.segment<3>(0).transpose() << std::endl;
//		  std::cout << "dr: " << dr.transpose() << std::endl;
	  Vector4d qtot_this = rbd.addQuaternionError(a, q);
//		  std::cout << "qtot_this: " << qtot_this.transpose() << std::endl;
	  Vector4d qtot_prev = rbd.addQuaternionError(aprev, qprev);
//		  std::cout << "qtot_prev: " << qtot_prev.transpose() << std::endl;

	  Vector4d qprev_inv;
	  qprev_inv << -qtot_prev(0), -qtot_prev(1), -qtot_prev(2), qtot_prev(3);
//		  std::cout << "qprev_inv: " << qprev_inv.transpose() << std::endl;
	  Vector4d qDiff = rbd.quaternionMultiplication(qtot_this, qprev_inv);
//		  std::cout << "qDiff: " << qDiff.transpose() << std::endl;
	  Vector3d mrp = rbd.quaternion2mrp(qDiff);
//		  std::cout << "mrp: " << mrp.transpose() << std::endl;
	  odo.tail(3) = mrp;
//	  Matrix3d rotmat = rbd.rotationMatrix(qDiff).transpose();
//	  odo.tail(3) = rbd.quaternion2mrp(rbd.quaternionFromRot(rotmat));


	  Matrix3d Rprev = rbd.rotationMatrix(qtot_prev);
	  odo.head(3) = Rprev.transpose() * dr;


//	  std::cout << "odo: " << odo.transpose() << std::endl;

	  return odo;
  }

  dynamicPose3d_NL getOdometryPose(dynamicPose3d_NL& prev, bool initVelocities, double dt) {
	  dynamicPose3d_NL newPose(prev.rbd.getIR(), prev.rbd.getSigmaV(), prev.rbd.getSigmaW());
      VectorXd new_x(12);
      Vector3d new_r;
      Vector3d new_v;
      Vector3d new_a;
      Vector4d new_q;
      Vector3d new_w;

	  VectorXd x, xprev;
	  Vector4d q, qprev;
	  Vector3d a, aprev;

	  //get x's
	  x = rbd.x();
	  xprev = prev.x();

	  //attitude gets
	  a = x.segment<3>(6);
	  aprev = xprev.segment<3>(6);
	  q = rbd.qref();
	  qprev = prev.q();
/*
	  std::cout << "a: " << a.transpose() << std::endl;
	  std::cout << "aprev: " << aprev.transpose() << std::endl;
	  std::cout << "q: " << q.transpose() << std::endl;
	  std::cout << "qprev: " << qprev.transpose() << std::endl;
*/

	  //total attitude
	  Vector4d qtot_this = rbd.addQuaternionError(a, q);
	  Vector4d qtot_prev = rbd.addQuaternionError(aprev, qprev);
	  Vector4d qprev_inv;
	  qprev_inv << -qtot_prev(0), -qtot_prev(1), -qtot_prev(2), qtot_prev(3);
	  Vector4d qDiff = rbd.quaternionMultiplication(qtot_this, qprev_inv);
/*
	  std::cout << "qtot_this: " << qtot_this.transpose() << std::endl;
	  std::cout << "qtot_prev: " << qtot_prev.transpose() << std::endl;
	  std::cout << "qprev_inv: " << qprev_inv.transpose() << std::endl;
	  std::cout << "qDiff: " << qDiff.transpose() << std::endl;
*/

	  //previous rotation mat
	  Matrix3d Rprev = rbd.rotationMatrix(qtot_prev);

	  new_r = Rprev.transpose()*(x.segment<3>(0) - xprev.segment<3>(0));
	  Matrix3d Rdiff = rbd.rotationMatrix(qDiff);
	  new_q = rbd.quaternionFromRot(Rdiff);

      if (initVelocities) {
	      //differentiate linear velocity
	      new_v = new_r / dt;

	      /* Differentiate quaternion:
	       * dq/dt = (q[k] - q[k-1])/dt = 0.5 O(w[k-1]) q[k-1]
	       * where O(w[k-1]) is an orthonormal quaternion mult matrix for [w1; w2; w3; 0] (i.e. quaternionMultiplication())
	       * set q[k-1] = [0;0;0;1] by definition (from a refererence frame) and solve for w[k-1] gives
	       * w[k-1] = 2 [q1[k]; q2[k]; q3[k]] / dt
	       */
	      new_w = 2*new_q.head(3) / dt;
      } else {
    	  new_v = Vector3d::Zero();
    	  new_w = Vector3d::Zero();
      }
      new_a = Vector3d::Zero();

      new_x.block<3,1>(0,0) = new_r;
      new_x.block<3,1>(3,0) = new_v;
      new_x.block<3,1>(6,0) = new_a;
      new_x.block<3,1>(9,0) = new_w;
/*
      std::cout << "new_r: " << new_r.transpose() << std::endl;
      std::cout << "new_v: " << new_v.transpose() << std::endl;
      std::cout << "new_a: " << new_a.transpose() << std::endl;
      std::cout << "new_w: " << new_w.transpose() << std::endl;
      std::cout << "new_q: " << new_q.transpose() << std::endl;
*/
      newPose.rbd.setState(new_x, new_q);
  	  return newPose;

    }

  dynamicPose3d_NL adjustAttitude(dynamicPose3d_NL& prev) {
	  Vector4d q, qprev;
	  dynamicPose3d_NL newPose(prev.rbd.getIR(), prev.rbd.getSigmaV(), prev.rbd.getSigmaW());

	  VectorXd x = rbd.x();
	  q = rbd.qTotal();
	  qprev = prev.qTotal();

	  std::cout << "q: " << q.transpose() << std::endl;
	  std::cout << "qprev: " << qprev.transpose() << std::endl;

	  Matrix3d R = rbd.rotationMatrix(q);
	  Matrix3d Rprev = rbd.rotationMatrix(qprev);
	  Matrix3d Rdiff = R * Rprev.transpose();
	  Vector4d new_qdiff = rbd.quaternionFromRot(Rdiff);

	  std::cout << "R: " << R << std::endl;
	  std::cout << "Rprev: " << Rprev << std::endl;
	  std::cout << "Rdiff: " << Rdiff << std::endl;
	  std::cout << "new_qdiff: " << new_qdiff.transpose() << std::endl;

/*
	  Vector4d qprev_inv;
	  qprev_inv << -qprev(0), -qprev(1), -qprev(2), qprev(3);
	  Vector4d qDiff = rbd.quaternionMultiplication(q, qprev_inv);

	  Matrix3d Rdiff = rbd.rotationMatrix(qDiff);
	  Vector4d new_qdiff = rbd.quaternionFromRot(Rdiff);
*/
	  Vector4d qnew = rbd.quaternionMultiplication(new_qdiff, qprev);
//	  std::cout << "Rdiff: " << std::endl << Rdiff << std::endl;

	  std::cout << "qnew aa: " << qnew.transpose() << std::endl << std::endl;
	  if (isnan(qnew(1))) {
		  std::cout << "qnew aa nan\n";
		  new_qdiff = rbd.quaternionFromRot(Rdiff);
	  }

	  x.segment<3>(6) = Vector3d::Zero();
	  rbd.setState(x, qnew);
      newPose.rbd.setState(x, qnew);
      return newPose;

  }

  void shortenQuaternion(dynamicPose3d_NL& prev) {
	  Vector4d q, qprev, qnew;

	  VectorXd x = rbd.x();
	  q = rbd.qTotal();
	  qprev = prev.qTotal();
/*
	  Matrix3d R = rbd.rotationMatrix(q);
	  Matrix3d Rprev = rbd.rotationMatrix(qprev);
	  Matrix3d Rdiff = R * Rprev.transpose();
	  Vector4d new_qdiff = rbd.quaternionFromRot(Rdiff);

	  Vector4d qnew = rbd.quaternionMultiplication(new_qdiff, qprev);
*/
/*	  Vector4d qnew;
	  if (q(3)*qprev(3) < 0) {
		  qnew = -q;
	  } else {
		  qnew = q;
	  }
*/
	  if(q.dot(qprev) < 0) {
		  qnew = -q;
		  x.segment<3>(6) = Vector3d::Zero();
		  rbd.setState(x, qnew);
	  }
	  /*
	  Vector4d qprev_inv;
	  qprev_inv << -qprev(0), -qprev(1), -qprev(2), qprev(3);
	  Vector4d dq = rbd.quaternionMultiplication(q, qprev_inv);
	  if (dq(3) < 0) {
		  dq = -dq;
		  qnew = rbd.quaternionMultiplication(dq,qprev);
	  } else {
		  qnew = q;
	  }
	  x.segment<3>(6) = Vector3d::Zero();
	  rbd.setState(x, qnew);
*/
  }


  dynamicPose3d_NL applyOdometry(dynamicPose3d_NL& prev) {
	  dynamicPose3d_NL newPose(prev.rbd.getIR(), prev.rbd.getSigmaV(), prev.rbd.getSigmaW());
      VectorXd new_x(12);
      Vector3d new_r;
      Vector3d new_v;
      Vector3d new_a;
      Vector4d new_q;
      Vector3d new_w;

	  VectorXd x, xprev;
	  Vector4d q, qprev;
	  Vector3d a, aprev;

	  //get x's
	  x = rbd.x();
	  xprev = prev.x();

	  //attitude gets
	  q = rbd.qTotal();
	  qprev = prev.qTotal();

	  new_q = rbd.quaternionMultiplication(q,qprev);

	  Matrix3d Rprev = rbd.rotationMatrix(qprev);
	  new_r = Rprev * x.head(3) + xprev.head(3);

	  new_v = Vector3d::Zero();
	  new_a = Vector3d::Zero();
	  new_w = Vector3d::Zero();

      new_x.block<3,1>(0,0) = new_r;
      new_x.block<3,1>(3,0) = new_v;
      new_x.block<3,1>(6,0) = new_a;
      new_x.block<3,1>(9,0) = new_w;

      newPose.rbd.setState(new_x, new_q);
      return newPose;
  }


  Matrix4d wTo() const {
    Matrix4d T;

    //error quaternion is applied
    Vector4d qtot = rbd.qTotal();
    VectorXd x = rbd.x();
/*
    T.topLeftCorner(3,3) = rbd.rotationMatrix(qtot);
    T.col(3).head(3) << x.segment<3>(0);
    T.row(3) << 0., 0., 0., 1.;
*/
    T.topLeftCorner(3,3) = rbd.rotationMatrix(qtot).transpose();
    T.col(3).head(3) << x.segment<3>(0);
    T.row(3) << 0., 0., 0., 1.;
    return T;
  }

  Matrix4d oTw() const {
      Matrix4d T;
      Matrix3d R;

      //error quaternion is applied
      Vector4d qtot = rbd.qTotal();
      VectorXd x = rbd.x();
/*
      R = rbd.rotationMatrix(qtot).transpose();

      T.topLeftCorner(3,3) = R;
      T.col(3).head(3) << - R * x.segment<3>(0);
      T.row(3) << 0., 0., 0., 1.;
*/
      R = rbd.rotationMatrix(qtot);

      T.topLeftCorner(3,3) = R;
      T.col(3).head(3) << - R * x.segment<3>(0);
      T.row(3) << 0., 0., 0., 1.;
      return T;
    }


  Pose3d getPose3d() {
	  return Pose3d(this->wTo());		//may be wrong: Mar 25, 2013, B.E.T.
	  //return Pose3d(this->oTw());
  }

  Point3dh transform_to_inertial(const Point3dh& pBody) const{
	  Vector3d p;
	  p << pBody.x(), pBody.y(), pBody.z();
      Vector4d qtot = rbd.qTotal();
      VectorXd x = rbd.x();
      Vector3d T = x.head(3);
/*
      Matrix3d R = rbd.rotationMatrix(qtot);

      Vector3d pInertial = R*p + T;
*/
      Matrix3d Rt = rbd.rotationMatrix(qtot).transpose();

      Vector3d pInertial = Rt*p + T;

	  return Point3dh(pInertial(0), pInertial(1), pInertial(2), 1.0);
  }

  Point3dh transform_to_body(const Point3dh& pInertial) const{
	  Vector3d p;
	  p << pInertial.x(), pInertial.y(), pInertial.z();
      Vector4d qtot = rbd.qTotal();
      VectorXd x = rbd.x();
      Vector3d T = x.head(3);
/*
      Matrix3d Rt = rbd.rotationMatrix(qtot).transpose();

      Vector3d pBody = Rt*(p - T);
*/
      Matrix3d R = rbd.rotationMatrix(qtot);

      Vector3d pBody = R*(p - T);

	  return Point3dh(pBody(0), pBody(1), pBody(2), 1.0);
    }


  Noise getProcessNoise (double dt, inertiaRatios ir) {
	  VectorXd x0 = VectorXd::Zero(90);
	  x0.head(12) = rbd.x();
	  rbd.setIR(ir);
	  VectorXd newLambda = rbd.propagateRK4_adaptive(dt, x0).tail(78);

	  Matrix<double,12,12> lambda = rbd.vec2symmMat(newLambda);
	  Noise n = isam::Covariance(lambda);
	  return n;
  }

  VectorXd vectorFull() const {
	  VectorXd x = rbd.x();
	  Vector4d q = rbd.qref();
	  Vector3d mrp = rbd.quaternion2mrp(q);
	  x(6) += mrp(0);
	  x(7) += mrp(1);
	  x(8) += mrp(2);
	  return x;
  }

  VectorXd vector() const{
	  //return vectorFull();
	  return rbd.x();
	}

  void write(std::ostream &out) const {

	  out << std::endl << "dP3d_NL x: " << rbd.x().transpose() << std::endl;
	  out <<  "dP3d_NL qref: " <<  rbd.qref().transpose() << std::endl;
	  out << std::endl;
  }


};
}
