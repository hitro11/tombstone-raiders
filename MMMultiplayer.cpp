// MODIFIED my MvM

//=============================================================


#include "MMMultiplayer.h"
#include "MMGame.h"

#include "MMFighter.h"
#include "MMGameWorld.h"

using namespace MMGame;


namespace
{
	enum
	{
		kScoreUpdateInterval			= 150,
		kScoreboardRefreshInterval		= 2000,
		kPlayerRespawnInterval			= 1500,
		kProtectionShieldInterval		= 4000
	};
}


ServerInfoMessage::ServerInfoMessage() : Message(kMessageServerInfo)
{
}

ServerInfoMessage::ServerInfoMessage(int32 numPlayers, int32 maxPlayers, const char *game, const char *world) : Message(kMessageServerInfo)
{
	playerCount = numPlayers;
	maxPlayerCount = maxPlayers;

	gameName = game;
	worldName = world;
}

ServerInfoMessage::~ServerInfoMessage()
{
}

void ServerInfoMessage::CompressMessage(Compressor& data) const
{
	data << unsigned_int16(playerCount);
	data << unsigned_int16(maxPlayerCount);
	data << gameName;
	data << worldName;
}

bool ServerInfoMessage::DecompressMessage(Decompressor& data)
{
	unsigned_int16	count, max;

	data >> count;
	data >> max;

	if (count > max)
	{
		return (false);
	}

	playerCount = count;
	maxPlayerCount = max;

	data >> gameName;
	data >> worldName;
	return (true);
}


GameInfoMessage::GameInfoMessage() : Message(kMessageGameInfo)
{
}

GameInfoMessage::GameInfoMessage(unsigned_int32 flags, const char *world) : Message(kMessageGameInfo)
{
	multiplayerFlags = flags;
	worldName = world;
}

GameInfoMessage::~GameInfoMessage()
{
}

void GameInfoMessage::CompressMessage(Compressor& data) const
{
	data << multiplayerFlags;
	data << worldName;
}

bool GameInfoMessage::DecompressMessage(Decompressor& data) 
{ 
	data >> multiplayerFlags;
	data >> worldName; 
	return (true); 
} 

bool GameInfoMessage::HandleMessage(Player *sender) const 
{ 
	if (sender->GetPlayerKey() == kPlayerServer) 
	{
		TheGame->JoinMultiplayerGame(worldName, multiplayerFlags);
	} 

	return (true);
}


PlayerStyleMessage::PlayerStyleMessage() : Message(kMessagePlayerStyle)
{
}

PlayerStyleMessage::PlayerStyleMessage(PlayerKey player, const int32 *style) : Message(kMessagePlayerStyle)
{
	playerKey = player;

	for (machine a = 0; a < kPlayerStyleCount; a++)
	{
		playerStyle[a] = style[a];
	}
}

PlayerStyleMessage::~PlayerStyleMessage()
{
}

void PlayerStyleMessage::CompressMessage(Compressor& data) const
{
	data << int16(playerKey);

	for (machine a = 0; a < kPlayerStyleCount; a++)
	{
		data << char(playerStyle[a]);
	}
}

bool PlayerStyleMessage::DecompressMessage(Decompressor& data)
{
	int16	player;

	data >> player;
	playerKey = player;

	for (machine a = 0; a < kPlayerStyleCount; a++)
	{
		char	style;

		data >> style;
		playerStyle[a] = style;
	}

	return (true);
}

bool PlayerStyleMessage::HandleMessage(Player *sender) const
{
	GamePlayer *player = static_cast<GamePlayer *>(TheMessageMgr->GetPlayer(playerKey));
	if (player)
	{
		player->SetPlayerStyle(playerStyle);
	}

	return (true);
}


CreateModelMessage::CreateModelMessage(ModelMessageType type) : Message(kMessageCreateModel)
{
	modelMessageType = type;
}

CreateModelMessage::CreateModelMessage(ModelMessageType type, int32 index, const Point3D& position) : Message(kMessageCreateModel)
{
	modelMessageType = type;
	controllerIndex = index;
	initialPosition = position;
}

CreateModelMessage::~CreateModelMessage()
{
}

