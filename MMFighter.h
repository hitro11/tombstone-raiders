
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


#ifndef MMFighter_h
#define MMFighter_h


#include "TSWorld.h"

#include "MMMultiplayer.h"
#include "MMCharacter.h"
#include "MMGamePlayer.h"






namespace MMGame
{
	using namespace Tombstone;
    

       enum : ModelType
        {
                kModelSoldier                   = 'sold'
        };
    
    enum{
        kControllerSoldier
    };

	enum : ModelType
	{
		kModelNPCPyramid			= 'nppy'
	};

	enum {
		kControllerNPCPyramid = 'cnpc' //TODO: move to GoblinEnemyController
	};

	enum : AnimatorType
	{
		kAnimatorSpineTwist			= 'spin',
		kAnimatorScale				= 'scal',
		kAnimatorWeaponIK			= 'wpik'
	};


	enum : InteractionType
	{
		kInteractionTake			= 'TAKE'
	};



	enum
	{
		kFighterIconChat,
		kFighterIconCount
	};


	enum
	{
		kFighterDead				= 1 << 0,
		kFighterFiringPrimary		= 1 << 1,
		kFighterFiringSecondary		= 1 << 2,
		kFighterTargetDistance		= 1 << 3,

		kFighterFiring				= kFighterFiringPrimary | kFighterFiringSecondary
	};


	enum
	{
		kFighterMotionNone,
		kFighterMotionStop,
		kFighterMotionStand,
		kFighterMotionForward,
		kFighterMotionBackward,
		kFighterMotionTurnLeft,
		kFighterMotionTurnRight,
		kFighterMotionJump,
		kFighterMotionDeath
	};

    
    class SpineTwistAnimator : public Animator
    {
    private:
        
        int32			superNodeTransformIndex[2];
        Quaternion		spineRotation;
        
        SpineTwistAnimator();
        
    public:
        
        SpineTwistAnimator(Model *model, Node *node);
        ~SpineTwistAnimator();
        
        void SetSpineRotation(const Quaternion& q)
        {
            spineRotation = q;
        }
        
        void PreprocessAnimator(void) override;
        void ConfigureAnimator(void) override;
        void MoveAnimator(void) override;
    };

	class FighterController;


	class FighterInteractor final : public Interactor
	{
		private:

			FighterController	*fighterController;

		public:

			FighterInteractor(FighterController *controller);
			~FighterInteractor();

			void HandleInteractionEvent(InteractionEventType type, Node *node, const InteractionProperty *property, const Point3D *position = nullptr) override;
	};


	class FighterController : public GameCharacterController
	{
		private:

			unsigned_int32			fighterFlags;
			Link<Player>            fighterPlayer;
 
			float					primaryAzimuth; 
			float					modelAzimuth;
 
			float					lookAzimuth; 
			float					lookAltitude; 
			float					deltaLookAzimuth;
			float					deltaLookAltitude; 
			float					lookInterpolateParam; 
 
			Point3D					previousCenterOfMass;

			unsigned_int32			movementFlags; 
			int32					fighterMotion;
			bool					motionComplete;

			Vector3D				firingDirection;
			float					targetDistance;

			int32					deathTime;
			int32					damageTime;
			unsigned_int32			weaponSwitchTime;

			Model					*weaponModel;
			const Marker			*weaponFireMarker;
			const Marker			*weaponMountMarker;

			int32					iconIndex;
			QuadEffect				*iconEffect;

			Node					*mountNode;
			Light					*flashlight;

			MergeAnimator			*rootAnimator;
			MergeAnimator			*mergeAnimator;
			BlendAnimator			*blendAnimator;
			FrameAnimator			*frameAnimator[2];
        
            SpineTwistAnimator			*spineTwistAnimator;
        
        
            // MVM
            int playerkey;


			FighterInteractor								fighterInteractor;
			Observer<FighterController, WorldObservable>	worldUpdateObserver;
			FrameAnimatorObserver<FighterController>		frameAnimatorObserver;

			void SetOrientation(float azm, float alt)
			{
				lookAzimuth = azm;
				lookAltitude = alt;
				lookInterpolateParam = 0.0F;
			}
        

