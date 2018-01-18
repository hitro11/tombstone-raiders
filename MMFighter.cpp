// MODIFIED my MvM


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


#include "MMFighter.h"
#include "MMGame.h"
#include "MMCameras.h"
#include <stdio.h>


using namespace MMGame;


namespace
{
	const float kFighterRunForce = 70.0F;
	const float kFighterResistForce = 10.0F;
}


FighterInteractor::FighterInteractor(FighterController *controller)
{
	fighterController = controller;
}

FighterInteractor::~FighterInteractor()
{
}

void FighterInteractor::HandleInteractionEvent(InteractionEventType type, Node *node, const InteractionProperty *property, const Point3D *position)
{
	
	
}


FighterController::FighterController(ControllerType type) :
		GameCharacterController(kCharacterPlayer, type),
		fighterInteractor(this),
		worldUpdateObserver(this, &FighterController::HandleWorldUpdate),
		frameAnimatorObserver(this, &FighterController::HandleAnimationEvent)
{
	weaponModel = nullptr;
	weaponMountMarker = nullptr;

	iconIndex = -1;
	iconEffect = nullptr;
	mountNode = nullptr;
	flashlight = nullptr;
	rootAnimator = nullptr;

	fighterFlags = 0;

	primaryAzimuth = 0.0F;
	lookAzimuth = 0.0F;
	lookAltitude = 0.0F;

	deltaLookAzimuth = 0.0F;
	deltaLookAltitude = 0.0F;
	lookInterpolateParam = 0.0F;

	movementFlags = 0;
	fighterMotion = kFighterMotionNone;
	motionComplete = false; 
	targetDistance = 0.0F; 
	damageTime = 0;
 
	//SetCollisionExclusionMask(kCollisionCorpse);
} 

FighterController::~FighterController() 
{ 
	delete rootAnimator; 
	delete mountNode;
}
 
void FighterController::Pack(Packer& data, unsigned_int32 packFlags) const
{
	GameCharacterController::Pack(data, packFlags);

	data << ChunkHeader('FLAG', 4);
	unsigned_int32 flags = fighterFlags & ~kFighterFiring;
	data << flags;

	data << ChunkHeader('ORNT', 12);
	data << primaryAzimuth;
	data << lookAzimuth;
	data << lookAltitude;

	data << ChunkHeader('DGTM', 4);
	data << damageTime;

	data << TerminatorChunk;
}

void FighterController::Unpack(Unpacker& data, unsigned_int32 unpackFlags)
{
	GameCharacterController::Unpack(data, unpackFlags);
	UnpackChunkList<FighterController>(data, unpackFlags);
}

void FighterController::UnpackChunk(const ChunkHeader *chunkHeader, Unpacker& data, unsigned_int32 unpackFlags)
{
	switch (chunkHeader->chunkType)
	{
		case 'FLAG':

			data >> fighterFlags;
			break;

		case 'ORNT':

			data >> primaryAzimuth;
			data >> lookAzimuth;
			data >> lookAltitude;
			break;

		case 'DGTM':

			data >> damageTime;
			break;
	}
}

/*
GamePlayer *FighterController::GetFighterPlayer(void) const
{
    return (static_cast<GamePlayer *>(fighterPlayer.GetTarget()));
}
*/


