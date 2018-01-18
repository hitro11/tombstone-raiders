#ifndef __EnemyController_H__
#define __EnemyController_H__

#include "TSNode.h"
#include "MMCharacter.h"
#include "MMFighter.h"
#include "MMMultiplayer.h"

namespace MMGame {

	using namespace Tombstone;

	enum {
		kMonsterType
	};

	enum : ControllerType
	{
		kControllerMonster = 'ctrm'
	};
	

	class EnemyController : public GameCharacterController, public ListElement<EnemyController>{

	private:
		Vector3D originalPosition;

		Tombstone::Transform4D originalTransform;
		EnemyController(const EnemyController& other);

		bool canAttack = true;

	protected:

		struct EnemyTimer
		{
			float	timeLeft;
			bool	canAttack = true;
		}attackTimer;
		Vector2D stop = Vector2D(0, 0);
		float heading = 0;
		float health = 0;
		float walkSpeed;
		float attackDamage, attackSpeed = 100;
		float aggroRange = 10;

		int	  spawnLocatorIndex;					//tags each enemy with an index number according to which spawn locator they've spawned.
		float timeLeftTillAttack = 0;

		void DisplayBlood(void);
		void DestroySelf(void);

	public:
		EnemyController(ControllerType type, int _spawnLocatorIndex);
		~EnemyController();

		static bool ValidNode(const Tombstone::Node *node);

		void PreprocessController();
		void MoveController();
		int GetSpawnLocatorIndex() {
			return spawnLocatorIndex;
		}
		void AttackClosestEnemy(Point3D position,float attackDamage);
		CharacterStatus Damage(Fixed damage, unsigned_int32 flags);
		RigidBodyStatus HandleNewRigidBodyContact(const RigidBodyContact *contact, RigidBodyController *contactBody) override;

		void Pack(Tombstone::Packer& data, unsigned_int32 packFlags) const;
		void Unpack(Tombstone::Unpacker& data, unsigned_int32 unpackFlags);
		Vector2D ComputeForce(Point3D position, int locatorIndex);
		bool GoTowardsPlayer(const Point3D position, Vector3D direction, float distToPlayer);
		void countDownAttackTimer() {
			attackTimer.timeLeft -= TheTimeMgr->GetFloatDeltaTime();
		};

		Point3D GetPosition(void) {
			Node * node = GetTargetNode();
			return(node->GetWorldPosition());
		}

		//ControllerMessage *CreateMessage(ControllerMessageType type) const override;
		//void ReceiveMessage(const ControllerMessage *message) override;
	};
	

	//DG: If desirable DisplayEnemyBloodMessage can be changed to actually deal damage to the enemy instead of just showing the blood
	// I didn't implement damage over the network to reduce overhead
	class DisplayEnemyBloodMessage : public ControllerMessage
	{
		friend class EnemyController;

	private:

	public:
		DisplayEnemyBloodMessage(ControllerMessageType type, int32 index, unsigned_int32 flags = 0);
		~DisplayEnemyBloodMessage();

		void CompressMessage(Compressor& data) const override;
		bool DecompressMessage(Decompressor& data) override;
	};
	
}
#endif