			void SetMountNodeTransform(void);

			void HandleWorldUpdate(WorldObservable *observable);
			void HandleAnimationEvent(FrameAnimator *animator, CueType cueType);

			static void HandleMotionCompletion(Interpolator *interpolator, void *cookie);



		protected:


			float GetModelAzimuth(void) const
			{
				return (modelAzimuth);
			}

			float GetInterpolatedLookAzimuth(void) const
			{
				return (lookAzimuth + deltaLookAzimuth * lookInterpolateParam);
			}

			float GetInterpolatedLookAltitude(void) const
			{
				return (lookAltitude + deltaLookAltitude * lookInterpolateParam);
			}

			bool DamageTimeExpired(int32 reset)
			{
				if (damageTime > 0)
				{
					return (false);
				}

				damageTime = reset;
				return (true);
			}

			MergeAnimator *GetMergeAnimator(void) const
			{
				return (mergeAnimator);
			}

			BlendAnimator *GetBlendAnimator(void) const
			{
				return (blendAnimator);
			}

			FrameAnimator *GetFrameAnimator(int32 index) const
			{
				return (frameAnimator[index]);
			}

			void SetFrameAnimatorObserver(FrameAnimator::ObserverType *observer)
			{
				frameAnimator[0]->SetObserver(observer);
				frameAnimator[1]->SetObserver(observer);
			}

		public:
			Node *BarrelNode;
            
			void fireLaser(void);


			FighterController(ControllerType type);

			enum
			{
				kFighterMessageEngageInteraction = kRigidBodyMessageBaseCount,
				kFighterMessageDisengageInteraction,
				kFighterMessageBeginMovement,
				kFighterMessageEndMovement,
				kFighterMessageChangeMovement,
				kFighterMessageBeginShield,
				kFighterMessageEndShield,
				kFighterMessageBeginIcon,
				kFighterMessageEndIcon,
				kFighterMessageTeleport,
				kFighterMessageLaunch,
				kFighterMessageLand,
				kFighterMessageUpdate,
				kFighterMessageWeapon,
				kFighterMessageEmptyAmmo,
				kFighterMessageDamage,
				kFighterMessageDeath
			};

			~FighterController();

			unsigned_int32 GetFighterFlags(void) const
			{
				return (fighterFlags);
			}

			void SetFighterFlags(unsigned_int32 flags)
			{
				fighterFlags = flags;
		
            }

            Point3D GetPosition(void)
            {
                Node* node= GetTargetNode();
                return(node->GetWorldPosition());
            }
        
            GamePlayer *GetFighterPlayer(void) const
            {
                return (static_cast<GamePlayer *>(fighterPlayer.GetTarget()));
            }
        
			void SetFighterPlayer(Player *player)
			{
				fighterPlayer = player;
			}

			float GetPrimaryAzimuth(void) const
			{
				return (primaryAzimuth);
			}

			void SetPrimaryAzimuth(float azimuth)
			{
				primaryAzimuth = azimuth;
			}

			float GetLookAzimuth(void) const
			{
				return (lookAzimuth);
			}

			void SetLookAzimuth(float azimuth)
			{
				lookAzimuth = azimuth;
			}

			float GetLookAltitude(void) const
			{
				return (lookAltitude);
			}

			void SetLookAltitude(float altitude)
			{
				lookAltitude = altitude;
			}

			unsigned_int32 GetMovementFlags(void) const
			{
				return (movementFlags);
			}

			void SetMovementFlags(unsigned_int32 flags)
			{
				movementFlags = flags;
			}

			int32 GetFighterMotion(void) const
			{
				return (fighterMotion);
			}

			const Vector3D& GetFiringDirection(void) const
			{
				return (firingDirection);
			}

			float GetTargetDistance(void) const
			{
				return (targetDistance);
			}

			void SetTargetDistance(float distance)
			{
				targetDistance = distance;
			}

			Model *GetWeaponModel(void) const
			{
				return (weaponModel);
			}

			const Marker *GetWeaponFireMarker(void) const
			{
				return (weaponFireMarker);
			}


			Light *GetFlashlight(void) const
			{
				return (flashlight);
			}