void FighterController::PreprocessController(void)
{
	GameCharacterController::PreprocessController();

    printf("Preprocess \n");
	SetFrictionCoefficient(0.001F);
	//SetCollisionKind(GetCollisionKind() | kCollisionPlayer);

	modelAzimuth = primaryAzimuth;

	mountNode = new Node;
	mountNode->SetNodeFlags(kNodeNonpersistent | kNodeCloneInhibit | kNodeAnimateInhibit);

	Model *model = GetTargetNode();
	model->AppendSubnode(mountNode);
    
    
    

	rootAnimator = new MergeAnimator(model);
	mergeAnimator = new MergeAnimator(model);
	blendAnimator = new BlendAnimator(model);
	frameAnimator[0] = new FrameAnimator(model);
	frameAnimator[1] = new FrameAnimator(model);
	frameAnimator[1]->GetFrameInterpolator()->SetCompletionCallback(&HandleMotionCompletion, this);

    
    // I have to do these here, it seems the coresponding ones of character
    // are not called, I should call the super class, will fix this ......
    
	mergeAnimator->AppendSubnode(blendAnimator);
	blendAnimator->AppendSubnode(frameAnimator[0]);
	blendAnimator->AppendSubnode(frameAnimator[1]);
  
    
    model->SetRootAnimator((frameAnimator[0]));
    
    blendAnimator->SetTargetModel(model);
    frameAnimator[0]->SetTargetModel(model);
    frameAnimator[1]->SetTargetModel(model);
    
    blendAnimator->AppendSubnode(frameAnimator[0]);
    blendAnimator->AppendSubnode(frameAnimator[1]);
    model->SetRootAnimator(blendAnimator);

    

	SetFrameAnimatorObserver(&frameAnimatorObserver);

	World *world = model->GetWorld();
	world->AddUpdateObserver(&worldUpdateObserver);

	if (TheMessageMgr->GetServerFlag())
	{
		world->AddInteractor(&fighterInteractor);
	}

	previousCenterOfMass = GetWorldCenterOfMass();
	weaponSwitchTime = TheTimeMgr->GetSystemAbsoluteTime();
    
    
    
    Animator *animator = GetMergeAnimator();
    
    spineTwistAnimator = new SpineTwistAnimator(model, model->FindNode(Text::StaticHash<'B', 'i', 'p', '0', '1', '_', 'S', 'p', 'i', 'n', 'e'>::value));
    animator->AppendNewSubnode(spineTwistAnimator);
    
    FrameAnimator *frameA = GetFrameAnimator(0);
    frameA->SetAnimation("soldier/Stand");
    
    frameA = GetFrameAnimator(1);
    frameA->SetAnimation("soldier/Stand");

	//Find the Barrel Locator Marker from which the bullets will be shot
	//start at the root although you could also start
	//at the target node with GetTargetNode()
	Node *root = TheWorldMgr->GetWorld()->GetRootNode();
	Node *thisnode = root;
	String<30> nodeName = "NoName";
	bool found = false;
	do
		{
			nodeName = "NoName";
			if (thisnode->GetNodeName() != NULL)
				nodeName = thisnode->GetNodeName();
			if ((Text::CompareText(nodeName, "Barrel"))) {
				BarrelNode = thisnode;
				found = true;
			}
			//thisnode = root->GetNextNode(thisnode);
			thisnode = root->GetNextTreeNode(thisnode);
		} while (thisnode && !found);

}

void FighterController::SetMountNodeTransform(void)
{

}

void FighterController::HandleWorldUpdate(WorldObservable *observable)
{
 
}

