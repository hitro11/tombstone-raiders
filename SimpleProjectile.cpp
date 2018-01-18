//=============================================================
//
// C4 Engine version 4.5
// Copyright 1999-2015, by Terathon Software LLC
//
// This file is part of the C4 Engine and is provided under the
// terms of the license agreement entered by the registed user.
//
// Unauthorized redistribution of source code is strictly
// prohibited. Violators will be prosecuted.
//
//=============================================================


#include "SimpleProjectile.h"
#include "MMBase.h"
#include "MMGame.h"


using namespace Tombstone;


// This is the definition of the pointer to the StartWindow class singleton.
// It should be initialized to nullptr, and its value will be set by
// the StartWindow class constructor.


Vector3D ballVelocity = Vector3D(0.0f, 0.0f, 0.0f);//projectile velocity
float speed = 20.0F;//projectile initial speed

BallController::BallController() : Tombstone::RigidBodyController(kControllerBall)
{
	//LB: Added Tombstone:: in front of RigidBodyController
	// This constructor is only called when a new ball model is created.
}

BallController::BallController(const BallController& ballController) : RigidBodyController(ballController)
{
	// This constructor is called when a ball controller is cloned.
}

BallController::~BallController()
{
}

BallController::BallController(Vector3D& velocity) : RigidBodyController(kControllerBall)
{
	ballVelocity = velocity;
	// This constructor is  called when a new ball model is created with an initial velocity
}

Controller *BallController::Replicate(void) const
{
	return (new BallController(*this));
}

bool BallController::ValidNode(const Node *node)
{
	// This function is called by the engine to determine whether
	// this particular type of controller can control the particular
	// node passed in through the node parameter. This function should 
	// return true if it can control the node, and otherwise it should 
	// return false. In this case, the controller can only be applied
	// to model nodes.

	return (node->GetNodeType() == kNodeModel);
}

void BallController::PreprocessController(void)
{
	// This function is called once before the target node is ever
	// rendered or moved. The base class Preprocess() function should
	// always be called first, and then the subclass can do whatever
	// preprocessing it needs to do. In this case, we set a few of the
	// ball's physical parameters and give it a random initial velocity.

	RigidBodyController::PreprocessController();

	SetRestitutionCoefficient(0.99F);
	SetSpinFrictionMultiplier(0.1F);
	SetRollingResistance(0.01F);

	//SetLinearVelocity(Math::RandomUnitVector3D() * 2.0F);
	SetLinearVelocity(ballVelocity);

	SetGravityMultiplier(0.1F);
	/*
	//Code that rotates bullet object to shoot in correct directon
	GameWorld *world = static_cast<GameWorld *>(TheWorldMgr->GetWorld());
	Matrix3D cameraMatrix3D = world->GetCamera()->GetNodeTransform().GetMatrix3D();
	Model *model = static_cast<Model *>(Controller::GetTargetNode());
	SetRigidBodyTransform(Transform4D(cameraMatrix3D, model->GetNodePosition()));
	model->Invalidate();
	SetLinearVelocity(ballVelocity / 10.0F);
	*/
}

RigidBodyStatus BallController::HandleNewRigidBodyContact(const RigidBodyContact *contact, RigidBodyController *contactBody)
{
	// This function is called when the ball makes contact with another rigid body.

	if (contactBody->GetControllerType() == kControllerBall)
	{
		// Add a sound effect and some sparks to the world.

		Node *node = GetTargetNode();
		World *world = node->GetWorld();
		Point3D position = node->GetWorldTransform() * contact->GetContactPosition();

		OmniSource *source = new OmniSource("model/Ball", 40.0F);
		source->SetNodePosition(position);
		world->AddNewNode(source);

		//SparkParticleSystem *sparks = new SparkParticleSystem(20);

		//LB: commented out sparks effects due to error, can add back in later
		//sparks->SetNodePosition(position);
		//world->AddNewNode(sparks);
	}

	return (kRigidBodyUnchanged);
}

RigidBodyStatus BallController::HandleNewGeometryContact(const GeometryContact *contact)
{
	// This function is called when the ball makes contact with any geometry.

	// Add a sound effect and some sparks to the zone containing the ball.
	// Transform the contact point into the zone's local coordinate system.

	Node *node = GetTargetNode();
	Zone *zone = node->GetOwningZone();
	Point3D zonePosition = node->GetWorldPosition();//();

	OmniSource *source = new OmniSource("model/Ball", 50.0F);
	source->SetNodePosition(zonePosition);
	//zone->AddNewSubnode(source);
	World *world = node->GetWorld();
	world->AddNewNode(source);

	/*
	SparkSystem *sparks = new SparkSystem(20);
	sparks->SetNodePosition(zonePosition);
	zone->AddNewSubnode(sparks);
	*/

	//SparkParticleSystem *sparks = new SparkParticleSystem(20);

	//LB: commented out sparks effects due to error, can add back in later
	//sparks->SetNodePosition(zonePosition);
	//world->AddNewNode(sparks);

	delete node;
	return (kRigidBodyDestroyed);
}


