//
//  MMGameWorld.cpp
//
//  Created by Martin on 2016-10-04.
//
//

#include "MMGameWorld.h"

#include "MMMultiPLayer.h"
#include "MMGamePlayer.h"
#include "MMFighter.h"

#include "MMColectCont.h"

#include "EnemyController.h"

#include "MMGame.h"
#include <stdio.h>






using namespace MMGame;




GameWorld::GameWorld(const char *name) :
World(name),
spectatorCamera(1.,1.0F, 0.3F)
{
}

GameWorld::~GameWorld()
{
    
}

WorldResult GameWorld::PreprocessWorld(void)
{
    WorldResult result = World::PreprocessWorld();
    if (result != kWorldOkay)
    {
        return (result);
    }
    
    SetWorldCamera(&spectatorCamera);
    playerCamera = &firstPersonCamera;
    
    spawnLocatorCount = 0;
	npcSpawnLocatorCount = 0;
    collLocatorCount = 0;
    AiLocatorCount = 0;
	baseLocatorCount = 0;
	basicTurretLocatorCount = 0;
    CollectZoneMarkers(GetRootNode());
	InitializePlayerKeyArray();
    printf("WE found %d coll Locators \n",collLocatorCount);
    
    {
        spectatorCamera.SetNodePosition(Point3D(0.0F, 0.0F, 1.0F));
    }
    
    bloodIntensity = 0.0F;

    return (kWorldOkay);
}

void GameWorld::CollectZoneMarkers(Zone *zone)
{
    Marker *marker = zone->GetFirstMarker();
    while (marker)
    {
        Marker *next = marker->GetNextListElement();
        
        if ((marker->GetMarkerType() == kMarkerLocator) && (marker->NodeEnabled()))
        {
            LocatorMarker *locator = static_cast<LocatorMarker *>(marker);
            switch (locator->GetLocatorType())
            {
                case kLocatorSpawn:
                    spawnLocatorCount++;
                    spawnLocatorList.AppendListElement(locator);
                    break;
                case kLoactorColl:
                    collLocatorCount++;
                    collLocatorList.AppendListElement(locator);
                    break;
                case kLocatorPys:
                case kLocatorAttack:
                case kLocatorEvade:
                case kLocatorRand:
                    AiLocatorCount++;
                    AiLocatorList.AppendListElement(locator);
					break;
				case kNPCLocatorSpawn:
					npcSpawnLocatorCount++;
					npcSpawnLocatorList.AppendListElement(locator);
					break;
				case kLocatorBase:
					baseLocatorCount++;
					baseLocatorList.AppendListElement(locator);
					break;
				case kLocatorBasicTurret:
					basicTurretLocatorCount++;
					basicTurretLocatorList.AppendListElement(locator);
					break;
            }
        }
        
        marker = next;
    }
    
    Zone *subzone = zone->GetFirstSubzone();
    while (subzone)
    {
        CollectZoneMarkers(subzone);
        subzone = subzone->GetNextListElement();
    }
}

RigidBodyStatus GameWorld::HandleNewRigidBodyContact(RigidBodyController *rigidBody, const RigidBodyContact *contact, RigidBodyController *contactBody)
{
    return (kRigidBodyUnchanged);
}

/*
 RigidBodyStatus GameWorld::HandleNewGeometryContact(RigidBodyController *rigidBody, const GeometryContact *contact)
 {
 
 
 
	return (kRigidBodyUnchanged);
 }
 */

void GameWorld::DetectInteractions(void)
{
    
    
    World::DetectInteractions();
}

void GameWorld::BeginRendering(void)
{
    World::BeginRendering();
}

void GameWorld::EndRendering(FrameBuffer *frameBuffer)
{
    World::EndRendering(frameBuffer);
    
    
}

void GameWorld::SetCameraTargetModel(Model *model)
{
    firstPersonCamera.SetTargetModel(model);
    chaseCamera.SetTargetModel(model);
    SetWorldCamera(playerCamera);
    
}