void FighterController::MoveController(void)
{
	GameCharacterController::MoveController();
    

	bool server = TheMessageMgr->GetServerFlag();
	//if (!(fighterFlags & kFighterDead))
	{
		static const unsigned_int8 movementIndexTable[16] =
		{
			8, 0, 1, 8,
			2, 4, 6, 2,
			3, 5, 7, 3,
			8, 0, 1, 8
		};

		damageTime = MaxZero(damageTime - TheTimeMgr->GetDeltaTime());

		if (fighterPlayer == TheMessageMgr->GetLocalPlayer()){
            			//float dt = TheTimeMgr->GetFloatDeltaTime();

            float azm = lookAzimuth + TheInputMgr->GetMouseDeltaX() ;//+ TheGame->GetLookSpeedX() * dt;
			if (azm < -Math::tau_over_2)
			{
				azm += Math::tau;
			}
			else if (azm > Math::tau_over_2)
			{
				azm -= Math::tau;
			}

            float alt = lookAltitude + TheInputMgr->GetMouseDeltaY();// + TheGame->GetLookSpeedY() * dt;
			alt = Clamp(alt, -1.5F, 1.5F);

			if ((azm != lookAzimuth) || (alt != lookAltitude))
			{
				lookAzimuth = azm;
				lookAltitude = alt;

				if (!server)
				{
					TheMessageMgr->SendMessage(kPlayerServer, ClientOrientationMessage(azm, alt));
				}
			}

			TheSoundMgr->SetListenerVelocity(GetLinearVelocity());
		}

		int32 motion = fighterMotion;
		Vector2D force(0.0F, 0.0F);
		float azimuthOffset = 0.0F;

		int32 index = movementIndexTable[movementFlags & kMovementPlanarMask];
		if (index < 8)
		{
			static const float movementDirectionTable[8] =
			{
				0.0F, 4.0F, 2.0F, -2.0F, 1.0F, -1.0F, 3.0F, -3.0F
			};

			static const float movementAzimuthTable[8] =
			{
				0.0F, 0.0F, 2.0F, -2.0F, 1.0F, -1.0F, -1.0F, 1.0F
			};

			float direction = movementDirectionTable[index] * Math::tau_over_8 + lookAzimuth;
			force += CosSin(direction) * kFighterRunForce;

			primaryAzimuth = lookAzimuth;
			azimuthOffset = movementAzimuthTable[index];
			motion = ((index == 1) || (index >= 6)) ? kFighterMotionBackward : kFighterMotionForward;
		}
		else if (motion <= kFighterMotionBackward)
		{
			motion = kFighterMotionStop;
		}

		if (GetCharacterState() & kCharacterGround)
		{
			SetExternalLinearResistance(Vector2D(kFighterResistForce, kFighterResistForce));
			SetExternalForce(force);
		}
		else
		{
			SetExternalLinearResistance(Zero2D);
			SetExternalForce(force * 0.02F);
		}

		lookInterpolateParam = FmaxZero(lookInterpolateParam - TheTimeMgr->GetSystemFloatDeltaTime() * TheMessageMgr->GetSnapshotFrequency());

		float azm = primaryAzimuth;
		if ((motion <= kFighterMotionStand) || (motion == kFighterMotionTurnLeft) || (motion == kFighterMotionTurnRight))
		{
			float interpolatedAzimuth = GetInterpolatedLookAzimuth();

			float da = interpolatedAzimuth - azm;
			if (da > Math::tau_over_2)
			{
				da -= Math::tau;
			}
			else if (da < -Math::tau_over_2)
			{
				da += Math::tau;
			}

			if (da > Math::tau_over_8)
			{
				if (da > Math::tau_over_4)
				{
					azm = interpolatedAzimuth - Math::tau_over_4;
				}

				motion = kFighterMotionTurnLeft;
			}
			else if (da < -Math::tau_over_8)
			{
				if (da < -Math::tau_over_4)
				{
					azm = interpolatedAzimuth + Math::tau_over_4;
				}

				motion = kFighterMotionTurnRight;
			}
		}

		if (motionComplete)
		{
			if (fighterMotion == kFighterMotionTurnLeft)
			{
				azm += Math::tau_over_4;
				if (azm > Math::tau_over_2)
				{
					azm -= Math::tau;
				}
			}
			else if (fighterMotion == kFighterMotionTurnRight)
			{
				azm -= Math::tau_over_4;
				if (azm < -Math::tau_over_2)
				{
					azm += Math::tau;
				}
			}

			motion = kFighterMotionStand;
			motionComplete = false;
		}

		if (motion != fighterMotion)
		{
            //printf("CHANGE  MOTION \n");
			SetFighterMotion(motion);
		}

		primaryAzimuth = azm;
		modelAzimuth = azimuthOffset * Math::tau_over_16 + azm;

		SetCharacterOrientation(modelAzimuth);
		SetMountNodeTransform();

		if (server){
			Model *model = GetTargetNode();
			model->GetWorld()->ActivateTriggers(previousCenterOfMass, GetWorldCenterOfMass(), 3.F, model); //changed from 0.33F to 3.F
		}

		previousCenterOfMass = GetWorldCenterOfMass();
	}
    /*
	else
	{
		if ((server) && ((deathTime -= TheTimeMgr->GetDeltaTime()) < 0))
		{
			TheMessageMgr->SendMessageAll(DeleteNodeMessage(GetControllerIndex()));
			return;
		}
     

		SetExternalLinearResistance(Vector2D(kFighterResistForce, kFighterResistForce));
	}
   */

	AnimateFighter();
}

ControllerMessage *FighterController::CreateMessage(ControllerMessageType type) const
{
	switch (type)
	{
				case kFighterMessageBeginMovement:
		case kFighterMessageEndMovement:
		case kFighterMessageChangeMovement:

			return (new FighterMovementMessage(type, GetControllerIndex()));

			case kFighterMessageDeath:

			return (new ControllerMessage(kFighterMessageDeath, GetControllerIndex()));
	}

	return (GameCharacterController::CreateMessage(type));
}

