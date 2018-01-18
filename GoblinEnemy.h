#ifndef __GoblinEnemy_H__
#define __GoblinEnemy_H__

#include "EnemyController.h"
#include "MMGame.h"

namespace MMGame {
	enum : ModelType
	{
		kModelGoblin = 'gbln'
	};

	enum : ControllerType
	{
		kControllerGoblin = 'gbln'
	};

	class GoblinEnemy : public EnemyController, public ListElement<GoblinEnemy>
	{
	public:
		GoblinEnemy(int _spawnLocatorIndex);
		~GoblinEnemy();

		ControllerMessage *CreateMessage(ControllerMessageType type)const override;
		void ReceiveMessage(const ControllerMessage *message) override;

	private:

		static bool ValidNode(const Tombstone::Node *node);
		void PreprocessController();
		void MoveController();
		RigidBodyStatus HandleNewRigidBodyContact(const RigidBodyContact *contact, RigidBodyController *contactBody) override;
		void SendInitialStateMessages(Player *player) const override;

	};

	class EnemyMessage : public ControllerMessage
	{
		friend class GoblinEnemy;

	private:
		Point3D _position;
		Vector3D _direction;

	public:
		EnemyMessage(ControllerMessageType type, int32 index);
		EnemyMessage(ControllerMessageType type, const Point3D position, Vector3D direction, int32 index, unsigned_int32 flags = 0);
		~EnemyMessage();

		void CompressMessage(Compressor& data) const override;
		bool DecompressMessage(Decompressor& data) override;

		const Point3D& getPosition() const {
			return (_position);
		}

		const Vector3D& getDirection() const {
			return (_direction);
		}
	};
}
#endif