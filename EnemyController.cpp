#include "EnemyController.h"
#include "TSNode.h"
#include "TSWorld.h"
#include "MMGameWorld.h"
#include "MMMultiplayer.h"
using namespace MMGame;


#define k_FloatInfinity 100000.f;

EnemyController::EnemyController(ControllerType type, int _spawnLocatorIndex) : GameCharacterController(kMonsterType, type)
{
	spawnLocatorIndex = _spawnLocatorIndex;
}

EnemyController::~EnemyController() {

}


void EnemyController::PreprocessController() {
	GameCharacterController::PreprocessController();
}

Vector2D EnemyController::ComputeForce(Point3D position,int locatorIndex)
{
	GameWorld *world = static_cast<GameWorld *>(TheWorldMgr->GetWorld());

	 //locatorCount = world->GetBaseLocatorCount();

	const Marker *marker =	world->GetBaseLocator(locatorIndex);
	Point3D pos =		marker->GetWorldPosition();
	Vector3D dir =		pos - position;

	const float dist = Magnitude(dir);
	dir.Normalize();

	heading = atan2(dir.y, dir.x);

	dir = dir*walkSpeed;

	if (!(dist < 1) && dist < 400.0f)
	{
		return(Vector2D(dir.x, dir.y));
	}
	else{
		return(Vector2D(0.0f, 0.0f));
	}	
}

void EnemyController::AttackClosestEnemy(Point3D position, float attackDamage) {
	float tmp = k_FloatInfinity;
	Point3D aPos, outPos;
	float dist;
	PlayerKey playerKey;

	GamePlayer* player = static_cast< GamePlayer *>(TheMessageMgr->GetFirstPlayer());
	while (player != nullptr) {
		FighterController* avantar = player->GetPlayerController();
		if (avantar != nullptr) {
			aPos = avantar->GetPosition();
			dist = Magnitude(position - aPos);
			if (dist<tmp) {
				playerKey = player->GetPlayerKey();
				outPos = aPos;
				tmp = dist;
			}
		}
		player = player->GetNextPlayer();
	}
	player = static_cast<GamePlayer *>(TheMessageMgr->GetPlayer(playerKey));
	player->Damage(attackDamage, 0, nullptr);
}

bool EnemyController::GoTowardsPlayer(const Point3D position, Vector3D direction, float distToPlayer) {

	if (distToPlayer < 1) {
		Vector2D v2 = stop;
		SetExternalForce(v2);
		SetCharacterOrientation(heading);
		SetExternalLinearResistance(Vector2D(1, 1));
		return true;
	}
	else {
		direction.Normalize();
		heading = atan2(direction.y, direction.x);
		direction = direction * walkSpeed;
		Vector2D v2 = Vector2D(direction.x, direction.y);
		SetExternalForce(v2);
		SetCharacterOrientation(heading);
		SetExternalLinearResistance(Vector2D(1, 1));
		return false;
	}
}

void EnemyController::MoveController() {
	//Controller::MoveController();

}

RigidBodyStatus EnemyController::HandleNewRigidBodyContact(const RigidBodyContact *contact, RigidBodyController *contactBody) {
	return GameCharacterController::HandleNewRigidBodyContact(contact, contactBody);
}

CharacterStatus EnemyController::Damage(Fixed damage, unsigned_int32 flags) {
	if (health <= 0) {
		DestroySelf();
		return kCharacterKilled;
	}
	else {
		health -= damage;
		TheMessageMgr->SendMessageAll(DisplayEnemyBloodMessage(kMessageDisplayEnemyBlood, GetControllerIndex()));

		return kCharacterDamaged;
	}
}

void EnemyController::DestroySelf() {
	//TODO: implement this functionality with networking messages - send 
	MMGame::GameWorld *world = static_cast<MMGame::GameWorld *>(TheWorldMgr->GetWorld());
	world->RemoveEnemy(this);
	Node *node = GetTargetNode();
	node->DisableNode();
	delete this;
}

void EnemyController::DisplayBlood() {
	MMGame::GameWorld *world = static_cast<MMGame::GameWorld *>(TheWorldMgr->GetWorld());
	SparksParticleSystem *sparks = new SparksParticleSystem(40, 1.0f);
	Vector3D sparksColor(255.0f, 0.0f, 0.0f);
	sparks->SetColor(sparksColor);
	sparks->SetNodePosition(GetPosition());
	world->AddNewNode(sparks);
}

bool EnemyController::ValidNode(const Tombstone::Node *node) {
	return node->GetNodeType() == Tombstone::kNodeModel;
}

void EnemyController::Pack(Tombstone::Packer& data, unsigned_int32 packFlags) const {
	data << originalTransform;
}
void EnemyController::Unpack(Tombstone::Unpacker& data, unsigned_int32 unpackFlags) {
	data >> originalTransform;
}
/*
ControllerMessage *EnemyController::CreateMessage(ControllerMessageType type) const {
	switch (type) {
		case kMessageDisplayEnemyBlood:
			return new TurretRotationMessage(type, GetControllerIndex());
	}
	return (Controller::CreateMessage(type));
}


void EnemyController::ReceiveMessage(const ControllerMessage *message)
{
	switch (message->GetControllerMessageType())
	{
		case kMessageDisplayEnemyBlood:
			DisplayBlood();
			break;
		default:
			Controller::ReceiveMessage(message);
			break;
	}
}
*/

DisplayEnemyBloodMessage::DisplayEnemyBloodMessage(ControllerMessageType type, int32 index, unsigned_int32 flags) : Tombstone::ControllerMessage(type, index, flags)
{
}

DisplayEnemyBloodMessage::~DisplayEnemyBloodMessage()
{
}

void DisplayEnemyBloodMessage::CompressMessage(Compressor& data) const
{
	ControllerMessage::CompressMessage(data);
}

bool DisplayEnemyBloodMessage::DecompressMessage(Decompressor& data)
{
	if (ControllerMessage::DecompressMessage(data))
	{
		return (true);
	}

	return (false);
}