void FighterController::ReceiveMessage(const ControllerMessage *message)
{
	switch (message->GetControllerMessageType())
	{
		case kFighterMessageBeginMovement:
		{
            
			const FighterMovementMessage *m = static_cast<const FighterMovementMessage *>(message);

			unsigned_int32 flag = m->GetMovementFlag();
			if (flag == kMovementUp)
			{
				if ((!(GetCharacterState() & kCharacterJumping)) && (GetOffGroundTime() < 250.0F))
				{
					float impulse = FmaxZero(2.5F * GetInverseBodyMass() - GetLinearVelocity().z) * GetBodyMass();
					if (impulse > 0.0F)
					{
						ApplyImpulse(Vector3D(0.0F, 0.0F, impulse));
						SetCharacterState(kCharacterJumping);
					}

					PurgeContacts();
				}
			}

			if (fighterPlayer != TheMessageMgr->GetLocalPlayer())
			{
				SetOrientation(m->GetMovementAzimuth(), m->GetMovementAltitude());
			}

			movementFlags |= flag;
			break;
		}

		case kFighterMessageEndMovement:
		{
			const FighterMovementMessage *m = static_cast<const FighterMovementMessage *>(message);

			if (fighterPlayer != TheMessageMgr->GetLocalPlayer())
			{
				SetOrientation(m->GetMovementAzimuth(), m->GetMovementAltitude());
			}

			movementFlags &= ~m->GetMovementFlag();
			break;
		}

		case kFighterMessageChangeMovement:
		{
			const FighterMovementMessage *m = static_cast<const FighterMovementMessage *>(message);

			if (fighterPlayer != TheMessageMgr->GetLocalPlayer())
			{
				SetOrientation(m->GetMovementAzimuth(), m->GetMovementAltitude());
			}

			movementFlags = (movementFlags & ~kMovementPlanarMask) | m->GetMovementFlag();
			break;
		}

		

		case kFighterMessageUpdate:
		{
			const FighterUpdateMessage *m = static_cast<const FighterUpdateMessage *>(message);

			if (fighterPlayer != TheMessageMgr->GetLocalPlayer())
			{
				UpdateOrientation(m->GetUpdateAzimuth(), m->GetUpdateAltitude());
			}

			break;
		}

		case kFighterMessageWeapon:
		{
			const FighterWeaponMessage *m = static_cast<const FighterWeaponMessage *>(message);
			SetWeapon(m->GetWeaponIndex(), m->GetWeaponControllerIndex());

			if (fighterPlayer == TheMessageMgr->GetLocalPlayer())
			{
				//TheDisplayBoard->UpdatePlayerWeapons();
			}

			break;
		}

		

		case kFighterMessageDeath:
		{
			fighterInteractor.CancelInteraction();
			GetTargetNode()->GetWorld()->RemoveInteractor(&fighterInteractor);

/*
			const Model *model = GetTargetNode();
			OmniSource *source = new OmniSource("gus/Death", 32.0F);
			source->SetSourcePriority(kSoundPriorityDeath);
			source->SetNodePosition(model->GetWorldPosition());
			model->GetWorld()->AddNewNode(source);

			if (fighterPlayer == TheMessageMgr->GetLocalPlayer())
			{
				static_cast<GameWorld *>(model->GetWorld())->SetBloodIntensity(1.0F);
			}

			SetExternalForce(Zero3D);
			SetCollisionKind(GetCollisionKind() | kCollisionCorpse);
			SetCollisionExclusionMask(~kCollisionRigidBody);
*/
			deathTime = 10000;
			fighterFlags |= kFighterDead;
			SetFighterMotion(kFighterMotionDeath);

			SetPerspectiveExclusionMask(0);
			break;
		}

		case kFighterMessageDamage:
		{
			

			break;
		}

		default:

			GameCharacterController::ReceiveMessage(message);
			break;
	}
}

void FighterController::SendInitialStateMessages(Player *player) const
{
    printf("FIGHTER Send Initial State Message CALLED \n");
    
    
    const Point3D& pos = GetTargetNode()->GetWorldPosition();
    CreateCharacterMessage message(kMessageCreateCharacter,GetControllerIndex(),kSoldierEntity,playerkey,pos,-1);
    player->SendMessage(message);
    
     
}

void FighterController::SendSnapshot(void)
{
	if (TheMessageMgr->GetServerFlag()) { //Gabe changed
		GameCharacterController::SendSnapshot();
	}
	TheMessageMgr->SendMessageClients(FighterUpdateMessage(GetControllerIndex(), lookAzimuth, lookAltitude), 0);
}

RigidBodyStatus FighterController::HandleNewGeometryContact(const GeometryContact *contact)
{
	

	return (kRigidBodyUnchanged);
}

void FighterController::EnterWorld(World *world, const Point3D& worldPosition)
{
    /*
	if (TheMessageMgr->GetMultiplayerFlag())
	{
		Point3D position(worldPosition.x, worldPosition.y, worldPosition.z + 1.0F);

	}
     */
}

CharacterStatus FighterController::Damage(Fixed damage, unsigned_int32 flags, GameCharacterController *attacker, const Point3D *position, const Vector3D *force)
{
		return (kCharacterUnaffected);
}

