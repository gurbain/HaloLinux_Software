/*
 * rigidBodyDynamics.cpp
 *
 *  Created on: Nov 23, 2012
 *      Author: tweddle
 */

#include "rigidBodyDynamics.h"

rigidBodyDynamics::rigidBodyDynamics(isam::inertiaRatios ir, double sigma_v, double sigma_w) {
	covProp = true;


	//_J = Matrix3d::Identity();
	_ir = ir;
	_qref << 0, 0, 0, 1;

	_r = Vector3d::Zero();
	_v = Vector3d::Zero();
	_a = Vector3d::Zero();
	_w = Vector3d::Zero();

	setMassProperties(ir);
	setCovProperties(sigma_v, sigma_w);
//	setMassProperties(DEFAULT_M, DEFAULT_J11, DEFAULT_J22, DEFAULT_J33);
//	setCovProperties(DEFAULT_SIGMA_V, DEFAULT_SIGMA_W);
/*
	std::cout << "m: " << _m << std::endl;
	std::cout << "J: " << _J << std::endl;
	std::cout << "sigma_v: " << _sigma_v << std::endl;
	std::cout << "sigma_w: " << _sigma_w << std::endl << std::endl;
*/
}

rigidBodyDynamics::rigidBodyDynamics() {
	covProp = true;
	_qref << 0, 0, 0, 1;

	_r = Vector3d::Zero();
	_v = Vector3d::Zero();
	_a = Vector3d::Zero();
	_w = Vector3d::Zero();

}


void rigidBodyDynamics::setMassProperties(isam::inertiaRatios ir) {
	_ir = ir;
}

void rigidBodyDynamics::setCovProperties(double sigma_v, double sigma_w) {
	_sigma_v = sigma_v;
	_sigma_w = sigma_w;
	_Q = Matrix<double,6,6>::Zero();
	_Q.block<3,3>(0,0) = _sigma_v * _sigma_v * Matrix<double,3,3>::Identity();
	_Q.block<3,3>(3,3) = _sigma_w * _sigma_w * Matrix<double,3,3>::Identity();
}

void rigidBodyDynamics::reset_qref() {
	Vector3d a_ = _a;
	Vector4d qref_ = _qref;
	_qref = addQuaternionError(a_, qref_);
	_a = Vector3d::Zero();
}

Vector4d rigidBodyDynamics::qTotal() const {
	Vector3d a_ = _a;
	Vector4d qref_ = _qref;
	return addQuaternionError(a_, qref_);
};

VectorXd rigidBodyDynamics::f(VectorXd x) {
	Vector3d dr, dv, da, dw;
	Matrix<double,12,12> lambda, dLambda;
	VectorXd vec_dLambda;

	int vecsize;
	if (covProp) {
		vecsize = 90;
	} else {
		vecsize = 12;
	}

	VectorXd dx(vecsize);

	Vector3d r = x.segment<3>(0);
	Vector3d v = x.segment<3>(3);
	Vector3d a = x.segment<3>(6);
	Vector3d w = x.segment<3>(9);

	MatrixXd Bw = getBw();
	Matrix3d J = _ir.getJ();


	//Nonlinear State Model \dot x = f(x)

	/*
	 * 	\mathbf{\dot r} = \mathbf{v}
	 */
	dr = v;

	/*
	 * 	\mathbf{\dot v} = 0
	 */
	dv = Vector3d::Zero();

	/*
	 * \frac{d \mathbf{a}_p}{dt} =
	 * 			\frac{1}{2}\left(\mathbf{[\omega \times]} +
	 * 			\mathbf{\omega} \cdot \mathbf{\bar q} \right) \mathbf{a}_p +
	 * 			\frac{2 q_4}{1+q_4} \mathbf{\omega}
	 */
	double c1, c2, c3;
	c1 = 0.5;
	c2 = 0.125 * w.dot(a);		//NEW simplification
	c3 = 1 - a.dot(a)/16;
	da = -c1 * w.cross(a) + c2* a + c3 * w;

	/*
	 * \dot \mathbf{w} = -\mathbf{J}^{-1} \mathbf{\omega} \times \mathbf{J} \mathbf{\omega}
	 */
	dw = - J.inverse() * w.cross(J * w);

	if (covProp) {

		//Covariance Propagation according to Lyapunov function
		//see Brown & Hwang pg 204

		//Compute Linear transition matrix
		Matrix<double,12,12> A = Matrix<double,12,12>::Zero();

		//position derivative
		A.block<3,3>(0,3) = Matrix<double,3,3>::Identity();

		//mrp kinematics
		A.block<3,3>(6,6) = -0.5*crossProductMat(w) + w.dot(a)/8 * Matrix3d::Identity();
		A.block<3,3>(6,9) = (1-a.dot(a/16))*Matrix3d::Identity();

		//angular velocity dynamics
		A.block<3,3>(9,9) = - J.inverse() * crossProductMat(w) * J;


		lambda = vec2symmMat(x.segment<78>(12));
		dLambda = A * lambda + lambda *A.transpose() + Bw * _Q * Bw.transpose();
		vec_dLambda = symmMat2Vec(dLambda);
	}

	//write to dx
	dx.segment<3>(0) = dr;
	dx.segment<3>(3) = dv;
	dx.segment<3>(6) = da;
	dx.segment<3>(9) = dw;
	if(covProp){
		dx.segment<78>(12) = vec_dLambda;
	}

	return dx;
}