void CreateModelMessage::CompressMessage(Compressor& data) const
{
	data << char(modelMessageType);
	data << controllerIndex;
	data << initialPosition;
}

bool CreateModelMessage::DecompressMessage(Decompressor& data)
{
	// The modelMessageType value is read in Game::CreateMessage().

	data >> controllerIndex;
	data >> initialPosition;
	return (true);
}

void CreateModelMessage::InitializeModel(GameWorld *world, Model *model, Controller *controller) const
{
    /*
	controller->SetControllerIndex(controllerIndex);
	model->SetController(controller);

	model->SetNodePosition(initialPosition);
	world->AddNewNode(model);

	if (TheMessageMgr->GetSynchronizedFlag())
	{
		RigidBodyController *rigidBody = static_cast<RigidBodyController *>(controller);
		RigidBodyType type = rigidBody->GetRigidBodyType();

		if (type == kRigidBodyProjectile)
		{
			static_cast<ProjectileController *>(rigidBody)->EnterWorld(world, initialPosition);
		}
		else
            if (type == kRigidBodyCharacter)
		{
			static_cast<GameCharacterController *>(rigidBody)->EnterWorld(world, initialPosition);
		}
	}
    */
}

CreateModelMessage *CreateModelMessage::CreateMessage(ModelMessageType type)
{
	switch (type)
	{
		

		case kModelMessageSoldier:

			return (new CreateFighterMessage(type));
	}

	return (nullptr);
}




DeathMessage::DeathMessage() : Message(kMessageDeath)
{
}

DeathMessage::DeathMessage(PlayerKey player, PlayerKey attacker) : Message(kMessageDeath)
{
	playerKey = player;
	attackerKey = attacker;
}

DeathMessage::~DeathMessage()
{
}

void DeathMessage::CompressMessage(Compressor& data) const
{
	data << playerKey;
	data << attackerKey;
}

bool DeathMessage::DecompressMessage(Decompressor& data)
{
	data >> playerKey;
	data >> attackerKey;
	return (true);
}

bool DeathMessage::HandleMessage(Player *sender) const
{
		return (true);
}


ClientStyleMessage::ClientStyleMessage() : Message(kMessageClientStyle)
{
}

ClientStyleMessage::ClientStyleMessage(const int32 *style) : Message(kMessageClientStyle)
{
	for (machine a = 0; a < kPlayerStyleCount; a++)
	{
		playerStyle[a] = style[a];
	}
}

ClientStyleMessage::~ClientStyleMessage()
{
}

void ClientStyleMessage::CompressMessage(Compressor& data) const
{
	for (machine a = 0; a < kPlayerStyleCount; a++)
	{
		data << char(playerStyle[a]);
	}
}

bool ClientStyleMessage::DecompressMessage(Decompressor& data)
{
	for (machine a = 0; a < kPlayerStyleCount; a++)
	{
		char	style;

		data >> style;
		playerStyle[a] = style;
	}

	return (true);
}

bool ClientStyleMessage::HandleMessage(Player *sender) const
{
	PlayerStyleMessage styleMessage(sender->GetPlayerKey(), playerStyle);
	TheMessageMgr->SendMessage(kPlayerServer, styleMessage);
	TheMessageMgr->SendMessageClients(styleMessage, sender);
	return (true);
}


ClientOrientationMessage::ClientOrientationMessage() : Message(kMessageClientOrientation)
{
}

ClientOrientationMessage::ClientOrientationMessage(float azimuth, float altitude) : Message(kMessageClientOrientation, kMessageUnreliable)
{
	orientationAzimuth = azimuth;
	orientationAltitude = altitude;
}

ClientOrientationMessage::~ClientOrientationMessage()
{
}

void ClientOrientationMessage::CompressMessage(Compressor& data) const
{
	data << orientationAzimuth;
	data << orientationAltitude;
}

bool ClientOrientationMessage::DecompressMessage(Decompressor& data)
{
	data >> orientationAzimuth;
	data >> orientationAltitude;
	return (true);
}

bool ClientOrientationMessage::HandleMessage(Player *sender) const
{
	FighterController *controller = static_cast<GamePlayer *>(sender)->GetPlayerController();
	if (controller)
	{
		controller->UpdateOrientation(orientationAzimuth, orientationAltitude);
	}

	return (true);
}