void FighterController::Kill(GameCharacterController *attacker, const Point3D *position, const Vector3D *force)
{
	//GetFighterPlayer()->Kill(attacker);
}

void FighterController::UpdateOrientation(float azm, float alt)
{
	deltaLookAzimuth = GetInterpolatedLookAzimuth() - azm;
	if (deltaLookAzimuth > Math::tau_over_2)
	{
		deltaLookAzimuth -= Math::tau;
	}
	else if (deltaLookAzimuth < -Math::tau_over_2)
	{
		deltaLookAzimuth += Math::tau;
	}

	deltaLookAltitude = GetInterpolatedLookAltitude() - alt;

	lookAzimuth = azm;
	lookAltitude = alt;
	lookInterpolateParam = 1.0F;
}

void FighterController::BeginMovement(unsigned_int32 flag, float azm, float alt)
{
	const Point3D& position = GetTargetNode()->GetWorldPosition();
	Vector3D velocity = GetLinearVelocity();
	FighterMovementMessage message(kFighterMessageBeginMovement, GetControllerIndex(), position, velocity, azm, alt, flag);
	TheMessageMgr->SendMessageAll(message);
}

void FighterController::EndMovement(unsigned_int32 flag, float azm, float alt)
{
	const Point3D& position = GetTargetNode()->GetWorldPosition();
	Vector3D velocity = GetLinearVelocity();

	FighterMovementMessage message(kFighterMessageEndMovement, GetControllerIndex(), position, velocity, azm, alt, flag);
	TheMessageMgr->SendMessageAll(message);
}

void FighterController::ChangeMovement(unsigned_int32 flags, float azm, float alt)
{
	const Point3D& position = GetTargetNode()->GetWorldPosition();
	Vector3D velocity = GetLinearVelocity();

	FighterMovementMessage message(kFighterMessageChangeMovement, GetControllerIndex(), position, velocity, azm, alt, flags);
	TheMessageMgr->SendMessageAll(message);
}

void FighterController::BeginFiring(bool primary, float azm, float alt)
{
	SetOrientation(azm, alt);

	unsigned_int32 flags = fighterFlags & ~kFighterFiring;
	fighterFlags = flags | ((primary) ? kFighterFiringPrimary : kFighterFiringSecondary);

	//GetWeaponController()->BeginFiring(primary);
}

void FighterController::EndFiring(float azm, float alt)
{
	SetOrientation(azm, alt);
	fighterFlags &= ~kFighterFiring;

	//GetWeaponController()->EndFiring();
}

void FighterController::SetWeapon(int32 weaponIndex, int32 weaponControllerIndex)
{
	//GamePlayer *player = GetFighterPlayer();

}

void FighterController::PlayInventorySound(int32 inventoryIndex)
{
	
}

void FighterController::ActivateFlashlight(void)
{
	
}

void FighterController::DeactivateFlashlight(void)
{
	
}

void FighterController::ToggleFlashlight(void)
{
	
}

void FighterController::SetPerspectiveExclusionMask(unsigned_int32 mask) const
{
	Node *node = GetTargetNode()->GetFirstSubnode();
	while (node)
	{
		if (node != mountNode)
		{
			Node *subnode = node;
			do
			{
				if (subnode->GetNodeType() == kNodeGeometry)
				{
					Geometry *geometry = static_cast<Geometry *>(subnode);
					geometry->SetPerspectiveExclusionMask(mask);

					if (mask == 0)
					{
						geometry->SetMinDetailLevel(0);
					}
					else
					{
						geometry->SetMinDetailLevel(geometry->GetObject()->GetGeometryLevelCount() - 1);
					}
				}

				subnode = node->GetNextTreeNode(subnode);
			} while (subnode);
		}

		node = node->GetNextSubnode();
	}
}

void FighterController::SetFighterStyle(const int32 *style, bool prep)
{
}

/*
void FighterController::SetFighterMotion(int32 motion)
{
	fighterMotion = motion;
	motionComplete = false;
}
 */

void FighterController::HandleAnimationEvent(FrameAnimator *animator, CueType cueType)
{
}

void FighterController::HandleMotionCompletion(Interpolator *interpolator, void *cookie)
{
	static_cast<FighterController *>(cookie)->motionComplete = true;
}

/*
void FighterController::AnimateFighter(void)
{
	GetTargetNode()->AnimateModel();
}
*/



