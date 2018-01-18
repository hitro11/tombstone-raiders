#ifndef SimpleTurret_h
#define SimpleTurret_h

#include "SimpleProjectile.h"
#include "DefenceComponent.h"


namespace MMGame
{
	using namespace Tombstone;

	enum
	{
		kModelTurret = 'Turr'
	};


	class TurretController : public DefenceComponent, public ListElement<TurretController>
	{
	private:
		
		int nextRotationCounter;
		int nextShotCounter;
		float rotationSpeed;
		float fireRate;
		float firingRange;
		Point3D targetToAimAt;
		Node *turretBarrel;		//Marker on Turret Model from which missiles are fired

	public:

		TurretController();
		~TurretController();
		static bool ValidNode(const Node *node);
		void PreprocessController(void);
		void MoveController(void) override;

		void InitializeView(void);

		void SetMaxTurretFireRate(float newFireRate);
		void SetFiringRange(float newFiringRange);
		void SetTargetToAimAt(Point3D newTarget);
		void SetTurretBarrelNode(void);

		Vector3D GetAdjustedRotation(Vector3D normalizedView);
		void SetRotation(Vector3D updatedView);
		
		void FireProjectile(float distance, Vector3D updatedView, Point3D startPos);
		void DisplayFireBurst(void);
		bool IsReadyToShoot(void);
		Point3D GetNearestEnemyLocation(void);

		void SendTurretRotationMessage(Vector3D updatedView);
		ControllerMessage *CreateMessage(ControllerMessageType type) const override;
		void ReceiveMessage(const ControllerMessage *message) override;
		void SendInitialStateMessages(Player *player) const override;
	};


	class TurretRotationMessage : public ControllerMessage
	{
		friend class TurretController;

	private:

		Vector3D				view;

	public:
		TurretRotationMessage(ControllerMessageType type, int32 index);
		TurretRotationMessage(ControllerMessageType type, const Vector3D& newView, int32 index, unsigned_int32 flags = 0);
		~TurretRotationMessage();

		const Vector3D& GetView(void) const
		{
			return (view);
		}

		void CompressMessage(Compressor& data) const override;
		bool DecompressMessage(Decompressor& data) override;
	};

	class TurretHitEnemyMessage : public ControllerMessage
	{
		friend class TurretController;

	private:

	public:
		TurretHitEnemyMessage(ControllerMessageType type, int32 index, unsigned_int32 flags = 0);
		~TurretHitEnemyMessage();
		
		void CompressMessage(Compressor& data) const override;
		bool DecompressMessage(Decompressor& data) override;
	};

}

#endif