			FighterInteractor *GetFighterInteractor(void)
			{
				return (&fighterInteractor);
			}

			const FighterInteractor *GetFighterInteractor(void) const
			{
				return (&fighterInteractor);
			}

			void Pack(Packer& data, unsigned_int32 packFlags) const override;
			void Unpack(Unpacker& data, unsigned_int32 unpackFlags) override;
			void UnpackChunk(const ChunkHeader *chunkHeader, Unpacker& data, unsigned_int32 unpackFlags);

			void PreprocessController(void) override;
			void MoveController(void) override;

			ControllerMessage *CreateMessage(ControllerMessageType type) const override;
			void ReceiveMessage(const ControllerMessage *message) override;
			void SendInitialStateMessages(Player *player) const override;
			void SendSnapshot(void) override;

			RigidBodyStatus HandleNewGeometryContact(const GeometryContact *contact) override;

			void EnterWorld(World *world, const Point3D& worldPosition) override;

			CharacterStatus Damage(Fixed damage, unsigned_int32 flags, GameCharacterController *attacker, const Point3D *position = nullptr, const Vector3D *impulse = nullptr) override;
			void Kill(GameCharacterController *attacker, const Point3D *position = nullptr, const Vector3D *impulse = nullptr) override;

			void UpdateOrientation(float azm, float alt);
			void BeginMovement(unsigned_int32 flag, float azm, float alt);
			void EndMovement(unsigned_int32 flag, float azm, float alt);
			void ChangeMovement(unsigned_int32 flags, float azm, float alt);

			void BeginFiring(bool primary, float azm, float alt);
			void EndFiring(float azm, float alt);

			void SetWeapon(int32 weaponIndex, int32 weaponControllerIndex);

			void PlayInventorySound(int32 inventoryIndex);

			void ActivateFlashlight(void);
			void DeactivateFlashlight(void);
			void ToggleFlashlight(void);

			void SetPerspectiveExclusionMask(unsigned_int32 mask) const;

			virtual void SetFighterStyle(const int32 *style, bool prep = true);
			virtual void SetFighterMotion(int32 motion);

            void AnimateFighter(void); // WAS VIRTUAL
        
        
        //MM
        void SetPlayerKey(int key){
            playerkey=key;
        }
        
    };


	class CreateFighterMessage : public CreateModelMessage
	{
		private:

			float				initialAzimuth;
			float				initialAltitude;

			unsigned_int32		movementFlags;

			int32				weaponIndex;
			int32				weaponControllerIndex;

			int32				playerKey;

		//protected:
        public:

			CreateFighterMessage(ModelMessageType type);
			~CreateFighterMessage();

		public:

			CreateFighterMessage(ModelMessageType type, int32 fighterIndex, const Point3D& position, float azm, float alt, unsigned_int32 movement, int32 weapon, int32 weaponController, int32 key);

			float GetInitialAzimuth(void) const
			{
				return (initialAzimuth);
			}

			float GetInitialAltitude(void) const
			{
				return (initialAltitude);
			}

			unsigned_int32 GetMovementFlags(void) const
			{
				return (movementFlags);
			}

			int32 GetWeaponIndex(void) const
			{
				return (weaponIndex);
			}

			int32 GetWeaponControllerIndex(void) const
			{
				return (weaponControllerIndex);
			}

			int32 GetPlayerKey(void) const
			{
				return (playerKey);
			}

			void CompressMessage(Compressor& data) const override;
			bool DecompressMessage(Decompressor& data) override;
	};




	class FighterMovementMessage : public CharacterStateMessage
	{
		friend class FighterController;

		private:

			float				movementAzimuth;
			float				movementAltitude;
			unsigned_int32		movementFlag;

			FighterMovementMessage(ControllerMessageType type, int32 controllerIndex);

		public:

			FighterMovementMessage(ControllerMessageType type, int32 controllerIndex, const Point3D& position, const Vector3D& velocity, float azimuth, float altitude, unsigned_int32 flag);
			~FighterMovementMessage();

			float GetMovementAzimuth(void) const
			{
				return (movementAzimuth);
			}