void FighterController::fireLaser(void)
{
	
	CollisionData   collisionData;
	const Point3D& position = GetTargetNode()->GetWorldPosition();
    Vector2D t = CosSin(lookAzimuth);
    Vector2D u = CosSin(lookAltitude);
#define FIRE_RANGE 100.
    Vector3D shotDirection(t.x * u.x,  t.y * u.x,  u.y);
    Point3D pos(position.x,position.y,position.z+1);
	
	World* world=TheWorldMgr->GetWorld();
	// DetectCollision works too if data not needed !
    CollisionState state = world->QueryCollision(pos, pos +  shotDirection* 100.F, 0.0F,kCollisionProjectile , &collisionData);
    if (state == kCollisionStateGeometry){
        printf("GEOMETRY \n");
		TheEngine->Report("Hit Geometry \n");
    }else if (state == kCollisionStateRigidBody){
        printf("BODY \n");
		TheEngine->Report("Hit Body \n");
		if (collisionData.rigidBody->GetControllerType() == kControllerGoblin) { //LB: added for player shooting - TODO: check that it is shooting here because enemy type might be general
			TheEngine->Report("Hit Goblin \n");
			MMGame::EnemyController *enemyContr = static_cast<MMGame::EnemyController *>(collisionData.rigidBody);
			enemyContr->Damage(40, 0); //LB: call damage on enemy
		}
    }else{
        printf(" Miss` \n");
		TheEngine->Report("Missed \n");
        
    };

}


//----------------------------------------------------------------------------------------------------------------

CreateFighterMessage::CreateFighterMessage(ModelMessageType type) : CreateModelMessage(type)
{
}

CreateFighterMessage::CreateFighterMessage(ModelMessageType type, int32 fighterIndex, const Point3D& position, float azm, float alt, unsigned_int32 movement, int32 weapon, int32 weaponController, int32 key) : CreateModelMessage(type, fighterIndex, position)
{
	initialAzimuth = azm;
	initialAltitude = alt;

	movementFlags = movement;

	weaponIndex = weapon;
	weaponControllerIndex = weaponController;

	playerKey = key;
}

CreateFighterMessage::~CreateFighterMessage()
{
}

void CreateFighterMessage::CompressMessage(Compressor& data) const
{
	CreateModelMessage::CompressMessage(data);

	data << initialAzimuth;
	data << initialAltitude;

	data << unsigned_int8(movementFlags);

	data << unsigned_int8(weaponIndex);
	data << weaponControllerIndex;

	data << int16(playerKey);
}

bool CreateFighterMessage::DecompressMessage(Decompressor& data)
{
	if (CreateModelMessage::DecompressMessage(data))
	{
		int16			key;
		unsigned_int8	movement;
		unsigned_int8	weapon;

		data >> initialAzimuth;
		data >> initialAltitude;

		data >> movement;
		movementFlags = movement;

		data >> weapon;
		weaponIndex = weapon;

		data >> weaponControllerIndex;

		data >> key;
		playerKey = key;

		return (true);
	}

	return (false);
}




FighterMovementMessage::FighterMovementMessage(ControllerMessageType type, int32 controllerIndex) : CharacterStateMessage(type, controllerIndex)
{
}

FighterMovementMessage::FighterMovementMessage(ControllerMessageType type, int32 controllerIndex, const Point3D& position, const Vector3D& velocity, float azimuth, float altitude, unsigned_int32 flag) : CharacterStateMessage(type, controllerIndex, position, velocity)
{
	movementAzimuth = azimuth;
	movementAltitude = altitude;
	movementFlag = flag;
}

FighterMovementMessage::~FighterMovementMessage()
{
}

void FighterMovementMessage::CompressMessage(Compressor& data) const
{
	CharacterStateMessage::CompressMessage(data);

	data << movementAzimuth;
	data << movementAltitude;

	data << unsigned_int8(movementFlag);
}

bool FighterMovementMessage::DecompressMessage(Decompressor& data)
{
	if (CharacterStateMessage::DecompressMessage(data))
	{
		unsigned_int8	flag;

		data >> movementAzimuth;
		data >> movementAltitude;

		data >> flag;
		movementFlag = flag;

		return (true);
	}

	return (false);
}




FighterUpdateMessage::FighterUpdateMessage(int32 controllerIndex) : ControllerMessage(FighterController::kFighterMessageUpdate, controllerIndex)
{
}

FighterUpdateMessage::FighterUpdateMessage(int32 controllerIndex, float azimuth, float altitude) : ControllerMessage(FighterController::kFighterMessageUpdate, controllerIndex)
{
	updateAzimuth = azimuth;
	updateAltitude = altitude;

	SetMessageFlags(kMessageUnreliable);
}

FighterUpdateMessage::~FighterUpdateMessage()
{
}

