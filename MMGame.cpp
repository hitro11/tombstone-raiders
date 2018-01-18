
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


#include "TSToolWindows.h"
#include "TSAudioCapture.h"
#include "TSWater.h"

#include "MMGame.h"
#include "MMMultiplayer.h"
#include "MMCameras.h"
#include "MMGameWorld.h"
#include "MMFighter.h"
#include "Timer.hpp"
#include <stdio.h>


using namespace MMGame;


Game *MMGame::TheGame = nullptr;


Game::Game() :
	Global<Game>(TheGame),

	controllerCreator(&CreateController),
	mmControllerReg(kControllerTest, "MMTest"),
	t2BoxControllerReg(Tombstone::kControllerBox, "Box Controller"),
	customSpinControlerReg(kControllerCustomSpin, "Custom Spin Controller"),
	//goblinEnemyControllerReg(kControllerGoblinEnemy, "Goblin Enemy Controller"),

	spawnLocatorRegistration(kLocatorSpawn, "Spawn Location"),
	collLocatorRegistration(kLoactorColl, "Coll Mark"),
	physLocatorRegistration(kLocatorPys, "Phys Mark"),
	evadeLocatorRegistration(kLocatorAttack, "Attack En"),
	attackLocatorRegistration(kLocatorEvade, "Evade En"),
	randLocatorRegistration(kLocatorRand, "Rand En"),
	npcSpawnLocatorRegistration(kNPCLocatorSpawn, "NPCSpawn Location"),
	baseLocatorRegistration(kLocatorBase, "Base Locator"),
	basicTurretLocatorRegistration(kLocatorBasicTurret, "Basic Turret Locator"),

	soldierModelReg(kModelSoldier, nullptr, "soldier/Soldier", kModelPrecache | kModelPrivate, kControllerSoldier),
	appleModelRegistration(kModelApple, nullptr, "health/Apple", 0, kControllerCollectable),
	//soldierModelReg(kModelSoldier, nullptr, "GUS/gus", kModelPrecache | kModelPrivate, kControllerSoldier),
	npcPyramidModelRegistration(kModelNPCPyramid, nullptr, "goblin/Goblin", 0, kControllerNPCPyramid),
	ballModelReg(kModelBall, "Bouncing Ball", "model/Ball2", kModelPrecache, kControllerBall),
	controllerReg(kControllerBall, "Bouncing Ball"),
	sparkParticleSystemReg(kParticleSystemSpark, "Sparks"),
	//burstParticleSystemReg(MMGame::kParticleSystemBurst, "Bursts"),

	turretControllerReg(kControllerTurret, "Turret"),
	turretModelReg(kModelTurret, "Turret", "Models/Turret", kModelPrecache, kControllerTurret),

	baseControllerReg(kControllerBase, "Base"),
	baseModelRegistration(kModelBase, "Base", "Models/tower_main_blue", kModelPrecache, kControllerBase),

	//goblinControllerRegistration(kControllerGoblin, "Goblin Enemy Controller"),
	goblinModelRegistration(kModelGoblin, nullptr, "goblin/Goblin", 0, kControllerGoblin),

	hostCommandObserver(this, &Game::HandleHostCommand),
	joinCommandObserver(this, &Game::HandleJoinCommand),


	hostCommand("host", &hostCommandObserver),
	joinCommand("join", &joinCommandObserver),

	forwardAction(kActionForward, kMovementForward, kSpectatorMoveForward),
	backwardAction(kActionBackward, kMovementBackward, kSpectatorMoveBackward),
	leftAction(kActionLeft, kMovementLeft, kSpectatorMoveLeft),
	rightAction(kActionRight, kMovementRight, kSpectatorMoveRight),
	upAction(kActionUp, kMovementUp, kSpectatorMoveUp),
	downAction(kActionDown, kMovementDown, kSpectatorMoveDown),
	movementAction(kActionMovement, 0, 0),
	primaryFireAction(kActionFirePrimary),
	buildDefenceAction(kActionFlashlight)

{
	//TheDisplayMgr->InstallDisplayEventHandler(&displayEventHandler);

	TheEngine->AddCommand(&hostCommand);
	TheEngine->AddCommand(&joinCommand);
	


	TheInputMgr->AddAction(&forwardAction);
	TheInputMgr->AddAction(&backwardAction);
	TheInputMgr->AddAction(&leftAction);
	TheInputMgr->AddAction(&rightAction);
	TheInputMgr->AddAction(&upAction);
	TheInputMgr->AddAction(&downAction);
	TheInputMgr->AddAction(&movementAction);
	TheInputMgr->AddAction(&primaryFireAction);
    TheInputMgr->AddAction(&testAction);
	TheInputMgr->AddAction(&buildDefenceAction);

	prevEscapeCallback = TheInputMgr->GetEscapeCallback();
	prevEscapeCookie = TheInputMgr->GetEscapeCookie();
	TheInputMgr->SetEscapeCallback(&EscapeCallback, this);

	TheWorldMgr->SetWorldCreator(&CreateWorld);

	TheMessageMgr->SetPlayerCreator(&CreatePlayer);

	gameFlags = 0;

	currentWorldName[0] = 0;


	TheInterfaceMgr->SetInputManagementMode(kInputManagementAutomatic);
    
    TheTimerMgr = new TimerMgr();


}