			float GetMovementAltitude(void) const
			{
				return (movementAltitude);
			}

			int32 GetMovementFlag(void) const
			{
				return (movementFlag);
			}

			void CompressMessage(Compressor& data) const override;
			bool DecompressMessage(Decompressor& data) override;
	};


	class FighterIconMessage : public ControllerMessage
	{
		friend class FighterController;

		private:

			int32		iconIndex;

			FighterIconMessage(ControllerMessageType type, int32 controllerIndex);

		public:

			FighterIconMessage(ControllerMessageType type, int32 controllerIndex, int32 icon);
			~FighterIconMessage();

			int32 GetIconIndex(void) const
			{
				return (iconIndex);
			}

			void CompressMessage(Compressor& data) const override;
			bool DecompressMessage(Decompressor& data) override;
	};


	class FighterTeleportMessage : public CharacterStateMessage
	{
		friend class FighterController;

		private:

			float			teleportAzimuth;
			float			teleportAltitude;

			Point3D			effectCenter;

			FighterTeleportMessage(int32 controllerIndex);

		public:

			FighterTeleportMessage(int32 controllerIndex, const Point3D& position, const Vector3D& velocity, float azimuth, float altitude, const Point3D& center);
			~FighterTeleportMessage();

			float GetTeleportAzimuth(void) const
			{
				return (teleportAzimuth);
			}

			float GetTeleportAltitude(void) const
			{
				return (teleportAltitude);
			}

			const Point3D& GetEffectCenter(void) const
			{
				return (effectCenter);
			}

			void CompressMessage(Compressor& data) const override;
			bool DecompressMessage(Decompressor& data) override;
	};


	class FighterUpdateMessage : public ControllerMessage
	{
		friend class FighterController;

		private:

			float		updateAzimuth;
			float		updateAltitude;

			FighterUpdateMessage(int32 controllerIndex);

		public:

			FighterUpdateMessage(int32 controllerIndex, float azimuth, float altitude);
			~FighterUpdateMessage();

			float GetUpdateAzimuth(void) const
			{
				return (updateAzimuth);
			}

			float GetUpdateAltitude(void) const
			{
				return (updateAltitude);
			}

			void CompressMessage(Compressor& data) const override;
			bool DecompressMessage(Decompressor& data) override;
	};


	class FighterWeaponMessage : public ControllerMessage
	{
		friend class FighterController;

		private:

			int32		weaponIndex;
			int32		weaponControllerIndex;

			FighterWeaponMessage(int32 controllerIndex);

		public:

			FighterWeaponMessage(int32 controllerIndex, int32 weapon, int32 weaponController);
			~FighterWeaponMessage();

			int32 GetWeaponIndex(void) const
			{
				return (weaponIndex);
			}

			int32 GetWeaponControllerIndex(void) const
			{
				return (weaponControllerIndex);
			}

			void CompressMessage(Compressor& data) const override;
			bool DecompressMessage(Decompressor& data) override;
	};






	class ScaleAnimator : public Animator
	{
		private:

			float		scale;

			ScaleAnimator();

		public:

			ScaleAnimator(Model *model, Node *node = nullptr);
			~ScaleAnimator();

			void SetScale(float s)
			{
				scale = s;
			}

			void ConfigureAnimator(void) override;
			void MoveAnimator(void) override;
	};


	class WeaponIKAnimator : public Animator
	{
		private:

			enum
			{
				kMaxSuperNodeCount = 8
			};

			LocatorType		locatorType;
			float			effectorDistance;

			const Node		*effectorNode;
			const Node		*ikTargetNode;

			int32			rootAnimationIndex;
			int32			middleAnimationIndex;
			int32			superAnimationIndex[kMaxSuperNodeCount];

			WeaponIKAnimator();

		public:

			WeaponIKAnimator(Model *model, Node *node, LocatorType type);
			~WeaponIKAnimator();

			void SetIKTarget(Node *node)
			{
				ikTargetNode = node;
			}

			void PreprocessAnimator(void) override;
			void ConfigureAnimator(void) override;
			void MoveAnimator(void) override;
	};
}


#endif

// ZUXSVMT