Matrix3d rigidBodyDynamics::crossProductMat(Vector3d vec) {
	Matrix3d M = Matrix3d::Zero();

	M(0,1) = -vec(2);
	M(0,2) = vec(1);
	M(1,0) = vec(2);
	M(1,2) = -vec(0);
	M(2,0) = -vec(1);
	M(2,1) = vec(0);

	//std::cout << "Cross Prod vec: " << vec.transpose() << std::endl << "vec cross: " << M << std::endl;
	return M;
}

VectorXd rigidBodyDynamics::symmMat2Vec(Matrix<double, 12, 12> M) {
//OLD
/*	VectorXd v(78);
	//check symmetric
	VectorXd currcol;
	int currind = 0;
	int n;
	for (int i = 0; i < 12; i++) {
		n = 12 - i;
		currcol = M.block(i,i,n,1);
		v.segment(currind,n) = currcol.tail(n);
		currind += n;
	}
	return v;
*/
//NEW
	VectorXd v(78);
	int count = 0;
	for (int row = 0; row < 12; row++) {
		for (int col = row; col < 12; col++) {
			v(count) = M(row,col);
			count++;
		}
	}
	return v;

}

Matrix<double, 12, 12> rigidBodyDynamics::vec2symmMat(VectorXd v) {

	//OLD
/*	Matrix<double, 12, 12> M = Matrix<double, 12, 12>::Zero();

		int currind = 0;
		int n;
		for (int i = 0; i < 12; i++) {
			n = 12 - i;
			M.block(i,i,n,1) = v.segment(currind, n);
			currind += n;
		}

		VectorXd diag = M.diagonal();
		MatrixXd diagM = diag.asDiagonal();
		MatrixXd Mtranspose = M.transpose();

		M += Mtranspose - diagM;
		return M;
*/
		//NEW
	Matrix<double, 12, 12> M = Matrix<double, 12, 12>::Zero();
	int count = 0;
	for (int row = 0; row < 12; row++) {
		for (int col = row; col < 12; col++) {
			M(row,col) = v(count);
			M(col,row) = v(count);
			count++;
		}
	}
	return M;

}

VectorXd rigidBodyDynamics::x() const{
	VectorXd x(12);
	x.segment<3>(0) = _r;
	x.segment<3>(3) = _v;
	x.segment<3>(6) = _a;
	x.segment<3>(9) = _w;
	return x;
}

void rigidBodyDynamics::setState(VectorXd x, Vector4d q) {
	_r = x.segment<3>(0);
	_v = x.segment<3>(3);
	_a = x.segment<3>(6);
	_w = x.segment<3>(9);
	_qref = q / q.norm();
}