void GameWorld::SetSpectatorCamera(const Point3D& position, float azm, float alt)
{
    firstPersonCamera.SetTargetModel(nullptr);
    chaseCamera.SetTargetModel(nullptr);
    SetWorldCamera(&spectatorCamera);
    spectatorCamera.SetNodePosition(position);
    spectatorCamera.SetCameraAzimuth(azm);
    spectatorCamera.SetCameraAltitude(alt);
}

void GameWorld::SetLocalPlayerVisibility(void)
{
    
}

void GameWorld::ChangePlayerCamera(void)
{
    const Player *player = TheMessageMgr->GetLocalPlayer();
    if ((player) && (static_cast<const GamePlayer *>(player)->GetPlayerController()))
    {
        if (playerCamera == &firstPersonCamera)
        {
            playerCamera = &chaseCamera;
        }
        else
        {
            playerCamera = &firstPersonCamera;
        }
        
        
        SetWorldCamera(playerCamera);
        SetLocalPlayerVisibility();
    }
}

void GameWorld::SetFocalLength(float focal)
{
    spectatorCamera.GetObject()->SetFocalLength(focal);
    firstPersonCamera.GetObject()->SetFocalLength(focal);
    chaseCamera.GetObject()->SetFocalLength(focal);
}

/*------------------------------------------*/

// THis is an "NICE" way to convert constants into strings for debugging !!

#define TYPE_NAME(type) \
(kSoldierEntity       == type ? "kSoldierEntity"    :  \
(kCollectEntity     == type ? "kCollectEntity"   :  \
(kPhysEntiy   == type ? "kPhysEntiy"  : \
(kPhysAttack   == type ? "kPhysAttack"  : \
(kPhysEvade   == type ? "kPhysEvade"  : \
(kPhysRand   == type ? "kPhysRnd"  : \
(kNPCEntity == type? "kNPCEntity"  : \
(kBaseTurretEntity == type? "kBaseTurretEntity"  : \
(kBaseEntity == type? "kBaseEntity"  : \
(0 == type ? "yellow" : "unknown"))))))))))


/*----------------------------------------------------------------------------------*/

/// HERE ARE THE FUNCTIONS FOR RUNTIME DISTRIBUTED SCENE GRAPH MANAGEMENT
// ADDING ANYHTING TO THE SCENE GRAPH WE REQUEST IT AND THEN A MESSAGE IS SEND TO ALL
// PARTICIPANTS TO ADD IT !


void GameWorld::ReqestOjectAtLocation(const Point3D& pos ,ObjectType type,PlayerKey key, int locatorIndex)
{
    
    // NO CALLS TP THIS BUT ON SERVER !!!!!
    
    // VERY IMPROTANT !! THIS IS THE UNIQUE CONTROLLER INDEX
    long cIndex=TheWorldMgr->GetWorld()->NewControllerIndex();
    //PlayerKey key=TheMessageMgr->GetLocalPlayer()->GetPlayerKey();
    
    
    printf("Request Oject: Index %d Type %s key %d \n",cIndex,TYPE_NAME(type),key);
	if (type == kSoldierEntity) {
		printf("THis is an Avatar, Spwn a spawn point \n");
		GameWorld *world = static_cast<GameWorld *>(TheWorldMgr->GetWorld());
		int spawnLocatorIndex = GetNextSpawnPointIndex();
		AddPlayerKeyToList(key);
		const Marker *marker = world->GetSpawnLocator(spawnLocatorIndex);

		CreateCharacterMessage  message(kMessageCreateCharacter, cIndex, type, key, marker->GetWorldPosition(),-1);
		TheMessageMgr->SendMessageAll(message, true);
	}
	else if (type == kNPCEntity) {
		//DG: This is not the problem code
		//DG: collect markers first then specify location after
		//DG: Players will see all of the elements in SendInitialStateMessage in the controller
		printf("This is a pyramid NPC, Spawn at NPC spawn point \n");
		//GameWorld *world = static_cast<GameWorld *>(TheWorldMgr->GetWorld());
		//const Marker *marker = world->GetNPCSpawnLocator(0);
		
		CreateCharacterMessage  message(kMessageCreateCharacter, cIndex, type, key,pos,locatorIndex);
		TheMessageMgr->SendMessageAll(message, true);
	}
	else if (type == kBaseEntity) {
		CreateCharacterMessage  message(kMessageCreateCharacter, cIndex, type, key, pos, locatorIndex);
		TheMessageMgr->SendMessageAll(message, true);
	}
    else{
        CreateCharacterMessage  message(kMessageCreateCharacter,cIndex,type,key,pos,locatorIndex);
        TheMessageMgr->SendMessageAll(message,true);
    }
    
 }



