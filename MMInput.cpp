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


#include "MMInput.h"
#include "MMGame.h"
#include "MMMultiPlayer.h"

#include "MMFighter.h"
#include "MMGameWorld.h"
#include "SimpleTurret.h"

using namespace MMGame;


MovementAction::MovementAction(ActionType type, unsigned_int32 moveFlag, unsigned_int32 specFlag) : Action(type)
{
	movementFlag = moveFlag;
	spectatorFlag = specFlag;
}

MovementAction::~MovementAction()
{
}

void MovementAction::HandleEngage(void)
{


	const Player *player = TheMessageMgr->GetLocalPlayer();
	if (player)
	{
		const FighterController *controller = static_cast<const GamePlayer *>(player)->GetPlayerController();
		if (controller)
		{
			ClientMovementMessage message(kMessageClientMovementBegin, movementFlag, controller->GetLookAzimuth(), controller->GetLookAltitude());
			TheMessageMgr->SendMessage(kPlayerServer, message);
			return;
		}
	}

	GameWorld *world = static_cast<GameWorld *>(TheWorldMgr->GetWorld());
	if (world)
	{
		SpectatorCamera *camera = world->GetSpectatorCamera();
		camera->SetSpectatorFlags(camera->GetSpectatorFlags() | spectatorFlag);
	}
}

void MovementAction::HandleDisengage(void)
{
	const Player *player = TheMessageMgr->GetLocalPlayer();
	if (player)
	{
		const FighterController *controller = static_cast<const GamePlayer *>(player)->GetPlayerController();
		if (controller)
		{
			ClientMovementMessage message(kMessageClientMovementEnd, movementFlag, controller->GetLookAzimuth(), controller->GetLookAltitude());
			TheMessageMgr->SendMessage(kPlayerServer, message);
			return;
		}
	}

	GameWorld *world = static_cast<GameWorld *>(TheWorldMgr->GetWorld());
	if (world)
	{
		SpectatorCamera *camera = world->GetSpectatorCamera();
		camera->SetSpectatorFlags(camera->GetSpectatorFlags() & ~spectatorFlag);
	}
}

void MovementAction::HandleMove(int32 value)
{
	static const unsigned_int32 movementFlags[8] =
	{
		kMovementForward,
		kMovementForward | kMovementRight,
		kMovementRight,
		kMovementRight | kMovementBackward,
		kMovementBackward,
		kMovementBackward | kMovementLeft,
		kMovementLeft,
		kMovementLeft | kMovementForward
	};

	static const unsigned_int32 spectatorFlags[8] =
	{
		kSpectatorMoveForward,
		kSpectatorMoveForward | kSpectatorMoveRight,
		kSpectatorMoveRight,
		kSpectatorMoveRight | kSpectatorMoveBackward,
		kSpectatorMoveBackward,
		kSpectatorMoveBackward | kSpectatorMoveLeft,
		kSpectatorMoveLeft,
		kSpectatorMoveLeft | kSpectatorMoveForward
	};

	const Player *player = TheMessageMgr->GetLocalPlayer();
	if (player)
	{
		const FighterController *controller = static_cast<const GamePlayer *>(player)->GetPlayerController();
		if (controller)
		{
			unsigned_int32 flags = (value >= 0) ? movementFlags[value] : 0;
			ClientMovementMessage message(kMessageClientMovementChange, flags, controller->GetLookAzimuth(), controller->GetLookAltitude());
			TheMessageMgr->SendMessage(kPlayerServer, message);
			return;
		}
	}

	GameWorld *world = static_cast<GameWorld *>(TheWorldMgr->GetWorld());
	if (world)
	{ 
		unsigned_int32 flags = (value >= 0) ? spectatorFlags[value] : 0; 
		SpectatorCamera *camera = world->GetSpectatorCamera();
		camera->SetSpectatorFlags((camera->GetSpectatorFlags() & ~kSpectatorPlanarMask) | flags); 
	} 
} 

 
LookAction::LookAction(ActionType type) : Action(type) 
{ 
}

LookAction::~LookAction() 
{
}

void LookAction::HandleUpdate(float value)
{
	
}


FireAction::FireAction(ActionType type) : Action(type)
{
}

FireAction::~FireAction()
{
}

