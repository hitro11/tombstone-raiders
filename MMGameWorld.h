//
//  MMGameWorld.h
//
//
//  Created by Martin on 2016-10-04.
//
//

#ifndef __Tombstone__MMGameWorld__
#define __Tombstone__MMGameWorld__

#include "MMGame.h"
#include "MMCameras.h"

enum {
	numEnemiesToSpawn = 2,
	timeBetweenSpawns = 1000
};

namespace MMGame
{
    using namespace Tombstone;


    class FighterController;


class GameWorld : public World
{
private:
	struct MyTimer
	{
		float	amountOfTime;
		float	timeLeft;
		bool	timeIsUp = false;
		bool	activate = true;
	}spawnTimer;

    int32					spawnLocatorCount;
	int32					npcSpawnLocatorCount;
    int32					collLocatorCount;
    int32					AiLocatorCount;
	int32					baseLocatorCount;
	int32					basicTurretLocatorCount;

	int32					enemyWaveCount = 0;
	int32					numEnemiesOnMap = 0;
	int32					numTurretsOnMap = 0;
	int32					numBasesOnMap = 0;
	int32					baseCounter = 0;

	List<EnemyController>	enemyControllerList;
	List<TurretController>	turretControllerList;
	List<BaseController>	baseControllerList;
    List<Marker>			spawnLocatorList;
    List<Marker>			collLocatorList;
	List<Marker>			npcSpawnLocatorList;
    List<Marker>			AiLocatorList;
	List<Marker>			baseLocatorList;
	List<Marker>			basicTurretLocatorList;
    
    
    SpectatorCamera			spectatorCamera;
    FirstPersonCamera		firstPersonCamera;
    ChaseCamera				chaseCamera;
    ModelCamera				*playerCamera;
    
    
    
    float					bloodIntensity;
    
    void CollectZoneMarkers(Zone *zone);
    
    static void HandleShakeCompletion(RumbleShaker *shaker, void *cookie);
    
    static ProximityResult RecordSplashDamage(Node *node, const Point3D& center, float radius, void *cookie);
    
    
    
    
public:
    
	PlayerKey				playerKeyArray[4];

    GameWorld(const char *name);
    ~GameWorld();
	
    
    int32 GetSpawnLocatorCount(void) const
    {
        return (spawnLocatorCount);
    }

	Marker *GetNPCSpawnLocator(int32 index) const
	{
		return (npcSpawnLocatorList[index]);
	}

    Marker *GetSpawnLocator(int32 index) const
    {
        return (spawnLocatorList[index]);
    }

	Marker *GetBaseLocator(int32 index) const
	{
		return (baseLocatorList[index]);
	}

	int32 GetBaseLocatorCount(void) const {
		return (baseLocatorCount);
	}
   
	Marker *GetBasicTurretSpawnLocator(int32 index) const
	{
		return (basicTurretLocatorList[index]);
	}

	void InitializePlayerKeyArray(void);
	int32 GetNextSpawnPointIndex(void);
	void AddPlayerKeyToList(PlayerKey key);
	void RemovePlayerKeyFromListByIndex(int index);
	void RemovePlayerKeyFromListByKey(PlayerKey key);
    
	Point3D GetEnemyNearestToLocation(Point3D point);
	void RemoveEnemy(EnemyController *enemy);

    // NOW REALLY NEEDED
    // ADDED MORE
    int32 GetCollLocatorCount(void) const
    {
        return (collLocatorCount);
    }
    
    Marker *GetCollLocator(int32 index) const
    {
        return (collLocatorList[index]);
    }
    
    SpectatorCamera *GetSpectatorCamera(void)
    {
        return (&spectatorCamera);
    }
    
    ModelCamera *GetPlayerCamera(void)
    {
        return (playerCamera);
    }
    
    bool UsingFirstPersonCamera(void) const
    {
        return (playerCamera == &firstPersonCamera);
    }
    
    void SetBloodIntensity(float blood)
    {
        bloodIntensity = Fmin(blood, 1.0F);
    }
    
    WorldResult PreprocessWorld(void) override;
    
    RigidBodyStatus HandleNewRigidBodyContact(RigidBodyController *rigidBody, const RigidBodyContact *contact, RigidBodyController *contactBody) override;
    
    
    void DetectInteractions(void) override;
    
    void BeginRendering(void) override;
    void EndRendering(FrameBuffer *frameBuffer = nullptr) override;
    
    void SetCameraTargetModel(Model *model);
    void SetSpectatorCamera(const Point3D& position, float azm, float alt);
    
    void SetLocalPlayerVisibility(void);
    void ChangePlayerCamera(void);
    void SetFocalLength(float focal);
    
    void ShakeCamera(float intensity, int32 duration);
    void ShakeCamera(const Point3D& position, float intensity, int32 duration);
    
    
			 //MVM (these functions are to create  Objects
    
    void AddOjectAtLocation(const Point3D& pos ,ObjectType type,long index,PlayerKey key,int locatorIndex);
    void ReqestOjectAtLocation(const Point3D& pos ,ObjectType type,PlayerKey index, int locatorIndex);
    // PRIVATE
    Controller* CreateAvatar(const Point3D& pos ,long index,PlayerKey key);

    void PopulateWorld(void);
	void PopulateTurrets(void);
	void PopulateBases(void);
	void SendExistingControllersForNewClientToPopulate(PlayerKey playerKey);

	void MoveWorld(void);

   FighterController* GetLocalAvantar(void);
   Point3D  GetClosestAvatarPosition(Point3D pos);
  
};

}

#endif /* defined(__Tombstone__MMGameWorld__) */
