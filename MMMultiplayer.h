
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


#ifndef MMMultiplayer_h
#define MMMultiplayer_h


#include "MMCharacter.h"
//#include "MMGamePlayer.h"
//#include "MMMultiplayer.h"
//#include "MMProperties.h"
#include "SimpleTurret.h"


namespace MMGame
{
	using namespace Tombstone;


	typedef unsigned_int32	ModelMessageType;
    
    
    struct playerState
    {
        int32 state;
    };


	enum
	{
		kGameProtocol	= 0x00000019,
		kGamePort		= 28327
	};


	enum
	{
		kMessageServerInfo = kMessageBaseCount,
		kMessageGameInfo,
		kMessagePlayerStyle,
		kMessageCreateModel,
		kMessageScore,
		kMessageHealth,
		kMessagePrimaryAmmo,
		kMessageSecondaryAmmo,
		kMessageWeapon,
		kMessagePower,
		kMessageTreasure,
		kMessageAddInventory,
		kMessageRemoveInventory,
		kMessageDeath,
		kMessageRefreshScoreboard,
		kMessageClientStyle,
		kMessageClientOrientation,
		kMessageClientMovementBegin,
		kMessageClientMovementEnd,
		kMessageClientMovementChange,
		kMessageClientFiringPrimaryBegin,
		kMessageClientFiringSecondaryBegin,
		kMessageClientFiringEnd,
		kMessageClientInteractionBegin,
		kMessageClientInteractionEnd,
		kMessageClientSwitchWeapon,
		kMessageClientDeferWeapon,
		kMessageClientSpecialWeapon,
		kMessageClientCycleWeapon,
		kMessageClientSpawn,
		kMessageClientScoreOpen,
		kMessageClientScoreClose,
		kMessageClientChatOpen,
		kMessageClientChatClose,
		kMessageClientVoiceReceiveStart,
		kMessageClientVoiceReceiveStop,
        kMessageCreateCharacter, // ADDED THIS
        kMessageRequestAvantar,
		kMessageTurretRotation,
		kMessageTurretCreation,
		kMessageTurretHitEnemy,
		kMessageDisplayEnemyBlood,
		kMessageClientRequestWorldControllers,
		kMessageMonsterMovement,
		kMessageMonsterAttack
	};


	enum
	{
		kModelMessageArrow,
		kModelMessageExplosiveArrow,
		kModelMessageSpike,
		kModelMessageRailSpike,
		kModelMessageGrenade,
		kModelMessageCharge,
		kModelMessageRocket,
		kModelMessagePlasma,
		kModelMessageFireball,
		kModelMessageTarball,
		kModelMessageLavaball,
		kModelMessageVenom,
		kModelMessageBlackCat,
		kModelMessageGusGraves,
		kModelMessageSoldier
	};

	enum
	{
		kPlayerStyleEmissionColor,
		kPlayerStyleArmorColor,
		kPlayerStyleArmorTexture,
		kPlayerStyleHelmetType,
		kPlayerStyleCount
	};


	enum
	{
		kPlayerInactive				= 1 << 0,
		kPlayerReceiveVoiceChat		= 1 << 1,
		kPlayerScoreUpdate			= 1 << 2,
		kPlayerScoreboardOpen		= 1 << 3
	};


	enum 
	{ 
		kMultiplayerDedicated		= 1 << 0
	};
 
 
	class GameWorld;
	class FighterController; 
 
 
	class ServerInfoMessage : public Message
	{
		friend class Game; 

		private:

			int32						playerCount;
			int32						maxPlayerCount;
			String<kMaxGameNameLength>	gameName;
			ResourceName				worldName;

			ServerInfoMessage();

		public:

			ServerInfoMessage(int32 numPlayers, int32 maxPlayers, const char *game, const char *world);
			~ServerInfoMessage();

			int32 GetPlayerCount(void) const
			{
				return (playerCount);
			}

			int32 GetMaxPlayerCount(void) const
			{
				return (maxPlayerCount);
			}

			const char *GetGameName(void) const
			{
				return (gameName);
			}

			const char *GetWorldName(void) const
			{
				return (worldName);
			}

			void CompressMessage(Compressor& data) const override;
			bool DecompressMessage(Decompressor& data) override;
	};


	class GameInfoMessage : public Message
	{
		friend class Game;

		private:

			unsigned_int32		multiplayerFlags;
			ResourceName		worldName;

			GameInfoMessage();

		public:

			GameInfoMessage(unsigned_int32 flags, const char *world);
			~GameInfoMessage();

			unsigned_int32 GetMultiplayerFlags(void) const
			{
				return (multiplayerFlags);
			}

			const char *GetWorldName(void) const
			{
				return (worldName);
			}

			void CompressMessage(Compressor& data) const override;
			bool DecompressMessage(Decompressor& data) override;