void rigidBodyDynamics::setState(VectorXd x) {
	_r = x.segment<3>(0);
	_v = x.segment<3>(3);
	_a = x.segment<3>(6);
	_w = x.segment<3>(9);
/*	std::cout << "_qref: " << _qref.transpose() << std::endl;
	if (isnan(_qref(1))) {
		std::cout << "Nan qref"<<std::endl;
	}
*/
}

Vector4d rigidBodyDynamics::mrp2quaternion(Vector3d mrp) const{
	Vector4d dq;
	dq << 8*mrp / (16 + mrp.transpose() * mrp), (16 - mrp.transpose() * mrp) / (16+mrp.transpose() * mrp);
//	std::cout << "MRP: " << mrp.transpose() << std::endl;
//	std::cout << "dq_prenorm: " << dq.transpose() << std::endl;
	dq /=dq.norm();
//	std::cout << "dq_postnorm: " << dq.transpose() << std::endl;

	return dq;
}

Vector3d rigidBodyDynamics::quaternion2mrp(Vector4d q) const{
	Vector3d mrp;
	if (q(3) < 0) {
		q = -q;

		//std::cout << "quat2mrp reverse quat: " <<q.transpose() <<  std::endl;
	}

	mrp << 4*q(0)/(1+q(3)), 4*q(1)/(1+q(3)), 4*q(2)/(1+q(3));
	return mrp;
}

Vector4d rigidBodyDynamics::addQuaternionError(Vector3d& mrp, Vector4d& qref) const{
	Vector4d qnew, dq;
	dq = mrp2quaternion(mrp);

	Vector4d qnew1 = quaternionMultiplication(dq, qref);

	if (qnew1.dot(qref) >= 0) {
		return qnew1;
	} else {
		Vector4d qnew2 = -1 * qnew1;
		return qnew2;
	}

}

Vector4d rigidBodyDynamics::quaternionMultiplication(Vector4d& q1, Vector4d& q2) const {
	//q1 \mult q2
	Matrix4d qm;
	Vector4d result;
	qm << 	q1(3),	q1(2),	-q1(1),	q1(0),
			-q1(2),	q1(3),	q1(0),	q1(1),
			q1(1),	-q1(0),	q1(3),	q1(2),
			-q1(0),	-q1(1),	-q1(2),	q1(3);

	result = qm*q2;
	result /= result.norm();

	return result;
}


Vector4d rigidBodyDynamics::quaternionDivision(Vector4d& q1, Vector4d& q2) const {
	Vector4d q2inv;

	q2inv << -q2(0) , -q2(1) , -q2(2) , q2(3);

	Vector4d result = quaternionMultiplication(q1,q2inv);
	return result;
}

Vector3d rigidBodyDynamics::diffQuaternion(Vector4d& q, Vector4d& qprev, double dt) const {
	/*
    qm = qinv.*qcurr;
    M = [   qm(4)    qm(3)    -qm(2)   -qm(1);
            -qm(3)   qm(4)    qm(1)    -qm(2);
            qm(2)    -qm(1)   qm(4)    -qm(3);
            qm(1)    qm(2)    qm(3)    qm(4)];
    w = 2*M*dq'
    */

	Vector4d dq = (q - qprev) / dt;
	Matrix4d M;

	M << 	qprev(3) , qprev(2),  -qprev(1),   -qprev(0),
	        -qprev(2),   qprev(3),   qprev(0),  -qprev(1),
	        qprev(1),  -qprev(0),   qprev(3),   -qprev(2),
	        qprev(0),  qprev(1),  qprev(2),  qprev(3);

	Vector4d wp = 2*M*dq;
	Vector3d w = wp.head(3);

	return w;
}


Matrix3d rigidBodyDynamics::rotationMatrix(Vector4d& q) const {
	Matrix3d rot;

	rot(0,0) = q(0)*q(0)-q(1)*q(1)-q(2)*q(2)+q(3)*q(3);
	rot(0,1) = 2*(q(0)*q(1)+q(2)*q(3));
	rot(0,2) = 2*(q(0)*q(2)-q(1)*q(3));

	rot(1,0) = 2*(q(0)*q(1)-q(2)*q(3));
	rot(1,1) = -q(0)*q(0)+q(1)*q(1)-q(2)*q(2)+q(3)*q(3);
	rot(1,2) = 2*(q(2)*q(1)+q(0)*q(3));

	rot(2,0) = 2*(q(0)*q(2)+q(1)*q(3));
	rot(2,1) = 2*(q(2)*q(1)-q(0)*q(3));
	rot(2,2) = -q(0)*q(0)-q(1)*q(1)+q(2)*q(2)+q(3)*q(3);

	return rot;
}