/*----------------------------------------------------------------------------------*/

Controller* GameWorld::CreateAvatar(const Point3D& pos ,long index,PlayerKey key)
{
    
    GameWorld *world = static_cast<GameWorld *>(TheWorldMgr->GetWorld());
    //const Marker *locator = world->GetSpawnLocator(0);
    
    
    // Calculate the angle corresponding to the direction the character is initially facing.
    //const Vector3D direction = locator->GetWorldTransform()[0];
    //float azimuth = atan2(direction.y, direction.x);
    
    
    Model *model = Model::GetModel(kModelSoldier);
    FighterController *contr = new FighterController(kSoldierEntity); //changed this from 12 to kSoldierEntity
    model->SetController(contr);
    
    // SET INDEX AND KEY !!! AND FOR THE PLAYER !!!!!!!!!!!!
    contr->SetPlayerKey(key);
    GamePlayer* player = static_cast<GamePlayer *>(TheMessageMgr->GetPlayer(key));
    player->SetPlayerController(contr, nullptr);
    
    
    // NOW WE Have to set the camara to this avantar if it is the local!
    if(key==TheMessageMgr->GetLocalPlayer()->GetKey()){
        
        printf("This is my player, Set Camara \n");
        
        
        world->SetCameraTargetModel(model);
        world->ChangePlayerCamera();
        
    }
    
    // I Do nto need these here actually
    contr->SetControllerIndex(index);
    model->SetNodePosition(pos);
    world->AddNewNode(model);

    return(contr);
}


/*----------------------------------------------------------------------------------*/


void GameWorld::AddOjectAtLocation(const Point3D& pos ,ObjectType type,long index,PlayerKey key,int locatorIndex)
{
    
    //printf("CREATE object of type %s with index %d for player key %d \n",TYPE_NAME(type),index,key);
    printf("CREATE  Oject: Index %d Type %s key %d \n",index,TYPE_NAME(type),key);
    
    // Create the controller and the model and put it together
    Controller* controller;
    Model *model;
    
    
    // TMP
    //if (key =! TheMessageMgr->GetLocalPlayer())return;
    
    GameWorld *world = static_cast<GameWorld *>(TheWorldMgr->GetWorld());
    
    switch(type){
    case kSoldierEntity:{
        // SPECIAL CASE !!!!
        CreateAvatar(pos , index, key);
        return;
    }
	// COLELCT OBJECT
	case kCollectEntity: {
            controller=new CollectableController();
             model = Model::NewModel("pumpkin/pumpkin/Pumpkin-A");
            // WE CAN LOAD MODELS AT RUNTIM TOO
            //model = Model::NewModel("models/model1");
            break;
    }
	case kNPCEntity: {
		controller = new GoblinEnemy(locatorIndex);
		if (TheMessageMgr->GetServerFlag()) {
			EnemyController *eController = static_cast<EnemyController *>(controller);
			enemyControllerList.AppendListElement(eController);
		}
		model = Model::GetModel(kModelNPCPyramid);
		break;
	}
	case kBaseTurretEntity: {
		controller = new TurretController();
		if (TheMessageMgr->GetServerFlag()) {
			TurretController *tController = static_cast<TurretController *>(controller);
			turretControllerList.AppendListElement(tController);
		}
		numTurretsOnMap++;
		model = Model::GetModel(kModelTurret);
		break;
	}
	case kBaseEntity: {
		controller = new BaseController();
		if (TheMessageMgr->GetServerFlag()) {
			
			BaseController *bController = static_cast<BaseController *>(controller);
			bController->SetPlayerPosition(baseCounter);
			baseControllerList.AppendListElement(bController);
			//NOTE: Can put this here because we only care about what the value is on the server as it handles damage and GameOver functionality
			baseCounter++; //ensure baseCounter does not exceed 4
		}
		numBasesOnMap++;
		model = Model::GetModel(kModelBase); //TODO: switch turret colour based on base number
		break;
	}
    default:
            printf("Unknown Object Type \n");
            return;

    }
    
    // SET THE CONTROLLER KEY
    model->SetController(controller);
    controller->SetControllerIndex(index);
    
    // Put the model in the world at the locator's position.
    
    Node* node=controller->GetTargetNode();
    node->SetNodePosition(pos);
    world->AddNewNode(node);
    
}

