#ifndef __SoldierController_H__
#define __SoldierController_H__


#include "TSWorld.h" 
#include "TSInput.h"
#include "TSCameras.h"
#include "TSApplication.h"
#include "TSInterface.h"
#include "TSCharacter.h"
#include "TSZones.h"
#include "TSEngine.h"


namespace MMGame {
	using namespace Tombstone;
	enum {
		kControllerSoldier = 'sold'
	};

	class SoldierController;


	// The Interactor class is used to track player interactions with objects in the scene.

	class SoldierInteractor : public Interactor
	{
	private:

		SoldierController	*soldierController;

	public:

		SoldierInteractor(SoldierController *controller);
		~SoldierInteractor();

		void HandleInteractionEvent(InteractionEventType type, Node *node, const InteractionProperty *property, const Point3D *position = nullptr) override;
	};

	class SoldierController final : public CharacterController
	{
	private:
		enum
		{
			kMotionNone,
			kMotionStand,
			kMotionForward,
			kMotionBackward
		};

		// The movement flags tell how the user is trying to move the player.
		unsigned_int32		movementFlags;

		// The soldier motion keeps track of what animation is currently playing.
		int32				soldierMotion;

		// The azimuth and altitude represent the direction the player is looking
		// by using the mouse.
		float				modelAzimuth;
		float				modelAltitude;

		// The frame animator controls playback of an animation resource.
		FrameAnimator		frameAnimator;

		// The previous center of mass stores the center point of the character on the
		// previous frame. This is used with the new center point to activate triggers.
		Point3D				previousCenterOfMass;

		// We keep an interactor object here in the controller.
		SoldierInteractor	soldierInteractor;

		SoldierController(const SoldierController& soldierController);

		Controller *Replicate(void) const override;

		void SetSoldierMotion(int32 motion);

	public:

		SoldierController(float azimuth);
		~SoldierController();

		Model *GetTargetNode(void) const
		{
			return (static_cast<Model *>(Controller::GetTargetNode()));
		}

		unsigned_int32 GetMovementFlags(void) const
		{
			return (movementFlags);
		}

		void SetMovementFlags(unsigned_int32 flags)
		{
			movementFlags = flags;
		}

		float GetModelAzimuth(void) const
		{
			return (modelAzimuth);
		}

		float GetModelAltitude(void) const
		{
			return (modelAltitude);
		}

		SoldierInteractor *GetSoldierInteractor(void)
		{
			return (&soldierInteractor);
		}

		void PreprocessController(void);// override;
		void MoveController(void);// override;
	};

}

#endif