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


#ifndef MMInput_h
#define MMInput_h


#include "TSInput.h"
#include "MMBase.h"


namespace MMGame
{
	using namespace Tombstone;


	enum : ActionType
	{
		kActionForward				= 'frwd',
		kActionBackward				= 'bkwd',
		kActionLeft					= 'left',
		kActionRight				= 'rght',
		kActionUp					= 'jump',
		kActionDown					= 'down',
		kActionMovement				= 'move',
		kActionHorizontal			= 'horz',
		kActionVertical				= 'vert',
		kActionFirePrimary			= 'fire',
		kActionFireSecondary		= 'trig',
		kActionUse					= 'uobj',
		kActionPistol				= 'pist',
		kActionShotgun				= 'shgn',
		kActionCrossbow				= 'cbow',
		kActionSpikeShooter			= 'spsh',
		kActionGrenadeLauncher		= 'gren',
		kActionQuantumCharger		= 'qchg',
		kActionRocketLauncher		= 'rock',
		kActionPlasmaGun			= 'plas',
		kActionProtonCannon			= 'pcan',
		kActionSpecialWeapon		= 'spec',
		kActionNextWeapon			= 'next',
		kActionPrevWeapon			= 'prev',
		kActionFlashlight			= 'lite',
		kActionCameraView			= 'camr',
		kActionScoreboard			= 'scor',
		kActionTest					= 'mess',
		kActionLoad					= 'load',
		kActionSave					= 'save'
	};


	enum
	{
		kMovementForward			= 1 << 0,
		kMovementBackward			= 1 << 1,
		kMovementLeft				= 1 << 2,
		kMovementRight				= 1 << 3,
		kMovementUp					= 1 << 4,
		kMovementDown				= 1 << 5,
		kMovementPlanarMask			= 15
	};


	class MovementAction : public Action
	{
		private:

			unsigned_int32		movementFlag;
			unsigned_int32		spectatorFlag;

		public:

			MovementAction(ActionType type, unsigned_int32 moveFlag, unsigned_int32 specFlag);
			~MovementAction();

			void HandleEngage(void);
			void HandleDisengage(void);
			void HandleMove(int32 value);
	};


	class LookAction : public Action
	{
		public:

			LookAction(ActionType type);
			~LookAction();

			void HandleUpdate(float value);
	};


	class FireAction : public Action
	{
		public:

			FireAction(ActionType type);
			~FireAction();

			void HandleEngage(void);
			void HandleDisengage(void);

			void Begin(void);
			void End(void);
	};
    
    class TestAction : public Action
    {
    public:
        
        TestAction();
        ~TestAction();
        
        void HandleEngage(void);
        
    };
    
	class BuildAction : public Action
	{
		//private:
		//DefenceBuilder builder;

	public:
		BuildAction(ActionType type);
		~BuildAction();

		void HandleEngage(void);
		void HandleDisengage(void);
	};
    
    
}


#endif

// ZUXSVMT