Game::~Game()
{
	ExitCurrentGame();
    //boardList.PurgeList();

	TheWorldMgr->SetWorldCreator(nullptr);
	TheMessageMgr->SetPlayerCreator(nullptr);

	TheInputMgr->SetEscapeCallback(prevEscapeCallback, prevEscapeCookie);
}

World *Game::CreateWorld(const char *name, void *cookie)
{
	return (new GameWorld(name));
}

Player *Game::CreatePlayer(PlayerKey key, void *cookie)
{
	return (new GamePlayer(key));
}

Controller *Game::CreateController(Unpacker& data)
{
	switch (data.GetType())
	{
	}

	return (nullptr);
}



void Game::EscapeCallback(void *cookie)
{
}





void Game::InitPlayerStyle(int32 *style)
{
}

void Game::HandleHostCommand(Command *command, const char *text)
{
	if (*text != 0)
	{
		ResourceName	name;

		Text::ReadString(text, name, kMaxResourceNameLength);
		HostMultiplayerGame(name, 0);
	}
}


void Game::StartSinglePlayerGame(const char *name)
{
    ExitCurrentGame();
    
    currentWorldName = name;
    
    HostMultiplayerGame(name,0);
    
    //World *world = new GameWorld(name);
}


// I hard coded this, so it connects to the local machine
void Game::HandleJoinCommand(Command *command, const char *text)
{
    printf(" Joining a Game \n");
    TheMessageMgr->BeginMultiplayerGame(false);
    
    //String *addressText=MessageMgr::AddressToString(TheNetworkMgr->GetLocalAddress());
    
    // We'll first want to provide the user with some feedback - so he'll know what he's doing.
    String<128> str("Attempting to join --> ");
    str += text;
    
    NetworkAddress address = MessageMgr::StringToAddress(text);
    if (address.GetPort() == 0)address.SetPort(kGamePort);

    
    TheEngine->Report(str, kReportError);
    TheNetworkMgr->SetPortNumber(kGamePort);
    TheNetworkMgr->SetBroadcastPortNumber(kGamePort);
    TheNetworkMgr->Initialize();
    
    // Now we're just going to (try to) connect to the entered address.
    NetworkAddress local_addr = TheNetworkMgr->GetLocalAddress();
    local_addr.SetPort(kGamePort);
    
    //TheMessageMgr->Connect(local_addr);
    TheMessageMgr->Connect(address);
}



EngineResult Game::HostMultiplayerGame(const char *name, unsigned_int32 flags)
{
	ExitCurrentGame();
    
    printf("Start Multi Player Game \n");

	//TheNetworkMgr->SetProtocol(kGameProtocol);
	TheNetworkMgr->SetPortNumber(kGamePort);
	TheNetworkMgr->SetBroadcastPortNumber(kGamePort);
    TheNetworkMgr->Initialize();

	EngineResult result = TheMessageMgr->BeginMultiplayerGame(true);
	if (result == kNetworkOkay)
	{
		result = TheWorldMgr->LoadWorld(name);
		if (result == kWorldOkay)
		{
            
            printf("World Loaded \n");
            GameWorld *world = static_cast<GameWorld *>(TheWorldMgr->GetWorld());
			//multiplayerFlags = flags;

			GamePlayer *player = static_cast<GamePlayer *>(TheMessageMgr->GetLocalPlayer());
			//unsigned_int32 playerFlags = (flags & kMultiplayerDedicated) ? kPlayerInactive : 0;
            
			//player->SetPlayerFlags(playerFlags);
            const Point3D pos= Point3D(0.0F, 0.0F, 2.0F);
            
            // Request an Avatar  ONLY ON SERVER !!!!!
            
            world->ReqestOjectAtLocation(pos ,kSoldierEntity,player->GetPlayerKey(),-1);
            
			// POPULATE ONLY ON SERVER !!!
            world->PopulateWorld();
            
             DisplayBoard::OpenBoard();
//            TheDisplayBoard->ShowMessageText("Server");
		}
	}

	return (result);
}