			bool HandleMessage(Player *sender) const override;
	};


	class PlayerStyleMessage : public Message
	{
		friend class Game;

		private:

			PlayerKey		playerKey;
			int32			playerStyle[kPlayerStyleCount];

			PlayerStyleMessage();

		public:

			PlayerStyleMessage(PlayerKey player, const int32 *style);
			~PlayerStyleMessage();

			PlayerKey GetPlayerKey(void) const
			{
				return (playerKey);
			}

			const int32 *GetPlayerStyle(void) const
			{
				return (playerStyle);
			}

			void CompressMessage(Compressor& data) const override;
			bool DecompressMessage(Decompressor& data) override;

			bool HandleMessage(Player *sender) const override;
	};


	class CreateModelMessage : public Message
	{
		private:

			ModelMessageType		modelMessageType;

			int32					controllerIndex;
			Point3D					initialPosition;

		protected:

			CreateModelMessage(ModelMessageType type);

			void InitializeModel(GameWorld *world, Model *model, Controller *controller) const;

		public:

			CreateModelMessage(ModelMessageType type, int32 index, const Point3D& position);
			~CreateModelMessage();

			ModelMessageType GetModelMessageType(void) const
			{
				return (modelMessageType);
			}

			int32 GetControllerIndex(void) const
			{
				return (controllerIndex);
			}

			const Point3D& GetInitialPosition(void) const
			{
				return (initialPosition);
			}

			void CompressMessage(Compressor& data) const override;
			bool DecompressMessage(Decompressor& data) override;

			static CreateModelMessage *CreateMessage(ModelMessageType type);
	};


	class ScoreMessage : public Message
	{
		friend class Game;

		private:

			int32		playerScore;

			ScoreMessage();

		public:

			ScoreMessage(int32 score);
			~ScoreMessage();

			int32 GetPlayerScore(void) const
			{
				return (playerScore);
			}

			void CompressMessage(Compressor& data) const override;
			bool DecompressMessage(Decompressor& data) override;

			bool HandleMessage(Player *sender) const override;
	};


	class HealthMessage : public Message
	{
		friend class Game;

		private:

			int32		playerHealth;

			HealthMessage();

		public:

			HealthMessage(int32 health);
			~HealthMessage();

			int32 GetPlayerHealth(void) const
			{
				return (playerHealth);
			}

			void CompressMessage(Compressor& data) const override;
			bool DecompressMessage(Decompressor& data) override;

			bool HandleMessage(Player *sender) const override;
	};


	class WeaponMessage : public Message
	{
		friend class Game;

		private:

			int32		weaponIndex;
			int32		weaponAmmo[2];

			WeaponMessage();

		public:

			WeaponMessage(int32 weapon, int32 ammo1, int32 ammo2);
			~WeaponMessage();

			int32 GetWeaponIndex(void) const
			{
				return (weaponIndex);
			}

			int32 GetWeaponAmmo(int32 index) const
			{
				return (weaponAmmo[index]);
			}

			void CompressMessage(Compressor& data) const override;
			bool DecompressMessage(Decompressor& data) override;

			bool HandleMessage(Player *sender) const override;
	};


	class AmmoMessage : public Message
	{
		friend class Game;

		private:

			int32		weaponIndex;
			int32		weaponAmmo;

			AmmoMessage(MessageType type);

		public:

			AmmoMessage(MessageType type, int32 weapon, int32 ammo);
			~AmmoMessage();

			int32 GetWeaponIndex(void) const
			{
				return (weaponIndex);
			}

			int32 GetWeaponAmmo(void) const
			{
				return (weaponAmmo);
			}

			void CompressMessage(Compressor& data) const override;
			bool DecompressMessage(Decompressor& data) override;

			bool HandleMessage(Player *sender) const override;
	};


	class PowerMessage : public Message
	{
		friend class Game;

		private:

			int32		powerIndex;
			int32		powerTime;

			PowerMessage();

		public:

			PowerMessage(int32 power, int32 time);
			~PowerMessage();

			int32 GetPowerIndex(void) const
			{
				return (powerIndex);
			}

			void CompressMessage(Compressor& data) const override;
			bool DecompressMessage(Decompressor& data) override;

			bool HandleMessage(Player *sender) const override;
	};


	class TreasureMessage : public Message
	{
		friend class Game;

		private:

			int32		treasureIndex;
			int32		treasureCount;

			TreasureMessage();

		public:

			TreasureMessage(int32 treasure, int32 count);
			~TreasureMessage();

			int32 GetTreasureIndex(void) const
			{
				return (treasureIndex);
			}

			int32 GetTreasureCount(void) const
			{
				return (treasureCount);
			}

			void CompressMessage(Compressor& data) const override;
			bool DecompressMessage(Decompressor& data) override;

