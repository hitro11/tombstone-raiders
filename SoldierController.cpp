#include "SoldierController.h"
#include "MMInput.h"

#include <stdio.h>
#include <math.h>

using namespace Tombstone;
using namespace MMGame;

//SoldierInteractor

//PRIVATE METHODS


//PUBLIC METHODS

SoldierInteractor::SoldierInteractor(SoldierController *controller)
{
	soldierController = controller;
}

SoldierInteractor::~SoldierInteractor()
{
}

void SoldierInteractor::HandleInteractionEvent(InteractionEventType type, Node *node, const InteractionProperty *property, const Point3D *position)
{
	// Always call the base class counterpart.

	Interactor::HandleInteractionEvent(type, node, property, position);

	// If the node with which we are interacting has a controller,
	// then pass the event through to that controller.

	Controller *controller = node->GetController();
	if (controller)
	{
		controller->HandleInteractionEvent(type, position);
	}
}


//SoldierController

//PRIVATE METHODS

SoldierController::SoldierController(const SoldierController& soldierController) :
	CharacterController(soldierController),
	soldierInteractor(this)
{
	soldierMotion = kMotionNone;
	movementFlags = 0;

	modelAzimuth = 0.0F;
	modelAltitude = 0.0F;
}

Controller *SoldierController::Replicate(void) const
{
	return (new SoldierController(*this));
}

void SoldierController::SetSoldierMotion(int32 motion)
{
	// This function sets the animation resource corresponding to
	// the current type of motion assigned to the soldier.

	Interpolator *interpolator = frameAnimator.GetFrameInterpolator();

	if (motion == kMotionStand)
	{
		frameAnimator.SetAnimation("soldier/Stand");
		interpolator->SetMode(kInterpolatorForward | kInterpolatorLoop);
	}
	else if (motion == kMotionForward)
	{
		frameAnimator.SetAnimation("soldier/Run");
		interpolator->SetMode(kInterpolatorForward | kInterpolatorLoop);
	}
	else if (motion == kMotionBackward)
	{
		frameAnimator.SetAnimation("soldier/Backward");
		interpolator->SetMode(kInterpolatorForward | kInterpolatorLoop);
	}

	soldierMotion = motion;
}


//PUBLIC METHODS

SoldierController::SoldierController(float azimuth) :
	CharacterController(kControllerSoldier),
	soldierInteractor(this)
{
	soldierMotion = kMotionNone;
	movementFlags = 0;

	modelAzimuth = azimuth;
	modelAltitude = 0.0F;
}

SoldierController::~SoldierController()
{
}

void SoldierController::PreprocessController(void)
{
	// This function is called once before the target node is ever
	// rendered or moved. The base class Preprocess() function should
	// always be called first, and then the subclass can do whatever
	// preprocessing it needs to do.

	printf("SOLDIER PROCESS \n");

	CharacterController::PreprocessController();


	SetRigidBodyFlags(kRigidBodyKeepAwake | kRigidBodyFixedOrientation);
	SetFrictionCoefficient(0.0F);

	// We use a frame animator to play animation resources
	// for the soldier model.

	Model *soldier = GetTargetNode();
	frameAnimator.SetTargetModel(soldier);
	soldier->SetRootAnimator(&frameAnimator);

	// Initialize the previous center of mass to the current center of mass
	// so that this doesn't contain garbage the first time be call ActivateTriggers().

	previousCenterOfMass = GetWorldCenterOfMass();

	// Register our interactor with the world.

	soldier->GetWorld()->AddInteractor(&soldierInteractor);
}

void SoldierController::MoveController(void)
{
	// This function is called once per frame to allow the controller to
	// move its target node.

	// The movementIndexTable is a 16-entry table that maps all combinations of
	// the forward, backward, left, and right movement flags to one of eight directions.
	// The direction codes are as follows:
	//
	// 0 - forward
	// 1 - backward
	// 2 - left
	// 3 - right
	// 4 - forward and left
	// 5 - forward and right
	// 6 - backward and left
	// 7 - backward and right
	//
	// The number 8 in the table means no movement, and it appears where either no
	// movement buttons are being pressed or two opposing buttons are the only ones pressed
	// (e.g., left and right pressed simultaneously cancel each other out).

	static const unsigned_int8 movementIndexTable[16] =
	{
		8, 0, 1, 8,
		2, 4, 6, 2,
		3, 5, 7, 3,
		8, 0, 1, 8
	};

	// First, we grab the mouse deltas from the Input Manager.
	// We use these to change the angles representing the direction in
	// which the player is looking/moving.

	float azm = modelAzimuth + TheInputMgr->GetMouseDeltaX();
	if (azm < -Math::tau_over_2)
	{
		azm += Math::tau;
	}
	else if (azm > Math::tau_over_2)
	{
		azm -= Math::tau;
	}

	float alt = Clamp(modelAltitude + TheInputMgr->GetMouseDeltaY(), -1.45F, 1.45F);

	modelAzimuth = azm;
	modelAltitude = alt;

	// Now, we determine whether the player is attempting to move, and
	// we play the appropriate animation on the soldier model.

	int32 motion = kMotionStand;
	Vector2D propel(0.0F, 0.0F);

	int32 index = movementIndexTable[movementFlags & MMGame::kMovementPlanarMask];
	if (index < 8)
	{
		// The movementDirectionTable maps each direction code looked up in the
		// movementIndexTable to an angle measured counterclockwise from the straight
		// ahead direction in units of tau/8.

		static const float movementDirectionTable[8] =
		{
			0.0F, 4.0F, 2.0F, -2.0F, 1.0F, -1.0F, 3.0F, -3.0F
		};

		float direction = movementDirectionTable[index] * Math::tau_over_8 + modelAzimuth;

		// Set the propulsive force based on the direction of movement.

		propel = CosSin(direction) * 100.0F;

		// Determine whether we should play the forward or backward running animation.

		motion = ((index == 1) || (index >= 6)) ? kMotionBackward : kMotionForward;
	}

	// Update the external force for the rigid body representing the character.
	// The GetGroundContact() function is a member of the CharacterController base class.

	if (GetGroundContact())
	{
		SetExternalLinearResistance(Vector2D(10.0F, 10.0F));
		SetExternalForce(propel);
	}
	else
	{
		// If the soldier is not on the ground, reduce the propulsive force down to 2%.
		// This controls how well the player is able to control his movement while
		// falling through the air.

		SetExternalLinearResistance(Zero2D);
		SetExternalForce(propel * 0.02F);
	}

	// Change the soldier's orientation based on horizontal mouse movement.
	// The SetCharacterOrientation() function is a member of the CharacterController base class.

	SetCharacterOrientation(modelAzimuth);

	// If the animation needs to be changed, do it.

	if (motion != soldierMotion)
	{
		SetSoldierMotion(motion);
	}

	// Activate triggers along the line connecting to the current center of mass
	// from the center of mass in the previous frame.

	Model *model = GetTargetNode();
	model->GetWorld()->ActivateTriggers(previousCenterOfMass, GetWorldCenterOfMass(), 0.25F, model);
	previousCenterOfMass = GetWorldCenterOfMass();

	// Call the Model::Animate() function to update the animation playing for the model.

	GetTargetNode()->AnimateModel();
}
