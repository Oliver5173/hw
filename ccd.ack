#include "InverseKinematics.h"
using namespace Eigen;

float degree2rad = 3.141592658f / 180.0f;
float max_arm_length = 31.56816;

//IK workflow
void InverseKinematics::IK() {

	//check if the endeffector and the target are close enough
	Vector3 endPos = Vector3(endEffector->GlobalPos.x, endEffector->GlobalPos.y, endEffector->GlobalPos.z);
	Vector3 tarPos = Vector3(target->x, target->y, target->z);
	
	int i = 0;
	while ((endPos - tarPos).length() > threshold && i < iterNum)
	{
		if (mode == 0)
		{
			CCDMode();
		}
		else if (mode == 1)
		{
			JacobianMode();
		}
		endPos = Vector3(endEffector->GlobalPos.x, endEffector->GlobalPos.y, endEffector->GlobalPos.z);
		i++;
	}
}

//CCD Mode IK for right arm
void InverseKinematics::CCDMode()
{
	//add your code here
	//hint: Do forward kinematics when the endeffector's global position need to be updated
	//with newly computed quaternions. 

	//I.e., use forwardKinematcis->forwardKinematicsComputation(base) 
	//whenever you need to update endeffector's global position.

	//calclate from second last eddEffector to base
	Joint* currentJoint = endEffector->parent;
	Vector3 toEnd, toTarget;
	while (currentJoint != nullptr)
	{
		toEnd.x = endEffector->GlobalPos.x - currentJoint->GlobalPos.x;
		toEnd.y = endEffector->GlobalPos.y - currentJoint->GlobalPos.y;
		toEnd.z = endEffector->GlobalPos.z - currentJoint->GlobalPos.z;
		toEnd = toEnd.normalize();

		toTarget.x = target->x - currentJoint->GlobalPos.x;
		toTarget.y = target->y - currentJoint->GlobalPos.y;
		toTarget.z = target->z - currentJoint->GlobalPos.z;
		toTarget = toTarget.normalize();

		float dotproduct = toEnd.dot(toTarget);
		if (dotproduct >= 1) return;
		float theta = acos(dotproduct);
		//std::cout << theta << "\t"<< toEnd  << "\t" << toTarget << "\t" << toEnd.dot(toTarget)  <<std::endl;
		Vector3 r_axis = toEnd.cross(toTarget);
		r_axis = r_axis.normalize();
		Vector4 quaternion = forwardKinematcis->buildQuaternionRotation(theta/degree2rad, r_axis.x, r_axis.y, r_axis.z);
		currentJoint->Globalquat = forwardKinematcis->quaternionMultiplication(quaternion, currentJoint->Globalquat);
		forwardKinematcis->forwardKinematicsComputation(currentJoint);

		if (currentJoint == base)
		{
			break;
		}
		currentJoint = currentJoint->parent;
	}
}

//Entire Right Arm Jacobian IK
void InverseKinematics::JacobianMode()
{
	//J[3,4] 4joints,xyz axis
	//each row J = [Rx * (EndPos - joint),...]
	//deltaO = (J+)V
	//V - changes of spatial location;

	//initialization
	Joint* currentJoint = endEffector->parent;

	//check range
	Vector3 baseTotarget(target->x - base->GlobalPos.x, target->y - base->GlobalPos.y, target->z - base->GlobalPos.z);
	if (baseTotarget.length() >= max_arm_length - threshold)
	{
		Vector3 axis(-1,0,0), r_axis;
		baseTotarget = baseTotarget.normalize();
		float dotproduct = baseTotarget.dot(axis);
		if (dotproduct >= 1) return;
		float theta = acos(dotproduct);
		r_axis = baseTotarget.cross(axis);
		r_axis = r_axis.normalize();
		while (currentJoint != nullptr) {
			currentJoint->Globalquat = forwardKinematcis->buildQuaternionRotation(-theta / degree2rad, r_axis.x, r_axis.y, r_axis.z);
			forwardKinematcis->forwardKinematicsComputation(currentJoint);
			if (currentJoint == base) break;
			currentJoint = currentJoint->parent;
		}
		return;
	}

	Eigen::MatrixXf J(3, 4), Rx(3, 4);
	Eigen::Vector3f V(
		target->x - endEffector->GlobalPos.x,
		target->y - endEffector->GlobalPos.y,
		target->z - endEffector->GlobalPos.z
	);
	
	int col_idx = 0;

	//fill jacobian matrix
	while (currentJoint != nullptr)
	{
		Eigen::Vector3f toTarget(
			target->x - currentJoint->GlobalPos.x,
			target->y - currentJoint->GlobalPos.y,
			target->z - currentJoint->GlobalPos.z),
			toEnd(
				endEffector->GlobalPos.x - currentJoint->GlobalPos.x,
				endEffector->GlobalPos.y - currentJoint->GlobalPos.y,
				endEffector->GlobalPos.z - currentJoint->GlobalPos.z);
		//toTarget = toTarget.normalized();
		//toEnd = toEnd.normalized();
		Eigen::Vector3f r_axis = toEnd.cross(toTarget);
		r_axis = r_axis.normalized();
		Rx.col(col_idx) = r_axis;
		//J_joint = CrossProduct(rotAxis_joint, endEffectorPos ¡ª jointAPos);
		J.col(col_idx) = r_axis.cross(toEnd);
		col_idx++;
		if (currentJoint == base)
		{
			break;
		}
		currentJoint = currentJoint->parent;
	}
	//Damped Pseudoinverse J+
	//Damping factor
	float lambda = 0.001;
	MatrixXf I(3,3),Jplus;
	I = MatrixXf::Identity(3, 3);
	Jplus = (J.transpose()) * ((J * J.transpose() + pow(lambda, 2) * I).inverse());
	Eigen::Vector4f deltaO;
	deltaO = Jplus * V;

	currentJoint = endEffector->parent;
	col_idx = 0;
	while (currentJoint != nullptr)
	{
		float theta = deltaO[col_idx];
		Eigen::Vector3f r_axis = Rx.col(col_idx);
		Vector4 quaternion = forwardKinematcis->buildQuaternionRotation(theta/degree2rad, r_axis.x(), r_axis.y(), r_axis.z());
		currentJoint->Globalquat = forwardKinematcis->quaternionMultiplication(quaternion, currentJoint->Globalquat);
		col_idx++;
		if (currentJoint == base)
		{
			break;
		}
		currentJoint = currentJoint->parent;
	}
	forwardKinematcis->forwardKinematicsComputation(base);
}
