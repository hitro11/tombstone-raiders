#include "GoblinEnemy.h"
#include "MMGameWorld.h"
#include "Interface.hpp"



using namespace MMGame;

GoblinEnemy::GoblinEnemy(int _spawnLocatorIndex) :EnemyController(kControllerGoblin, spawnLocatorIndex) //TODO: might not have to pass in spawnLocator
{
	spawnLocatorIndex = _spawnLocatorIndex;
	walkSpeed = 7.0;
	attackDamage = 2;
	health = 100;
}


GoblinEnemy::~GoblinEnemy()
{
}

ControllerMessage *GoblinEnemy::CreateMessage(ControllerMessageType type)const {
	switch (type) {
	case kMessageMonsterMovement:
		return (new EnemyMessage(type, GetControllerIndex()));
	case kMessageDisplayEnemyBlood:
		return new TurretRotationMessage(type, GetControllerIndex());
	}

	return (Controller::CreateMessage(type));
}

void GoblinEnemy::ReceiveMessage(const ControllerMessage *message) {
	switch (message->GetControllerMessageType())
	{
	case kMessageMonsterMovement:
	{
		const EnemyMessage *m = static_cast<const EnemyMessage *>(message);

		const float distToPlayer = Magnitude(m->getDirection());

		// if player is within aggro range then go t-owards the player
		if (distToPlayer<aggroRange) {

			bool closeToAttack = GoTowardsPlayer(m->getPosition(), m->getDirection(), distToPlayer);
			if (closeToAttack) {
				AttackClosestEnemy(m->getPosition(), attackDamage);
			}
		}
		// no player in range head towards base
		else {
			Vector2D v2 = ComputeForce(m->getPosition(), spawnLocatorIndex);
			SetExternalForce(v2);
			SetCharacterOrientation(heading);
			SetExternalLinearResistance(Vector2D(1, 1));
		}

		break;
	}
	case kMessageDisplayEnemyBlood:
		DisplayBlood();
		break;
	default:
		Controller::ReceiveMessage(message);
		break;
	}
}

bool GoblinEnemy::ValidNode(const Tombstone::Node *node) {
	return node->GetNodeType() == Tombstone::kNodeModel;
}

void GoblinEnemy::PreprocessController() {
	EnemyController::PreprocessController();
}

void GoblinEnemy::MoveController() {


	if (TheMessageMgr->GetServerFlag()) {

		const Point3D& position = GetTargetNode()->GetWorldPosition();
		GameWorld *world = static_cast<GameWorld *>(TheWorldMgr->GetWorld());
		Point3D posOfPlayer = world->GetClosestAvatarPosition(position);

		Vector3D dir = posOfPlayer - position;

		EnemyMessage message(kMessageMonsterMovement, position, dir, GetControllerIndex(), 0);
		TheMessageMgr->SendMessageAll(message);
	}
}

RigidBodyStatus GoblinEnemy::HandleNewRigidBodyContact(const RigidBodyContact *contact, RigidBodyController *contactBody) {


	RigidBodyType type = contactBody->GetRigidBodyType();

	if (!attackTimer.canAttack) {
		countDownAttackTimer();
	}
	/*Player controller is assigned the Int value of 12. TODO: need to figure out what the name of it is. It is not kControllerSoldier*/
	if (contactBody->GetControllerType() == 12) {
		FighterController *character = static_cast<FighterController *>(contactBody);

		TheDisplayBoard->UpdateAlert(String<>("Contact with Soldier"));
		if (attackTimer.canAttack) {
			character->Damage(30, 0, nullptr);
			attackTimer.timeLeft = attackSpeed;
			attackTimer.canAttack = false;
		}
	}
	ControllerType cont = contactBody->GetControllerType();
	if (cont == kControllerGoblin) {
		TheDisplayBoard->UpdateAlert(String<>("Contact Goblin by Goblin"));
	}
	return RigidBodyStatus(kRigidBodyUnchanged);
}

void GoblinEnemy::SendInitialStateMessages(Player *player) const
{
	printf("Goblin Initialized and Created \n");

	const Point3D& pos = GetTargetNode()->GetWorldPosition();
	EnemyType type = kNPCEntity; //TODO: set this to appropriate enemy type as a property of the enemyController
	CreateCharacterMessage  message(kMessageCreateCharacter, GetControllerIndex(), type, kPlayerServer, pos, spawnLocatorIndex);
	player->SendMessage(message);

}

EnemyMessage::EnemyMessage(ControllerMessageType type, int32 index) : Tombstone::ControllerMessage(type, index) {

}

EnemyMessage::EnemyMessage(ControllerMessageType type, const Point3D position, Vector3D direction, int32 index, unsigned_int32 flags) : Tombstone::ControllerMessage(type, index) {
	_position = position;
	_direction = direction;

}

EnemyMessage::~EnemyMessage() {
}


void EnemyMessage::CompressMessage(Compressor& data) const
{
	ControllerMessage::CompressMessage(data);
	data << _position.x;
	data << _position.y;
	data << _position.z;
	data << _direction.x;
	data << _direction.y;
	data << _direction.z;
}

bool EnemyMessage::DecompressMessage(Decompressor& data) {
	if (ControllerMessage::DecompressMessage(data)) {
		data >> _position.x;
		data >> _position.y;
		data >> _position.z;
		data >> _direction.x;
		data >> _direction.y;
		data >> _direction.z;
		return true;
	}
	return false;
}