/*----------------------------------------------------------*/

void GameWorld::PopulateWorld(void)
{
    printf("Populating World \n");
	
	PopulateTurrets();
	PopulateBases();

	// Collect Items
	for(int i=0;i<collLocatorCount;i++){
		ReqestOjectAtLocation(collLocatorList[i]->GetNodePosition(),kCollectEntity,-1,-1);
	}
}

void GameWorld::PopulateTurrets() {
	for (int i = 0; i < basicTurretLocatorCount; i++) {
		ReqestOjectAtLocation(basicTurretLocatorList[i]->GetNodePosition(), kBaseTurretEntity, -1, -1);
	}
}

void GameWorld::PopulateBases() {
	for (int i = 0; i < baseLocatorCount; i++) {
		ReqestOjectAtLocation(baseLocatorList[i]->GetNodePosition(), kBaseEntity, -1, -1);
	}
}

void GameWorld::SendExistingControllersForNewClientToPopulate(PlayerKey playerKey) {
	
	//Send Enemy controllers
	/*for (int i = 0; i < numEnemiesOnMap; i++) {
		EnemyController *enemy = enemyControllerList[i];
		EnemyType type = kNPCEntity; //TODO: set this to appropriate enemy type as a property of the enemyController
		int index = enemy->GetSpawnLocatorIndex();
		CreateCharacterMessage  message(kMessageCreateCharacter, enemy->GetControllerIndex(), type, kPlayerServer, enemy->GetPosition(), enemy->GetSpawnLocatorIndex());
		TheMessageMgr->SendMessage(playerKey, message);
	}*/
	
	//Send Turret controllers
	for (int i = 0; i < numTurretsOnMap; i++) {
		TurretController *turret = turretControllerList[i];
		EnemyType type = kBaseTurretEntity;
		CreateCharacterMessage  message(kMessageCreateCharacter, turret->GetControllerIndex(), type, kPlayerServer, turret->GetPosition(), -1);
		//Change to Turret Creation Message
		TheMessageMgr->SendMessage(playerKey, message);
	}

	//Send Base controllers
	for (int i = 0; i < numBasesOnMap; i++) {
		BaseController *base = baseControllerList[i];
		EnemyType type = kBaseEntity;
		CreateCharacterMessage  message(kMessageCreateCharacter, base->GetControllerIndex(), type, kPlayerServer, base->GetPosition(), -1);
		TheMessageMgr->SendMessage(playerKey, message);
	}
	
}

void GameWorld::InitializePlayerKeyArray() {
	for (int i = 0; i < sizeof(playerKeyArray); i++) {
		playerKeyArray[i] = 0;
	}
}

int32 GameWorld::GetNextSpawnPointIndex() {
	for (int i = 0; i < sizeof(playerKeyArray); i++) {
		if (playerKeyArray[i] == 0) {
			return i;
		}
	}
	return -1;
}