void FighterUpdateMessage::CompressMessage(Compressor& data) const
{
	ControllerMessage::CompressMessage(data);

	data << updateAzimuth;
	data << updateAltitude;
}

bool FighterUpdateMessage::DecompressMessage(Decompressor& data)
{
	if (ControllerMessage::DecompressMessage(data))
	{
		data >> updateAzimuth;
		data >> updateAltitude;
		return (true);
	}

	return (false);
}




SpineTwistAnimator::SpineTwistAnimator() : Animator(kAnimatorSpineTwist)
{
}

SpineTwistAnimator::SpineTwistAnimator(Model *model, Node *node) : Animator(kAnimatorSpineTwist, model, node)
{
}

SpineTwistAnimator::~SpineTwistAnimator()
{
}

void SpineTwistAnimator::PreprocessAnimator(void)
{
	Animator::PreprocessAnimator();

	const Animator *animator = GetSuperNode()->GetFirstSubnode();
	const Node *node = GetTargetNode()->GetSuperNode();
	superNodeTransformIndex[0] = animator->GetNodeTransformIndex(node);
	superNodeTransformIndex[1] = animator->GetNodeTransformIndex(node->GetSuperNode());

	spineRotation = 1.0F;
}

void SpineTwistAnimator::ConfigureAnimator(void)
{
	AllocateStorage(GetAnimatorTransformNodeStart(), 3, 3);

	AnimatorTransform **outputTable = GetOutputTransformTable();
	AnimatorTransform *transformTable = GetAnimatorTransformTable();
	for (machine a = 0; a < 3; a++)
	{
		outputTable[a] = &transformTable[a];
	}
}

void SpineTwistAnimator::MoveAnimator(void)
{
	const Animator *animator = GetSuperNode()->GetFirstSubnode();
	int32 spineIndex = GetOutputTransformNodeStart() - animator->GetOutputTransformNodeStart();

	const AnimatorTransform *table = animator->GetAnimatorTransformTable();
	const AnimatorTransform *transform1 = &table[superNodeTransformIndex[0]];
	const AnimatorTransform *transform2 = &table[superNodeTransformIndex[1]];

	Quaternion superRotation = transform2->rotation * transform1->rotation;

	AnimatorTransform *transformTable = GetAnimatorTransformTable();
	for (machine a = 0; a < 3; a++)
	{
		Quaternion q = Conjugate(superRotation) * spineRotation * superRotation;

		const AnimatorTransform *transform = &table[spineIndex + a];
		superRotation = superRotation * transform->rotation;

		transformTable[a].rotation = q * transform->rotation;
		transformTable[a].position = transform->position;
	}
}


ScaleAnimator::ScaleAnimator() : Animator(kAnimatorScale)
{
}

ScaleAnimator::ScaleAnimator(Model *model, Node *node) : Animator(kAnimatorScale, model, node)
{
	scale = 1.0F;
}

ScaleAnimator::~ScaleAnimator()
{
}

void ScaleAnimator::ConfigureAnimator(void)
{
	AllocateStorage(GetAnimatorTransformNodeStart(), 1, 1);

	AnimatorTransform **outputTable = GetOutputTransformTable();
	AnimatorTransform *transformTable = GetAnimatorTransformTable();
	outputTable[0] = &transformTable[0];
}

void ScaleAnimator::MoveAnimator(void)
{
	const Animator *animator = GetSuperNode()->GetFirstSubnode();
	int32 targetIndex = GetOutputTransformNodeStart() - animator->GetOutputTransformNodeStart();

	const AnimatorTransform *previousTable = animator->GetAnimatorTransformTable();
	AnimatorTransform *transformTable = GetAnimatorTransformTable();
	transformTable[0].rotation = previousTable[targetIndex].rotation * scale;
	transformTable[0].position = previousTable[targetIndex].position * scale;
}


/*---------------- ADDED THESE ------*/

