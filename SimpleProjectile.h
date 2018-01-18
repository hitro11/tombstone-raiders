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


#ifndef SimpleProjectile_h
#define SimpleProjectile_h


#include "TSWorld.h"
#include "TSInput.h"
#include "TSCameras.h"
#include "TSApplication.h"
#include "TSInterface.h"
#include "TSParticles.h"
#include "TSZones.h"



//namespace MMGame
namespace Tombstone
{

	// Model types are associated with a model resource using the ModelRegistration
	// class. Models are registered with the engine in the Game constructor.

	enum : ModelType
	{
		kModelBall = 'ball'
	};


	// New controller types are registered with the engine in the Game constructor.

	enum : ControllerType
	{
		kControllerBall = 'ball'
	};


	// This is the type of our custom particle system.
	
	enum : ParticleSystemType
	{
		kParticleSystemSpark = 'sprk'
	};
	

	// New locator types are registered with the engine in the Game constructor.
	// The 'spec' locator is used to specify where the spectator camera should
	// be positioned when a world is loaded.

	enum : LocatorType
	{
		kLocatorSpectator = 'spec'
	};

	// Controllers are used to control anything that moves in the world.
	// New types of controllers defined by the application/game module are
	// registered with the engine when the Game class is constructed.
	//
	// The BallController inherits from the built-in rigid body controller,
	// which handles the ball's motion and collision detection. We are only
	// adding a little bit of functionality that causes a particle system 
	// to be created when a ball hits another ball. 

	class BallController final : public RigidBodyController
	{
	private:

		BallController(const BallController& ballController);

		Controller *Replicate(void) const override;

	public:

		BallController();
		~BallController();
		BallController(Vector3D& velocity);//new constructor
		static bool ValidNode(const Node *node);

		void PreprocessController(void) override;

		RigidBodyStatus HandleNewRigidBodyContact(const RigidBodyContact *contact, RigidBodyController *contactBody);
		RigidBodyStatus HandleNewGeometryContact(const GeometryContact *contact);//declare Handler for Geometry Contact
	};


	// The SparkParticleSystem class implements a simple particle system that
	// creates a small burst of sparks.
	class SparksParticleSystem : public LineParticleSystem
	{
		friend class ParticleSystemReg<SparksParticleSystem>;

	private:

		enum
		{
			kMaxParticleCount = 64
		};

		int32				particleCount;
		float				particleSpeed;
		float				lengthMultiplier;
		float				sphereRadius;
		float				particleRadius;
		Vector3D			particleColor;
		int32				particleLifeTime;

		ParticlePool<>		particlePool;
		Particle			particleArray[kMaxParticleCount];

		SparksParticleSystem();

		bool CalculateBoundingSphere(BoundingSphere *sphere) const override;

	public:

		SparksParticleSystem(int32 count, float speed = 1.0F);
		~SparksParticleSystem();

		void PreprocessNode(void) override;
		void AnimateParticles(void) override;

		void SetLengthMultiplier(float newLength);
		void SetSphereRadius(float newSphereRadius);
		void SetParticleRadius(float newParticleRadius);
		void SetColor(Vector3D RGBValues);
		void SetLifeTime(int32 newLifeTime);
	};
}


#endif

// ZYVRPLN