ClientMovementMessage::ClientMovementMessage(MessageType type) : Message(type)
{
}

ClientMovementMessage::ClientMovementMessage(MessageType type, unsigned_int32 flag, float azimuth, float altitude) : Message(type)
{
	movementFlag = flag;
	movementAzimuth = azimuth;
	movementAltitude = altitude;
}

ClientMovementMessage::~ClientMovementMessage()
{
}

void ClientMovementMessage::CompressMessage(Compressor& data) const
{
	data << unsigned_int8(movementFlag);
	data << movementAzimuth;
	data << movementAltitude;
}

bool ClientMovementMessage::DecompressMessage(Decompressor& data)
{
	unsigned_int8	flag;

	data >> flag;
	movementFlag = flag;

	data >> movementAzimuth;
	data >> movementAltitude;

	return (true);
}

bool ClientMovementMessage::HandleMessage(Player *sender) const
{
	FighterController *controller = static_cast<GamePlayer *>(sender)->GetPlayerController();
	if (controller)
	{
		switch (GetMessageType())
		{
			case kMessageClientMovementBegin:
				controller->BeginMovement(movementFlag, movementAzimuth, movementAltitude);
				break;

			case kMessageClientMovementEnd:

				controller->EndMovement(movementFlag, movementAzimuth, movementAltitude);
				break;

			case kMessageClientMovementChange:

				controller->ChangeMovement(movementFlag, movementAzimuth, movementAltitude);
				break;
		}
	}

	return (true);
}


ClientFiringMessage::ClientFiringMessage(MessageType type) : Message(type)
{
}

ClientFiringMessage::ClientFiringMessage(MessageType type, float azimuth, float altitude) : Message(type)
{
	firingAzimuth = azimuth;
	firingAltitude = altitude;
}

ClientFiringMessage::~ClientFiringMessage()
{
}

void ClientFiringMessage::CompressMessage(Compressor& data) const
{
	data << firingAzimuth;
	data << firingAltitude;
}

bool ClientFiringMessage::DecompressMessage(Decompressor& data)
{
	data >> firingAzimuth;
	data >> firingAltitude;
	return (true);
}

bool ClientFiringMessage::HandleMessage(Player *sender) const
{
	
       FighterController *controller = static_cast<GamePlayer *>(sender)->GetPlayerController();
        if (controller) {
		controller->fireLaser();
        }

	return (true);
}



ClientMiscMessage::ClientMiscMessage(MessageType type) : Message(type)
{
}

ClientMiscMessage::~ClientMiscMessage()
{
}

bool ClientMiscMessage::HandleMessage(Player *sender) const
{
    /*
	GamePlayer *player = static_cast<GamePlayer *>(sender);

	switch (GetMessageType())
	{
		case kMessageClientSpawn:

			if ((player->GetDeathTime() < 0))
			{
				FighterController *controller = player->GetPlayerController();
				if (!controller)
				{

					if (TheMessageMgr->GetMultiplayerFlag())
					{
						TheGame->SpawnPlayer(player);
					}
					else
					{
						TheGame->RestartWorld();
					}
				}
			}

			break;
    }
*/

	return (true);
}



/*---------------------------------------------------*/



CreateCharacterMessage:: CreateCharacterMessage(MessageType type): Message(type)
{
}


CreateCharacterMessage:: CreateCharacterMessage(MessageType type,long i, EnemyType t,PlayerKey key, Point3D p,int _locatorIndex):Message(type)
{
    pos=p;
    index =i;
    chartype=t;
    ownerKey=key;
	locatorIndex = _locatorIndex;
    //printf("Create Charater Message index %d type %d ownkey %d\n",index,chartype,ownerKey);
}


CreateCharacterMessage::~CreateCharacterMessage()
{
}



void CreateCharacterMessage::CompressMessage(Compressor& data) const
{
	Message::CompressMessage(data);
    data << index;
    data << chartype;
    data << ownerKey;
    data << pos.x;
    data << pos.y;
    data << pos.z;
	data << locatorIndex;
}