void FighterController::AnimateFighter(void)
{
    //`if (!(GetFighterFlags() & kFighterDead))
    {
        Quaternion      rotation;
        
        float azm = GetInterpolatedLookAzimuth();
        float alt = GetInterpolatedLookAltitude();
         modelAzimuth = GetModelAzimuth();
        
        Model *fighter = GetTargetNode();
        
        int32 motion = GetFighterMotion();
        if ((motion <= kFighterMotionStand) || (motion == kFighterMotionForward) || (motion == kFighterMotionBackward))
        {
            rotation = Quaternion::MakeRotationY(-alt * Math::one_over_3);
        }
        else
        {
            const Matrix3D& p = GetFrameAnimator(1)->GetTransformTrackHeader()->GetTransformFrameData()->transform;
            Transform4D m = fighter->GetFirstSubnode()->GetNodeTransform() * Inverse(p);
            float x = m(0,0);
            float y = m(1,0);
            modelAzimuth += Arctan(y, x);
            
            float d = InverseSqrt(x * x + y * y);
            rotation = Quaternion::MakeRotation(alt * Math::one_over_3, Vector3D(y * d, -x * d, 0.0F));
        }
        
        float f = azm - modelAzimuth;
        if (f < -Math::tau_over_2)
        {
            f += Math::tau;
        }
        else if (f > Math::tau_over_2)
        {
            f -= Math::tau;
        }
        
        rotation = Quaternion::MakeRotationZ(f * Math::one_over_3) * rotation;
        spineTwistAnimator->SetSpineRotation(rotation);
    }
    
    //FighterController::AnimateFighter();
    GetTargetNode()->AnimateModel();
}



void FighterController::SetFighterMotion(int32 motion)
{
    //FighterController::SetFighterMotion(motion);
    
    fighterMotion = motion;
    
    FrameAnimator *animator1 = GetFrameAnimator(0);
    FrameAnimator *animator2 = GetFrameAnimator(1);
    Interpolator *interpolator1 = animator1->GetFrameInterpolator();
    Interpolator *interpolator2 = animator2->GetFrameInterpolator();
    Interpolator *weight1 = animator1->GetWeightInterpolator();
    Interpolator *weight2 = animator2->GetWeightInterpolator();
    
    switch (motion)
    {
        case kFighterMotionStop:
        {
            interpolator1->SetMode(kInterpolatorForward | kInterpolatorLoop);
            
            float w = weight2->GetValue();
            weight2->SetState(w, 0.004F, kInterpolatorBackward);
            weight1->SetState(1.0F - w, 0.004F, kInterpolatorForward);
            break;
        }
            
        case kFighterMotionStand:
            
            interpolator1->SetValue(0.0F);
            interpolator1->SetMode(kInterpolatorForward | kInterpolatorLoop);
            weight1->SetState(1.0F, 0.0F, kInterpolatorStop);
            weight2->SetState(0.0F, 0.0F, kInterpolatorStop);
            break;
            
        case kFighterMotionForward:
        {
            animator2->SetAnimation("soldier/Run");
            interpolator2->SetMode(kInterpolatorForward | kInterpolatorLoop);
            
            float w = weight2->GetValue();
            weight2->SetState(w, 0.004F, kInterpolatorForward);
            weight1->SetState(1.0F - w, 0.004F, kInterpolatorBackward);
            break;
        }
            
        case kFighterMotionBackward:
        {
            animator2->SetAnimation("soldier/Backward");
            interpolator2->SetMode(kInterpolatorBackward | kInterpolatorLoop);
            interpolator2->SetValue(1.0F);
            
            float w = weight2->GetValue();
            weight2->SetState(w, 0.004F, kInterpolatorForward);
            weight1->SetState(1.0F - w, 0.004F, kInterpolatorBackward);
            break;
        }
        case kFighterMotionTurnLeft:
            
            animator2->SetAnimation("soldier/TurnLeft");
            interpolator2->SetMode(kInterpolatorForward);
            
            interpolator1->SetMode(kInterpolatorStop);
            weight1->SetState(0.0F, 0.0F, kInterpolatorStop);
            weight2->SetState(1.0F, 0.0F, kInterpolatorStop);
            break;
            
        case kFighterMotionTurnRight:
            
            animator2->SetAnimation("soldier/TurnRight");
            interpolator2->SetMode(kInterpolatorForward);
            
            interpolator1->SetMode(kInterpolatorStop);
            weight1->SetState(0.0F, 0.0F, kInterpolatorStop);
            weight2->SetState(1.0F, 0.0F, kInterpolatorStop);
            break;
            
        case kFighterMotionJump:
            
            animator2->SetAnimation("soldier/Jump");
            interpolator2->SetMode(kInterpolatorForward);
            break;
        
            
        case kFighterMotionDeath:
            
            animator2->SetAnimation("soldier/Death");
            interpolator2->SetMode(kInterpolatorForward);
            
            interpolator1->SetMode(kInterpolatorStop);
            weight1->SetState(0.0F, 0.0F, kInterpolatorStop);
            weight2->SetState(1.0F, 0.0F, kInterpolatorStop);
            break;
    }
}

/* ----- Make it look different for each player */