			bool HandleMessage(Player *sender) const override;
	};


	class InventoryMessage : public Message
	{
		friend class Game;

		private:

			PlayerKey		playerKey;
			int32			inventoryIndex;

			InventoryMessage(MessageType type);

		public:

			InventoryMessage(MessageType type, PlayerKey player, int32 inventory);
			~InventoryMessage();

			PlayerKey GetPlayerKey(void) const
			{
				return (playerKey);
			}

			int32 GetInventoryIndex(void) const
			{
				return (inventoryIndex);
			}

			void CompressMessage(Compressor& data) const override;
			bool DecompressMessage(Decompressor& data) override;

			bool HandleMessage(Player *sender) const override;
	};


	class DeathMessage : public Message
	{
		friend class Game;

		private:

			PlayerKey		playerKey;
			PlayerKey		attackerKey;

			DeathMessage();

		public:

			DeathMessage(PlayerKey player, PlayerKey attacker);
			~DeathMessage();

			PlayerKey GetPlayerKey(void) const
			{
				return (playerKey);
			}

			PlayerKey GetAttackerKey(void) const
			{
				return (attackerKey);
			}

			void CompressMessage(Compressor& data) const override;
			bool DecompressMessage(Decompressor& data) override;

			bool HandleMessage(Player *sender) const override;
	};


	class RefreshScoreboardMessage : public Message
	{
		friend class Game;

		public:

			enum
			{
				kMaxScoreboardRefreshCount = (kMaxMessageDataSize - 2) / 8
			};

			struct PlayerData
			{
				PlayerKey	playerKey;
				int32		playerScore;
				int32		playerPing;
			};

		private:

			int32			playerCount;
			PlayerData		playerData[kMaxScoreboardRefreshCount];

			RefreshScoreboardMessage();

		public:

			RefreshScoreboardMessage(int32 count);
			~RefreshScoreboardMessage();

			int32 GetPlayerCount(void) const
			{
				return (playerCount);
			}

			PlayerData *GetPlayerData(int32 index)
			{
				return (&playerData[index]);
			}

			const PlayerData *GetPlayerData(int32 index) const
			{
				return (&playerData[index]);
			}

			void CompressMessage(Compressor& data) const override;
			bool DecompressMessage(Decompressor& data) override;

			bool HandleMessage(Player *sender) const override;
	};


	class ClientStyleMessage : public Message
	{
		friend class Game;

		private:

			int32		playerStyle[kPlayerStyleCount];

			ClientStyleMessage();

		public:

			ClientStyleMessage(const int32 *style);
			~ClientStyleMessage();

			const int32 *GetPlayerStyle(void) const
			{
				return (playerStyle);
			}

			void CompressMessage(Compressor& data) const override;
			bool DecompressMessage(Decompressor& data) override;

			bool HandleMessage(Player *sender) const override;
	};


	class ClientOrientationMessage : public Message
	{
		friend class Game;

		private:

			float		orientationAzimuth;
			float		orientationAltitude;

			ClientOrientationMessage();

		public:

			ClientOrientationMessage(float azimuth, float altitude);
			~ClientOrientationMessage();

			float GetOrientationAzimuth(void) const
			{
				return (orientationAzimuth);
			}

			float GetOrientationAltitude(void) const
			{
				return (orientationAltitude);
			}

			void CompressMessage(Compressor& data) const override;
			bool DecompressMessage(Decompressor& data) override;

			bool HandleMessage(Player *sender) const override;
	};


	class ClientMovementMessage : public Message
	{
		friend class Game;

		private:

			unsigned_int32	movementFlag;
			float			movementAzimuth;
			float			movementAltitude;

			ClientMovementMessage(MessageType type);

		public:

			ClientMovementMessage(MessageType type, unsigned_int32 flag, float azimuth, float altitude);
			~ClientMovementMessage();

			int32 GetMovementFlag(void) const
			{
				return (movementFlag);
			}

			float GetMovementAzimuth(void) const
			{
				return (movementAzimuth);
			}

			float GetMovementAltitude(void) const
			{
				return (movementAltitude);
			}

			void CompressMessage(Compressor& data) const override;
			bool DecompressMessage(Decompressor& data) override;

			bool HandleMessage(Player *sender) const override;
	};


	class ClientFiringMessage : public Message
	{
		friend class Game;

		private:

			float		firingAzimuth;
			float		firingAltitude;

			ClientFiringMessage(MessageType type);

		public:

			ClientFiringMessage(MessageType type, float azimuth, float altitude);
			~ClientFiringMessage();

			float GetFiringAzimuth(void) const
			{
				return (firingAzimuth);
			}

			float GetFiringAltitude(void) const
			{
				return (firingAltitude);
			}

			void CompressMessage(Compressor& data) const override;
			bool DecompressMessage(Decompressor& data) override;