void GameWorld::AddPlayerKeyToList(PlayerKey key) {
	for (int i = 0; i < sizeof(playerKeyArray); i++) {
		if (playerKeyArray[i] == 0) {
			playerKeyArray[i] = key;
			break;
		}
	}
}

void GameWorld::RemovePlayerKeyFromListByIndex(int index) {
	if (index < sizeof(playerKeyArray)) {
		playerKeyArray[index] = 0;
	}
}

void GameWorld::RemovePlayerKeyFromListByKey(PlayerKey key) {
	for (int i = 0; i < sizeof(playerKeyArray); i++) {
		if (playerKeyArray[i] == key) {
			playerKeyArray[i] = 0;
			break;
		}
	}
}


/*-----------------------------------*/


FighterController* GameWorld::GetLocalAvantar(void)
{
        GamePlayer* player = static_cast< GamePlayer *>(TheMessageMgr->GetLocalPlayer());
        return(player->GetPlayerController());
}

#define k_FloatInfinity 100000.f;


Point3D  GameWorld::GetClosestAvatarPosition(Point3D pos)
{
        float tmp = k_FloatInfinity;
        Point3D aPos,outPos;
        float dist;
		int count = TheMessageMgr->GetPlayerCount();
        GamePlayer* player= static_cast< GamePlayer *>(TheMessageMgr->GetFirstPlayer());
        while(player!=nullptr){
                FighterController* avantar=player->GetPlayerController();
                if(avantar!=nullptr){
                                aPos=avantar->GetPosition();
                                dist=Magnitude(pos-aPos);
                                if(dist<tmp){
									outPos=aPos;
									tmp=dist;
								}
                }
                player=player->GetNextPlayer();
        }
        return(outPos);
}

Point3D GameWorld::GetEnemyNearestToLocation(Point3D point) {
	Point3D nearestEnemyLocation; //TODO: initialize better - turret will hit at the base location
	double shortestDistance = 1000000.0f; //TODO: use max float value
	
	for (int i = 0; i < numEnemiesOnMap; i++) {
		EnemyController *enemy = enemyControllerList[i];
		Point3D enemyLocation = enemy->GetPosition();
		
		double distance = Sqrt(pow(point.x - enemyLocation.x, 2) + pow(point.y - enemyLocation.y, 2) + pow(point.z - enemyLocation.z, 2));
		if (distance < shortestDistance) {
			shortestDistance = distance;
			nearestEnemyLocation = enemyLocation;
		}
	}

	return nearestEnemyLocation;
}

void GameWorld::RemoveEnemy(EnemyController *enemy) {
	enemyControllerList.RemoveListElement(enemy);
	numEnemiesOnMap--; //just added this untested
}

void GameWorld::MoveWorld(void) {
	World::MoveWorld();
	if (TheMessageMgr->GetServerFlag()) {
		if (spawnTimer.activate) {
			spawnTimer.timeLeft = timeBetweenSpawns;
			spawnTimer.activate = false;
		}

		else if (spawnTimer.timeLeft < 0 && enemyWaveCount < numEnemiesToSpawn) {

			GameWorld *world = static_cast<GameWorld *>(TheWorldMgr->GetWorld());

			for (int i = 0; i < npcSpawnLocatorCount; i++) {
				const Marker *marker = world->GetNPCSpawnLocator(i);

				Point3D pos = marker->GetWorldPosition();

				ReqestOjectAtLocation(pos, kNPCEntity, kPlayerServer, i); //TODO: find out why we are choosing player key of 1
				numEnemiesOnMap++;
			}
			enemyWaveCount++;
			spawnTimer.activate = true;
		}

		else {
			spawnTimer.timeLeft -= TheTimeMgr->GetFloatDeltaTime();

			//DisplayBoard::OpenBoard();
			//TheDisplayBoard->ShowMessageText1();
			//printf_s("%d", 2.2);
		}
	}
	
}
//(printf_s("%d", spawnTimer.timeLeft))