EngineResult Game::JoinMultiplayerGame(const char *name, unsigned_int32 flags)
{

	EngineResult result = TheWorldMgr->LoadWorld(name);
	if (result == kWorldOkay)
	{

        printf("Joinging Game in word %s \n",name);
		//GamePlayer *player = static_cast<GamePlayer *>(TheMessageMgr->GetPlayer(kPlayerServer));
		//if (flags & kMultiplayerDedicated)
		{
			//player->SetPlayerFlags(player->GetPlayerFlags() | kPlayerInactive);
		}
        DisplayBoard::OpenBoard();
		//TheDisplayBoard->ShowMessageText("Client");
	}

	return (result);
}

void Game::ExitCurrentGame(void)
{

	TheMessageMgr->EndGame();
    delete TheDisplayBoard;

	gameFlags = 0;

	TheWorldMgr->UnloadWorld();

}

void Game::RestartWorld(void)
{
	GameWorld *world = static_cast<GameWorld *>(TheWorldMgr->GetWorld());
	if ((world) && (world->GetSpawnLocatorCount() != 0))
	{
		StartSinglePlayerGame(currentWorldName);
	}
}

EngineResult Game::LoadWorld(const char *name)
{
	StartSinglePlayerGame(name);
	return (kEngineOkay);
}

void Game::UnloadWorld(void)
{
	ExitCurrentGame();
	
}



void Game::HandleConnectionEvent(ConnectionEvent event, const NetworkAddress& address, const void *param)
{
    printf("HandleConnectionEvent\n");
    switch (event)
    {
        case kConnectionQueryReceived:
        {
            World *world = TheWorldMgr->GetWorld();
            if (world){
                printf("SENDING WORLD NAME \n");
                const char *gameName = TheEngine->GetVariable("gameName")->GetValue();
                const char *worldName = world->GetWorldName();
                
                ServerInfoMessage message(TheMessageMgr->GetPlayerCount(), TheMessageMgr->GetMaxPlayerCount(), gameName, worldName);
                TheMessageMgr->SendConnectionlessMessage(address, message);
            }
            
            break;
        }
            
        case kConnectionAttemptFailed:
            printf("kConnectionAttemptFailed\n");
            
            // The server rejected our connection.
            
            
            break;
            
        case kConnectionServerAccepted:
            
            // The server accepted our connection.
            printf("kConnectionServerAccepted\n");
            
            
            
            // Tell the server what player styles the user has set.
            // The server will forward this information to the other players.
            
            //TheMessageMgr->SendMessage(kPlayerServer, ClientStyleMessage(static_cast<GamePlayer *>(TheMessageMgr->GetLocalPlayer())->GetPlayerStyle()));
            break;
            
        case kConnectionServerClosed:
            
            // The server was shut down.
            printf("closed \n");
            
            ExitCurrentGame();
            break;
            
        case kConnectionServerTimedOut:
            
            // The server has stopped responding.
            
            ExitCurrentGame();
            break;
    }
    
    Application::HandleConnectionEvent(event, address, param);
}

void Game::HandlePlayerEvent(PlayerEvent event, Player *player, const void *param)
{
    printf("HandlePlayerEvent\n");
    switch (event)
    {
        case kPlayerConnected:
        {
            printf("kPlayerConnected\n");
            
            if (TheMessageMgr->GetSynchronizedFlag())
            {
				// NOTHING TO DO
            }
            
            if (TheMessageMgr->GetServerFlag())
            {
                /*
                 GamePlayer *gamePlayer = static_cast<GamePlayer *>(TheMessageMgr->GetFirstPlayer());
                 while (gamePlayer)
                 {
                 if ((gamePlayer != player) && (gamePlayer->GetPlayerFlags() & kPlayerReceiveVoiceChat))
                 {
                 new Channel(player, gamePlayer);
                 }
                 
                 gamePlayer = gamePlayer->GetNextPlayer();
                 }
                 */
            }
            
            break;
        }
            
        case kPlayerDisconnected:
        {
             printf("kPlayerDisconnected\n");
            Controller *controller = static_cast<GamePlayer *>(player)->GetPlayerController();
            if (controller)
            {
                //delete controller->GetTargetNode();
            }
            
			//Remove Player from PlayerKeyList
			//TODO: !!verify that the player doesn't have to be removed from the list in other scenarios (i.e. timeout
			GameWorld *world = static_cast<GameWorld *>(TheWorldMgr->GetWorld());
			world->RemovePlayerKeyFromListByKey(player->GetPlayerKey());

            break;
        }
            
        case kPlayerTimedOut:
        {
            printf("kPlayerTimedOut\n");
            Controller *controller = static_cast<GamePlayer *>(player)->GetPlayerController();
            if (controller)
            {
                //delete controller->GetTargetNode();
            }
            
            break;
        }
            
        case kPlayerInitialized:
        {
            
            printf("kPlayerInitialized\n");
            // A new player joining the game has been initialized. For each player already
            // in the game, send a message  TO constrcut the coresponding Avatar
            
            const GamePlayer *gamePlayer = static_cast<GamePlayer *>(TheMessageMgr->GetFirstPlayer());
            do
            {
                if (gamePlayer != player)
                {
					// NOTHING TO DO
                }
                
                gamePlayer = gamePlayer->GetNextPlayer();
            } while (gamePlayer);
            
            // Now tell the new player what world is being played.
            
            World *world = TheWorldMgr->GetWorld();
            printf("Server Send Word Name \n");
            if (world) player->SendMessage(GameInfoMessage(0,world->GetWorldName()));
            
            break;
        }
            
            
    }
    
    Application::HandlePlayerEvent(event, player, param);
}