			bool HandleMessage(Player *sender) const override;
	};


	class ClientInteractionMessage : public Message
	{
		friend class Game;

		private:

			Point3D		interactionPosition;

			ClientInteractionMessage(MessageType type);

		public:

			ClientInteractionMessage(MessageType type, const Point3D& position);
			~ClientInteractionMessage();

			const Point3D& GetInteractionPosition(void) const
			{
				return (interactionPosition);
			}

			void CompressMessage(Compressor& data) const override;
			bool DecompressMessage(Decompressor& data) override;

			bool HandleMessage(Player *sender) const override;
	};


	class ClientWeaponMessage : public Message
	{
		friend class Game;

		private:

			int32		weaponIndex;

			ClientWeaponMessage(MessageType type);

		public:

			ClientWeaponMessage(MessageType type, int32 weapon);
			~ClientWeaponMessage();

			void CompressMessage(Compressor& data) const override;
			bool DecompressMessage(Decompressor& data) override;

			bool HandleMessage(Player *sender) const override;
	};


	class ClientWeaponCycleMessage : public Message
	{
		friend class Game;

		private:

			int32		cycleDirection;

			ClientWeaponCycleMessage();

		public:

			ClientWeaponCycleMessage(int32 direction);
			~ClientWeaponCycleMessage();

			void CompressMessage(Compressor& data) const override;
			bool DecompressMessage(Decompressor& data) override;

			bool HandleMessage(Player *sender) const override;
	};


	class ClientMiscMessage : public Message
	{
		public:

			ClientMiscMessage(MessageType type);
			~ClientMiscMessage();

			bool HandleMessage(Player *sender) const override;
	};

/*---------------------------------------------------------------------------------------------------*/

// I added these

//typedef int EnemyType;

//! Message to request the generation of a character (from server)

typedef int EnemyType;
    
    
    //! Request soemthing  (Avatar) form the server
    
    class ClientRequestMessage : public Message
    {
        friend class Game;
    private:
        
        int myData;
        
        ClientRequestMessage(MessageType type);
        
    public:
        
        ClientRequestMessage(MessageType type, const long data);
        ~ClientRequestMessage();
        
        const long getData(void) const
        {
            return (myData);
        }
        
        
        void CompressMessage(Compressor& data) const override;
        bool DecompressMessage(Decompressor& data) override;
        
        bool HandleMessage(Player *sender) const override;    
	};
    

class CreateCharacterMessage: public Message
{
    friend class Game;
    
private:
    
    Point3D         pos;
    unsigned_int16  index;
    EnemyType       chartype;
    PlayerKey       ownerKey;
	int				locatorIndex;
    
    CreateCharacterMessage(MessageType type);
    
public:
    
    CreateCharacterMessage(MessageType type,long index, EnemyType chartype,PlayerKey key, Point3D position,int locatorIndex);
    ~CreateCharacterMessage();


    void CompressMessage(Compressor& data) const override;
    bool DecompressMessage(Decompressor& data) override;

    bool HandleMessage(Player *sender) const override;

    
    
    long GetControllerIndex(void){
        return(index);
    }
    
    Point3D GetPosition(void){
        return(pos);
    }
    
    EnemyType GetCharType(void){
        return(chartype);
    }
};



class TurretCreationMessage : public Message
{
	friend class Game;

private:

	Point3D		position;
	ObjectType	objectType;
	PlayerKey	playerKey;
	int32		locatorIndex;

public:
	TurretCreationMessage(MessageType type);
	TurretCreationMessage(MessageType type, Point3D newPosition, ObjectType newObjectType, int32 newLocatorIndex, int32 index, unsigned_int32 flags = 0);
	~TurretCreationMessage();

	Point3D GetPosition(void) {
		return position;
	}

	ObjectType GetObjectType(void) {
		return objectType;
	}

	PlayerKey GetPlayerKey(void) {
		return playerKey;
	}

	int32 GetLocatorIndex(void) {
		return locatorIndex;
	}

	void CompressMessage(Compressor& data) const override;
	bool DecompressMessage(Decompressor& data) override;
	bool HandleMessage(Player *sender) const override;
};


class ClientRequestWorldControllersMessage : public Message
{
	friend class Game;

private:

	PlayerKey	playerKey;

public:
	ClientRequestWorldControllersMessage(MessageType type);
	ClientRequestWorldControllersMessage(MessageType type, PlayerKey newPlayerKey, unsigned_int32 flags = 0); //TODO: removed int32 index, verify this is ok
	~ClientRequestWorldControllersMessage();

	PlayerKey GetPlayerKey(void) {
		return playerKey;
	}

	void CompressMessage(Compressor& data) const override;
	bool DecompressMessage(Decompressor& data) override;
	bool HandleMessage(Player *sender) const override;
};

}

#endif

// ZUXSVMT