Vector4d rigidBodyDynamics::quaternionFromRot(Matrix3d& R) const{
	Vector4d q;
	double div1, div2, div3, div4;

	double numerical_limit = 1.0e-4;

	if (abs(R.determinant()-1) > numerical_limit ) {
		std::cerr << "R does not have a determinant of +1" << std::endl;
	} else {
		div1 = 0.5*sqrt(1+R(0,0)+R(1,1)+R(2,2));
		div2 = 0.5*sqrt(1+R(0,0)-R(1,1)-R(2,2));
		div3 = 0.5*sqrt(1-R(0,0)-R(1,1)+R(2,2));
		div4 = 0.5*sqrt(1-R(0,0)+R(1,1)-R(2,2));

		//if (div1 > div2 && div1 > div3 && div1 > div4) {
		if (fabs(div1) > numerical_limit) {
			q(3) = div1;
			q(0) = 0.25*(R(1,2)-R(2,1))/q(3);
			q(1) = 0.25*(R(2,0)-R(0,2))/q(3);
			q(2) = 0.25*(R(0,1)-R(1,0))/q(3);
		} else if (fabs(div2) > numerical_limit) {
		//} else if (div2 > div1 && div2 > div3 && div2 > div4) {
			q(0) = div2;
			q(1) = 0.25*(R(0,1)+R(1,0))/q(0);
			q(2) = 0.25*(R(0,2)+R(2,0))/q(0);
			q(3) = 0.25*(R(1,2)+R(2,1))/q(0);
		} else if (fabs(div3) > numerical_limit) {
		//} else if (div3 > div1 && div3 > div2 && div3 > div4) {
			q(2) = div3;
			q(0) = 0.25*(R(0,2)+R(2,0))/q(2);
			q(1) = 0.25*(R(1,2)+R(2,1))/q(2);
			q(3) = 0.25*(R(0,1)-R(1,0))/q(2);
		//} else {
		} else if (fabs(div4) > numerical_limit) {
			q(1) = div4;
			q(0) = 0.25*(R(0,1)+R(1,0))/q(1);
			q(2) = 0.25*(R(1,2)+R(2,1))/q(1);
			q(3) = 0.25*(R(2,0)-R(0,2))/q(1);
		} else {
			std::cerr << "quaternionFromRot didn't convert: [" << div1 << ", " << div2 << ", " << div3 << ", " << div4 << std::endl;
			std::cerr << "Rotation Matrix: " << R << std::endl;
		}
	}
/*
	if (q(3) < 0) {
		q *= -1;
	}
*/
	q /=q.norm();

	return q;
}

MatrixXd rigidBodyDynamics::getBw() const {
	Matrix<double, 12,6> Bw;
	Bw = Matrix<double,12,6>::Zero();
/*	Bw.block<3,3>(3,0) = 1/_m * Matrix<double,3,3>::Identity();
	Bw.block<3,3>(9,3) = _ir.getJ().inverse();
*/

	Bw.block<3,3>(3,0) = Matrix3d::Identity();
	Bw.block<3,3>(9,3) = Matrix3d::Identity();

	return Bw;
}


Matrix3d rigidBodyDynamics::getJ() const{
	return _ir.getJ();
}

isam::inertiaRatios rigidBodyDynamics::getIR() const{
	return _ir;
}

void rigidBodyDynamics::setIR(isam::inertiaRatios ir) {
	_ir = ir;
}

/*double rigidBodyDynamics::getMass() const{
	return _m;
}
*/
double rigidBodyDynamics::getSigmaV() const{
	return _sigma_v;
}

double rigidBodyDynamics::getSigmaW() const {
	return _sigma_w;
}