SparksParticleSystem::SparksParticleSystem() :
	LineParticleSystem(kParticleSystemSpark, &particlePool, "particle/Spark1"),
	particlePool(kMaxParticleCount, particleArray)
{
	particleLifeTime = 0;
}

SparksParticleSystem::SparksParticleSystem(int32 count, float speed) :
	LineParticleSystem(kParticleSystemSpark, &particlePool, "particle/Spark1"),
	particlePool(kMaxParticleCount, particleArray)
{
	particleLifeTime = 0;
	particleCount = Min(count, kMaxParticleCount);
	particleSpeed = speed;
	if (lengthMultiplier > 0) {
		SetLengthMultiplier(lengthMultiplier);
	}
	else {
		SetLengthMultiplier(1.0F);
	}
}

SparksParticleSystem::~SparksParticleSystem()
{
}

bool SparksParticleSystem::CalculateBoundingSphere(BoundingSphere *sphere) const
{
	sphere->SetCenter(0.0F, 0.0F, 0.0F);
	if (sphereRadius > 0) {
		sphere->SetRadius(sphereRadius);
	}
	else{
		sphere->SetRadius(20.0F);
	}
	return (true);
}

void SparksParticleSystem::PreprocessNode(void)
{
	LineParticleSystem::PreprocessNode();
	SetParticleSystemFlags(kParticleSystemSelfDestruct);

	if (!GetFirstParticle())
	{
		Point3D center = GetSuperNode()->GetWorldTransform() * GetNodePosition();
		const ConstVector2D *trig = Math::GetTrigTable();

		int32 count = particleCount;
		for (machine a = 0; a < count; a++)
		{
			Particle *particle = particlePool.NewParticle();

			particle->emitTime = 0;
			if (particleLifeTime > 0) {
				particle->lifeTime = particleLifeTime;
			}
			else {
				particle->lifeTime = 200;
			}
			
			if (particleRadius > 0) {
				particle->radius = particleRadius;
			}
			else {
				particle->radius = 0.1F;
			}

			if (particleColor.x > -1) {
				particle->color.Set(particleColor.x, particleColor.y, particleColor.z, 1.0f);
			}
			else {
				float brightness = Math::RandomFloat(0.875F) + 0.125F;
				particle->color.Set(brightness, brightness, brightness * Math::RandomFloat(0.5F) + 0.1F, 1.0F);
			}
			
			particle->orientation = 0;
			particle->position = center;

			int32 phi = Math::RandomInteger(128);
			int32 theta = Math::RandomInteger(256);
			float speed = (Math::RandomFloat(0.01F) + 0.01F) * particleSpeed;
			Vector2D csp = trig[phi] * speed;
			const Vector2D& cst = trig[theta];
			particle->velocity.Set(cst.x * csp.y, cst.y * csp.y, csp.x);

			AddParticle(particle);
		}
	}
}

void SparksParticleSystem::AnimateParticles(void)
{
	int32 dt = TheTimeMgr->GetDeltaTime();
	float fdt = TheTimeMgr->GetFloatDeltaTime();

	Particle *particle = GetFirstParticle();
	while (particle)
	{
		Particle *next = particle->nextParticle;

		int32 life = (particle->lifeTime -= dt);
		if (life > 0)
		{
			if (life < 200)
			{
				particle->color.alpha = float(life) * 0.005F;
			}

			particle->velocity.z += Math::gravity * fdt;
			particle->position += particle->velocity * fdt;
		}
		else
		{
			FreeParticle(particle);
		}

		particle = next;
	}
}

void SparksParticleSystem::SetLengthMultiplier(float newLength) {
	lengthMultiplier = newLength;
}

void SparksParticleSystem::SetSphereRadius(float newSphereRadius) {
	sphereRadius = newSphereRadius;
}

void SparksParticleSystem::SetParticleRadius(float newParticleRadius) {
	particleRadius = newParticleRadius;
}

void SparksParticleSystem::SetColor(Vector3D RGBValues) {
	particleColor = RGBValues;
}

void SparksParticleSystem::SetLifeTime(int32 newLifeTime) {
	particleLifeTime = newLifeTime;
}


// ZYVRPLN