// THIS IS CALLED WHEN ALL THE LOADING ON THE CLIENT IS DONE
void Game::HandleGameEvent(GameEvent event, const void *param)
{
    printf("HandleGameEvent\n");
    switch (event)
    {
        case kGameSynchronized:
            printf("Game Syncronized request avatar\n");
            ClientRequestMessage message(kMessageRequestAvantar,0);
            TheMessageMgr->SendMessage(kPlayerServer,message);
			
			//Request Missing Controllers from world
			//NOTE: Removed this code because it should be handled in the SendInitialStateMessage method for the controller
			/*
			printf("Request Missing Controllers in World\n");
			GamePlayer *player = static_cast<GamePlayer *>(TheMessageMgr->GetLocalPlayer());
			ClientRequestWorldControllersMessage clientRequestMessage(kMessageClientRequestWorldControllers, player->GetPlayerKey(), 0);
			TheMessageMgr->SendMessage(kPlayerServer, clientRequestMessage);
			*/
            break;
    }
}

// Add all messages so they can be send over the network !!
Message *Game::CreateMessage(MessageType type, Decompressor& data) const
{
    switch (type)
    {
        case kMessageServerInfo:
            
            return (new ServerInfoMessage);
            
        case kMessageGameInfo:
            
            return (new GameInfoMessage);
            
        case kMessagePlayerStyle:
            
            return (new PlayerStyleMessage);
            
        case kMessageCreateModel:
        {
            unsigned_int8	modelType;
            
            data >> modelType;
            return (CreateModelMessage::CreateMessage(modelType));
        }
            
            
        case kMessageDeath:
            
            return (new DeathMessage);
            
            
        case kMessageClientStyle:
            
            return (new ClientStyleMessage);
            
        case kMessageClientOrientation:
            
            return (new ClientOrientationMessage);
            
        case kMessageClientMovementBegin:
        case kMessageClientMovementEnd:
        case kMessageClientMovementChange:
            
            return (new ClientMovementMessage(type));
            
        case kMessageClientFiringPrimaryBegin:
        case kMessageClientFiringEnd:
            
            return (new ClientFiringMessage(type));
            
        case kMessageCreateCharacter:
            return(new CreateCharacterMessage(type));
            
        case kMessageRequestAvantar:
            
            return(new ClientRequestMessage(type));
		
		case kMessageTurretCreation:
			return(new TurretCreationMessage(type));
        
		case kMessageClientRequestWorldControllers:
			return(new ClientRequestWorldControllersMessage(type));
            
    }
    
    return (nullptr);
}

void Game::ReceiveMessage(Player *sender, const NetworkAddress& address, const Message *message)
{
    
}

void Game::SpawnPlayer(Player *player)
{
    GameWorld *world = static_cast<GameWorld *>(TheWorldMgr->GetWorld());
    if (world)
    {
        int32 count = world->GetSpawnLocatorCount();
        if (count != 0)
        {
            //const Marker *marker = world->GetSpawnLocator(Math::RandomInteger(count));
            
            //const Vector3D direction = marker->GetWorldTransform()[0];
            //float azimuth = Arctan(direction.y, direction.x);
            
            //int32 fighterIndex = world->NewControllerIndex();
            //int32 weaponIndex = world->NewControllerIndex();
            //TheMessageMgr->SendMessageAll(CreateGusGravesMessage(fighterIndex, marker->GetWorldPosition(), azimuth, //0.0F, 0, kWeaponPistol, weaponIndex, player->GetPlayerKey()));
            
        }
    }
}

void Game::ApplicationTask(void)
{
    TheTimerMgr->tick();        // execute all timer tasks
    
}

