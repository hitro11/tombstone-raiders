//=============================================================
//
// Tombstone Engine version 1.0
// Copyright 2016, by Terathon Software LLC
//
// This file is part of the Tombstone Engine and is provided under the
// terms of the license agreement entered by the registed user.
//
// Unauthorized redistribution of source code is strictly
// prohibited. Violators will be prosecuted.
//
//=============================================================


#include "MMCameras.h"
//#include "MMGame.h"
#include "MMFighter.h"


using namespace MMGame;


const float MMGame::kCameraPositionHeight = 1.6F;


ModelCamera::ModelCamera() : FrustumCamera(1.0F, 1.0F)
{
	targetModel = nullptr;
	SetNodeFlags(kNodeCloneInhibit | kNodeAnimateInhibit);
}

ModelCamera::~ModelCamera()
{
}


FirstPersonCamera::FirstPersonCamera()
{
}

FirstPersonCamera::~FirstPersonCamera()
{
}

void FirstPersonCamera::MoveCamera(void)
{
	Model *model = GetTargetModel();
	if (model)
	{
		FighterController *controller = static_cast<FighterController *>(model->GetController());
		Vector2D t = CosSin(controller->GetLookAzimuth());
		Vector2D u = CosSin(controller->GetLookAltitude());

		Vector3D view(t.x * u.x, t.y * u.x, u.y);
		Vector3D right(t.y, -t.x, 0.0F);
		Vector3D down = Cross(view, right);

		const Point3D& position = model->GetWorldPosition();
		Point3D p(position.x, position.y, position.z + kCameraPositionHeight);

		SetNodeTransform(right, down, view, p);

		const Quaternion& q = TheWorldMgr->GetTrackingOrientation();
		SetNodeMatrix3D(GetNodeTransform().GetMatrix3D() * q.GetRotationMatrix());
	}
}


ChaseCamera::ChaseCamera()
{
}

ChaseCamera::~ChaseCamera()
{
}

void ChaseCamera::MoveCamera(void)
{
	Model *model = GetTargetModel();
	if (model)
	{
		CollisionData	data;

		FighterController *controller = static_cast<FighterController *>(model->GetController());
		Vector2D t = CosSin(controller->GetLookAzimuth());
		Vector2D u = CosSin(controller->GetLookAltitude());

		Vector3D view(t.x * u.x, t.y * u.x, u.y);
		Vector3D right(t.y, -t.x, 0.0F);
		Vector3D down = Cross(view, right);

		const Point3D& position = model->GetWorldPosition();
		Point3D p1(position.x, position.y, position.z + 1.5F);
		Point3D p2 = p1 - view * 4.0F;

		if (GetWorld()->DetectCollision(p1, p2, 0.3F, kCollisionCamera, &data))
		{
			float s = data.param;
			p2 = p1 * (1.0F - s) + p2 * s;
		}

		SetNodeTransform(right, down, view, p2);
	}
}

// ZUXSVMT