void FireAction::HandleEngage(void)
{
	const Player *player = TheMessageMgr->GetLocalPlayer();
	if (player)
	{
		FighterController *controller = static_cast<const GamePlayer *>(player)->GetPlayerController();
		if (controller)
		{
			// The player is controlling a fighter, so either fire the weapon or interact with a panel effect.

			const FighterInteractor *interactor = controller->GetFighterInteractor();
			const Node *interactionNode = interactor->GetInteractionNode();
			if ((!interactionNode) || (interactionNode->GetNodeType() != kNodeEffect))
			{
				// No panel effect, so fire the weapon.

				ClientFiringMessage message((GetActionType() == kActionFirePrimary) ? kMessageClientFiringPrimaryBegin : kMessageClientFiringSecondaryBegin, controller->GetLookAzimuth(), controller->GetLookAltitude());
				TheMessageMgr->SendMessage(kPlayerServer, message);
			}
			else
			{
				// The player's interacting with a panel effect.

				//ClientInteractionMessage message(kMessageClientInteractionBegin, interactor->GetInteractionPosition());
				//TheMessageMgr->SendMessage(kPlayerServer, message);
			}
		}
		else
		{
			// There's no fighter yet, so spawn the player in response to the fire button being pressed.

			if (TheMessageMgr->GetSynchronizedFlag())
			{
				if ((!TheMessageMgr->GetServerFlag()))
				{
					TheMessageMgr->SendMessage(kPlayerServer, ClientMiscMessage(kMessageClientSpawn));
				}
			}
		}
	}
}

void FireAction::HandleDisengage(void)
{
	const Player *player = TheMessageMgr->GetLocalPlayer();
	if (player)
	{
		FighterController *controller = static_cast<const GamePlayer *>(player)->GetPlayerController();
		if (controller)
		{
			// The player is controlling a fighter, so either stop firing the weapon or interact with a panel effect.

			const FighterInteractor *interactor = controller->GetFighterInteractor();
			const Node *interactionNode = interactor->GetInteractionNode();
			if ((!interactionNode) || (interactionNode->GetNodeType() != kNodeEffect))
			{
				// No panel effect, so stop firing the weapon.

				ClientFiringMessage message(kMessageClientFiringEnd, controller->GetLookAzimuth(), controller->GetLookAltitude());
				TheMessageMgr->SendMessage(kPlayerServer, message);
			}
			else
			{
				// The player's interacting with a panel.

				//ClientInteractionMessage message(kMessageClientInteractionEnd, interactor->GetInteractionPosition());
				//TheMessageMgr->SendMessage(kPlayerServer, message);
			}
		}
	}
}

void FireAction::Begin(void) {

}

void FireAction::End(void) {

}

TestAction::TestAction() : Action(kActionTest)
{
}

TestAction::~TestAction()
{
}

void TestAction::HandleEngage(void)
{
    
    printf("T BUTTON PRESSED\n");
    TheDisplayBoard->UpdatePlayerScore();
    

    Player *player = TheMessageMgr->GetLocalPlayer();
    
    //static_cast<GamePlayer *>(player)->Damage(10, 0, nullptr);
    
    FighterController *ctrlr =  static_cast<GamePlayer *>(player)->GetPlayerController();
    
    GamePlayer *gameplayer = ctrlr->GetFighterPlayer();
    
    gameplayer->Damage(10,0,nullptr);


    
//    TheDisplayBoard->UpdateAlert();
    TheDisplayBoard->UpdateAlert(String<>("Countdown"));

    
}

BuildAction::BuildAction(ActionType type) : Action(type)
{
}

BuildAction::~BuildAction()
{
}

void BuildAction::HandleEngage(void)
{
	const Player *player = TheMessageMgr->GetLocalPlayer();
	if (player)
	{
		FighterController *controller = static_cast<const GamePlayer *>(player)->GetPlayerController();
		if (controller)
		{
			// The player is controlling a fighter, so either fire the weapon or interact with a panel effect.

			const FighterInteractor *interactor = controller->GetFighterInteractor();
			const Node *interactionNode = interactor->GetInteractionNode();
			if ((!interactionNode) || (interactionNode->GetNodeType() != kNodeEffect))
			{
				// No panel effect, so build.
				//builder.BuildSelectedDefenceAtPlayerLocation();
				
				
				GameWorld *world = static_cast<GameWorld *>(TheWorldMgr->GetWorld());

				Vector3D direction = controller->GetTargetNode()->GetWorldTransform()[0];
				direction = direction.Normalize();
				float placementHeight = 2.5f;
				
				Point3D playerPos = controller->GetTargetNode()->GetWorldPosition();
				Point3D startPos = Point3D(3 * direction[0], 3 * direction[1], placementHeight);
				startPos = startPos + playerPos;
				
				TurretCreationMessage message(kMessageTurretCreation, startPos, kBaseTurretEntity, kPlayerServer, -1);
				TheMessageMgr->SendMessage(kPlayerServer, message);
			}
			else
			{
				// The player's interacting with a panel effect.

			}
		}
		else
		{
			// There's no fighter yet, so spawn the player in response to the fire button being pressed.

			if (TheMessageMgr->GetSynchronizedFlag())
			{
				if ((!TheMessageMgr->GetServerFlag()))
				{
					TheMessageMgr->SendMessage(kPlayerServer, ClientMiscMessage(kMessageClientSpawn));
				}
			}
		}
	}
}

void BuildAction::HandleDisengage(void)
{
}


// ZUXSVMT
