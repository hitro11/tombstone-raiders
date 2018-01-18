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


#include "MMCharacter.h"


using namespace MMGame;


namespace
{
	const float kStandingVelocityTime = 400.0F;
}


GameCharacterController::GameCharacterController(CharacterType charType, ControllerType contType) : CharacterController(contType)
{
	characterType = charType;
	characterState = 0;

	standingTime = 0.0F;
	offGroundTime = 0.0F;

	SetRigidBodyType(kRigidBodyCharacter);
	SetCollisionKind(kCollisionCharacter);
	//SetCollisionExclusionMask(kCollisionSoundPath | kCollisionCorpse);
}

GameCharacterController::GameCharacterController(const GameCharacterController& gameCharacterController) : CharacterController(gameCharacterController)
{
	characterType = gameCharacterController.characterType;
	characterState = 0;

	standingTime = 0.0F;
	offGroundTime = 0.0F;
}

GameCharacterController::~GameCharacterController()
{
}

void GameCharacterController::Pack(Packer& data, unsigned_int32 packFlags) const
{
	CharacterController::Pack(data, packFlags);

	if (!(packFlags & kPackEditor))
	{
		data << ChunkHeader('STAT', 4);
		data << characterState;

		data << ChunkHeader('STND', 4);
		data << standingTime;

		data << ChunkHeader('OFFG', 4);
		data << offGroundTime;
	}

	data << TerminatorChunk;
}

void GameCharacterController::Unpack(Unpacker& data, unsigned_int32 unpackFlags)
{
	CharacterController::Unpack(data, unpackFlags);
	UnpackChunkList<GameCharacterController>(data, unpackFlags);
}

void GameCharacterController::UnpackChunk(const ChunkHeader *chunkHeader, Unpacker& data, unsigned_int32 unpackFlags)
{
	switch (chunkHeader->chunkType)
	{
		case 'STAT':

			data >> characterState;
			break;

		case 'STND':

			data >> standingTime;
			break;

		case 'OFFG':

			data >> offGroundTime;
			break;
	}
}

void GameCharacterController::MoveController(void)
{
	unsigned_int32 state = characterState;
	if (!(state & (kCharacterFlying | kCharacterSwimming)))
	{
		const CollisionContact *contact = GetGroundContact();
		if (contact)
		{
			if (state & kCharacterOffGround)
			{
				state &= ~(kCharacterOffGround | kCharacterJumping);
			}

			characterState = state | kCharacterGround;
			offGroundTime = 0.0F;

			if ((!GetSubmergedWaterBlock()) && (SquaredMag(GetExternalForce()) < Math::min_float))
			{
				float time = standingTime; 
				if (time < kStandingVelocityTime) 
				{
					standingTime = (time += TheTimeMgr->GetFloatDeltaTime()); 
					SetVelocityMultiplier(FmaxZero(1.0F - time * (1.0F / kStandingVelocityTime))); 
				} 
				else
				{ 
					SetVelocityMultiplier(0.0F); 
				} 
			}
			else
			{ 
				ResetStandingTime();
			}
		}
		else
		{
			characterState = (state | kCharacterOffGround) & ~kCharacterGround;

			standingTime = 0.0F;
			offGroundTime += TheTimeMgr->GetFloatDeltaTime();

			SetVelocityMultiplier(1.0F);

			if (!(state & kCharacterJumping))
			{
				Vector3D velocity = GetLinearVelocity();
				velocity.z = FminZero(velocity.z);
				SetLinearVelocity(velocity);
			}
		}
	}
}

void GameCharacterController::HandlePhysicsSpaceExit(void)
{
	if ((!(characterState & kCharacterDead)) && (TheMessageMgr->GetServerFlag()))
	{
		Kill(nullptr);
	}
}

const SubstanceData *GameCharacterController::GetGroundSubstanceData(void) const
{


	return (nullptr);
}

void GameCharacterController::EnterWorld(World *world, const Point3D& worldPosition)
{
}

CharacterStatus GameCharacterController::Damage(Fixed damage, unsigned_int32 flags, GameCharacterController *attacker, const Point3D *position, const Vector3D *force)
{
	return (kCharacterUnaffected);
}

void GameCharacterController::Kill(GameCharacterController *attacker, const Point3D *position, const Vector3D *force)
{
	characterState = (characterState | kCharacterDead) & ~kCharacterAttackable;
}

void GameCharacterController::SetAttackable(bool attackable)
{
	unsigned_int32 state = characterState;
	if (attackable)
	{
		if (!(state & kCharacterDead))
		{
			characterState = state | kCharacterAttackable;
		}
	}
	else
	{
		characterState = state & ~kCharacterAttackable;
	}
}