bool CreateCharacterMessage::DecompressMessage(Decompressor& data)
{
    
    data >> index;
    data >> chartype;
    data >> ownerKey;
    data >> pos.x;
    data >> pos.y;
    data >> pos.z;
	data >> locatorIndex;
    
    return (true);
}

bool CreateCharacterMessage::HandleMessage(Player *sender) const
{
    //printf("Handle Charater Message index %d type %d ownkey %d \n",index,chartype,ownerKey);
    GameWorld *world = static_cast<GameWorld *>(TheWorldMgr->GetWorld());
    world->AddOjectAtLocation(pos,chartype,index,ownerKey,locatorIndex);
    return(true);
    
}
/*-----------------------*/



ClientRequestMessage::ClientRequestMessage(MessageType type) : Message(type)
{
}

ClientRequestMessage::ClientRequestMessage(MessageType type, const long data ) : Message(type)
{
    myData=data;
}

ClientRequestMessage::~ClientRequestMessage()
{
}

void ClientRequestMessage::CompressMessage(Compressor& data) const
{
    data<<myData;
}

bool ClientRequestMessage::DecompressMessage(Decompressor& data)
{
    
    data >> myData;
    return (true);
}


bool ClientRequestMessage::HandleMessage(Player *sender) const
{
    //printf("Recieved ClientRequestMessage \n");
    
    //switch (GetMessageType()){
      //  case  kMessageTypeRequestAvantar:{
            //printf("ClientRequestMessage::  Requesting Avatar Player Key %d  \n",sender->GetPlayerKey());
            //TODO: check this
            Point3D pos;
            GameWorld *world = static_cast<GameWorld *>(TheWorldMgr->GetWorld());
			Marker *marker = world->GetSpawnLocator(0);
			pos = marker->GetWorldPosition();
            world->ReqestOjectAtLocation(pos,kSoldierEntity,sender->GetPlayerKey(),-1);
        //}
    //}
			return true; //LB: added to avoid a no return error
}



TurretCreationMessage::TurretCreationMessage(MessageType type) : Message(type)
{
}

TurretCreationMessage::TurretCreationMessage(MessageType type, Point3D newPosition, ObjectType newObjectType, int32 newLocatorIndex, int32 index, unsigned_int32 flags) : Message(type)
{
	position = newPosition;
	objectType = newObjectType;
	locatorIndex = newLocatorIndex;
}

TurretCreationMessage::~TurretCreationMessage()
{
}

void TurretCreationMessage::CompressMessage(Compressor& data) const
{
	data << position.x;
	data << position.y;
	data << position.z;
	data << objectType;
	data << locatorIndex;
}

bool TurretCreationMessage::DecompressMessage(Decompressor& data)
{
	data >> position.x;
	data >> position.y;
	data >> position.z;
	data >> objectType;
	data >> locatorIndex;
	return (true);
}

bool TurretCreationMessage::HandleMessage(Player *sender) const {
	GameWorld *world = static_cast<GameWorld *>(TheWorldMgr->GetWorld());
	world->ReqestOjectAtLocation(position, objectType, playerKey, locatorIndex); //TODO: verify it should be ReqestOjectAtLocation
	return(true);
}


ClientRequestWorldControllersMessage::ClientRequestWorldControllersMessage(MessageType type) : Message(type)
{
}

ClientRequestWorldControllersMessage::ClientRequestWorldControllersMessage(MessageType type, PlayerKey newPlayerKey, unsigned_int32 flags) : Message(type)
{
	playerKey = newPlayerKey;
}

ClientRequestWorldControllersMessage::~ClientRequestWorldControllersMessage()
{
}

void ClientRequestWorldControllersMessage::CompressMessage(Compressor& data) const
{
	data << playerKey;
}

bool ClientRequestWorldControllersMessage::DecompressMessage(Decompressor& data)
{
	data >> playerKey;
	return (true);
}

bool ClientRequestWorldControllersMessage::HandleMessage(Player *sender) const {
	GameWorld *world = static_cast<GameWorld *>(TheWorldMgr->GetWorld());
	world->SendExistingControllersForNewClientToPopulate(playerKey);
	return(true);
}

// ZUXSVMT
