Vector4 ForwardKinematic::computeLocalQuaternion(Joint* joint)
{
	Vector4 q = Vector4(0.0f, 0.0f, 0.0f, 0.0f);
	Vector4 qx, qy, qz;
	float cx, sx, cy, sy, cz, sz;
	float to_rand = 3.14f / 180.f;
	float x,y,z;
	switch (joint->rotationOrder)
	{
	//enum { NONE = 0, ZYX = 1, YZX = 2, ZXY = 3, XZY = 5, YXZ = 6, XYZ = 7 };
	case 1: 
	{
		z = pFrame[0];
		y = pFrame[1];
		x = pFrame[2];
		pFrame += 3;
		break;
	}
	case 2:
	{
		y = pFrame[0];
		z = pFrame[1];
		x = pFrame[2];
		pFrame += 3;
		break;
	}
	case 3:
	{
		z = pFrame[0];
		x = pFrame[1];
		y = pFrame[2];
		pFrame += 3;
		break;
	}
	case 5:
	{
		x = pFrame[0];
		z = pFrame[1];
		y = pFrame[2];
		pFrame += 3;
		break;
	}
	case 6:
	{
		y = pFrame[0];
		x = pFrame[1];
		z = pFrame[2];
		pFrame += 3;
		break;
	}
	case 7:
	{
		x = pFrame[0];
		y = pFrame[1];
		z = pFrame[2];
		pFrame += 3;
		break;
	}
	default:
		return q;
	}
	
	//caclualate quaternion;
	x *= to_rand;
	y *= to_rand;
	z *= to_rand;
	cx = cos(x/2);
	cy = cos(y/2);
	cz = cos(z/2);
	sx = sin(x/2);
	sy = sin(y/2);
	sz = sin(z/2);

	qx = Vector4(cx,sx,sx,sx);
	qy = Vector4(cy, sy, sy, sy);
	qz = Vector4(cz, sz, sz, sz);

	switch (joint->rotationOrder)
	{
		//enum { NONE = 0, ZYX = 1, YZX = 2, ZXY = 3, XZY = 5, YXZ = 6, XYZ = 7 };
	case 1:
	{
		Vector4 temp = quaternionMultiplication(qz, qy);
		q = quaternionMultiplication(temp, qx);
		break;
	}
	case 2:
	{
		Vector4 temp = quaternionMultiplication(qy, qz);
		q = quaternionMultiplication(temp, qx);
		break;
	}
	case 3:
	{
		Vector4 temp = quaternionMultiplication(qz, qx);
		q = quaternionMultiplication(temp, qy);
		break;
	}
	case 5:
	{
		Vector4 temp = quaternionMultiplication(qx, qz);
		q = quaternionMultiplication(temp, qy);
		break;
	}
	case 6:
	{
		Vector4 temp = quaternionMultiplication(qy, qx);
		q = quaternionMultiplication(temp, qz);
		break;
	}
	case 7:
	{
		Vector4 temp = quaternionMultiplication(qx, qy);
		q = quaternionMultiplication(temp, qz);
		break;
	}
	default:
		return q;
	}
	return q;
}
Vector4 q(0.0f, 0.0f, 0.0f, 0.0f);
	q = quaternionMultiplication(joint->parent->Globalquat, localQuat);
	return q;
Vector4 pos(0.0f, 0.0f, 0.0f, 0.0f);
	pos = joint->parent->GlobalPos + joint->Globalquat * joint->LocalPos;

	return pos;