CharacterStateMessage::CharacterStateMessage(ControllerMessageType type, int32 controllerIndex) : ControllerMessage(type, controllerIndex)
{
}

CharacterStateMessage::CharacterStateMessage(ControllerMessageType type, int32 controllerIndex, const Point3D& position, const Vector3D& velocity) : ControllerMessage(type, controllerIndex)
{
	initialPosition = position;
	initialVelocity = velocity;
}

CharacterStateMessage::~CharacterStateMessage()
{
}

void CharacterStateMessage::CompressMessage(Compressor& data) const
{
	ControllerMessage::CompressMessage(data);

	data << initialPosition;
	data << initialVelocity;
}

bool CharacterStateMessage::DecompressMessage(Decompressor& data)
{
	if (ControllerMessage::DecompressMessage(data))
	{
		data >> initialPosition;
		data >> initialVelocity;
		return (true);
	}

	return (false);
}


AnimationBlender::AnimationBlender()
{
	blendParity = 0;
}

AnimationBlender::~AnimationBlender()
{
}

void AnimationBlender::Pack(Packer& data, unsigned_int32 packFlags) const
{
	data << ChunkHeader('PRTY', 4);
	data << blendParity;

	PackHandle handle = data.BeginChunk('BLND');
	blendAnimator.Pack(data, packFlags);
	data.EndChunk(handle);

	handle = data.BeginChunk('FRM0');
	frameAnimator[0].Pack(data, packFlags);
	data.EndChunk(handle);

	handle = data.BeginChunk('FRM1');
	frameAnimator[1].Pack(data, packFlags);
	data.EndChunk(handle);

	data << TerminatorChunk;
}

void AnimationBlender::Unpack(Unpacker& data, unsigned_int32 unpackFlags)
{
	UnpackChunkList<AnimationBlender>(data, unpackFlags);
}

void AnimationBlender::UnpackChunk(const ChunkHeader *chunkHeader, Unpacker& data, unsigned_int32 unpackFlags)
{
	switch (chunkHeader->chunkType)
	{
		case 'PRTY':

			data >> blendParity;
			break;

		case 'BLND':

			blendAnimator.Unpack(data, unpackFlags);
			break;

		case 'FRM0':

			frameAnimator[0].Unpack(data, unpackFlags);
			break;

		case 'FRM1':

			frameAnimator[1].Unpack(data, unpackFlags);
			break;
	}
}

void AnimationBlender::PreprocessAnimationBlender(Model *model)
{
	blendAnimator.SetTargetModel(model);
	frameAnimator[0].SetTargetModel(model);
	frameAnimator[1].SetTargetModel(model);

	blendAnimator.AppendSubnode(&frameAnimator[0]);
	blendAnimator.AppendSubnode(&frameAnimator[1]);
	model->SetRootAnimator(&blendAnimator);
}

FrameAnimator *AnimationBlender::StartAnimation(const char *name, unsigned_int32 mode, Interpolator::CompletionCallback *callback, void *cookie)
{
	FrameAnimator *animator = &frameAnimator[0];

	animator->SetAnimation(name);
	animator->GetWeightInterpolator()->SetState(1.0F, 0.0F, kInterpolatorStop);

	Interpolator *interpolator = animator->GetFrameInterpolator();
	interpolator->SetMode(mode);
	interpolator->SetCompletionCallback(callback, cookie);

	frameAnimator[1].GetWeightInterpolator()->SetState(0.0F, 0.0F, kInterpolatorStop);
	frameAnimator[1].GetFrameInterpolator()->SetCompletionCallback(nullptr);

	blendParity = 0;
	return (animator);
}

FrameAnimator *AnimationBlender::BlendAnimation(const char *name, unsigned_int32 mode, float blendRate, Interpolator::CompletionCallback *callback, void *cookie)
{
	int32 parity = blendParity;
	FrameAnimator *oldAnimator = &frameAnimator[parity];
	FrameAnimator *newAnimator = &frameAnimator[parity ^= 1];
	blendParity = parity;

	newAnimator->SetAnimation(name);
	newAnimator->GetWeightInterpolator()->SetState(0.0F, blendRate, kInterpolatorForward);

	Interpolator *interpolator = newAnimator->GetFrameInterpolator();
	interpolator->SetMode(mode);
	interpolator->SetCompletionCallback(callback, cookie);

	oldAnimator->GetWeightInterpolator()->SetState(1.0F, blendRate, kInterpolatorBackward);
	oldAnimator->GetFrameInterpolator()->SetCompletionCallback(nullptr);

	return (newAnimator);
}

// ZUXSVMT
