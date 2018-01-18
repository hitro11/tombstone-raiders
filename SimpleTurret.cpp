#include "SimpleTurret.h"
#include "TSCameras.h"
#include "MMCameras.h"
#include "MMGameWorld.h"
#include "Math.h"
#include "EnemyController.h"
#include "MMMultiplayer.h"


namespace MMGame {
	using namespace Tombstone;
	using Tombstone::ControllerType;

	TurretController::TurretController() : DefenceComponent(defenceControllerType::kControllerTurret)
	{
	}

	TurretController::~TurretController()
	{
	}

	bool TurretController::ValidNode(const Node *node)
	{
		return DefenceComponent::ValidNode(node);
	}

	void TurretController::PreprocessController(void)
	{
		DefenceComponent::PreprocessController();
		rotationSpeed = 0.1f;
		SetMaxTurretFireRate(0.05f);
		SetFiringRange(1000.0f);
		nextShotCounter = 0;
		SetTurretBarrelNode();
	}

	void TurretController::InitializeView() {
		DefenceComponent::InitializeView();
	}

	void TurretController::MoveController(void)
	{
		if (TheMessageMgr->GetServerFlag() && nextRotationCounter++ % 2 == 0) {
			DefenceComponent::MoveController();

			Node *target = DefenceComponent::GetTargetNode();

			Point3D startPos = turretBarrel->GetWorldPosition();
			MMGame::GameWorld *world = static_cast<MMGame::GameWorld *>(TheWorldMgr->GetWorld());
			//Point3D targetPoint = world->GetClosestAvatarPosition(startPos);
			Point3D targetPoint = GetNearestEnemyLocation();

			SetTargetToAimAt(targetPoint);

			Vector3D view = (targetPoint - startPos);
			float distance = Sqrt(view.x * view.x + view.y * view.y + view.z * view.z);
			view = view.Normalize();

			if (distance < firingRange) {
				Vector3D updatedView = GetAdjustedRotation(view);
				SendTurretRotationMessage(updatedView);
				FireProjectile(distance, updatedView, startPos);
			}
		}
	}

	Vector3D TurretController::GetAdjustedRotation(Vector3D normalizedView) {
		Vector3D updatedView = Vector3D(originalView.x + (normalizedView.x - originalView.x) * rotationSpeed, originalView.y + (normalizedView.y - originalView.y) * rotationSpeed, originalView.z + (normalizedView.z - originalView.z) * rotationSpeed);
		originalView = updatedView;
		return updatedView;
	}

	void TurretController::SetRotation(Vector3D updatedView) {
		float x = updatedView.x;
		float y = updatedView.y;
		float f = InverseSqrt(x * x + y * y);
		Vector3D right(y * f, -x * f, 0.0F);
		Vector3D down = Cross(updatedView, right);

		Node *target = DefenceComponent::GetTargetNode();
		target->SetNodeMatrix3D(updatedView, -right, -down);
		target->InvalidateNode(); //TODO: check this is in the right location
	}

	void TurretController::FireProjectile(float distance, Vector3D updatedView, Point3D startPos) {
		World *world = TheWorldMgr->GetWorld();
		if (world)
		{
			Controller    *controller;
			Model *model = nullptr;
			if (IsReadyToShoot())
			{
				float lookAzimuth = atan2(updatedView.y, updatedView.x);
				float lookAltitude = asin(updatedView.z);


				CollisionData   collisionData;
				const Point3D& position = turretBarrel->GetWorldPosition();
				Vector2D t = CosSin(lookAzimuth);
				Vector2D u = CosSin(lookAltitude);
				Vector3D shotDirection(t.x * u.x, t.y * u.x, u.y);
				Point3D pos(position.x, position.y, position.z);

				World* world = TheWorldMgr->GetWorld();
				CollisionState state = world->QueryCollision(pos, pos + shotDirection* 100.F, 0.0F, kCollisionProjectile, &collisionData);
				if (state == kCollisionStateGeometry) {
					printf("GEOMETRY \n");
				}
				else if (state == kCollisionStateRigidBody) {
					printf("BODY \n");
					//Hit Enemy
					//TODO: Add a try catch here in case it cannot cast to EnemyController type
					TheMessageMgr->SendMessageAll(TurretHitEnemyMessage(kMessageTurretHitEnemy, GetControllerIndex()));
					MMGame::EnemyController *enemyContr = static_cast<MMGame::EnemyController *>(collisionData.rigidBody);
					enemyContr->Damage(10, 0);
				}
				else {
					printf(" Miss` \n");
				}
				
			}
		}
	}

	void TurretController::DisplayFireBurst() {
		MMGame::GameWorld *world = static_cast<MMGame::GameWorld *>(TheWorldMgr->GetWorld());
		SparksParticleSystem *sparks = new SparksParticleSystem(40, 1.0f);
		sparks->SetNodePosition(turretBarrel->GetWorldPosition());
		world->AddNewNode(sparks);
	}

	bool TurretController::IsReadyToShoot() {
		return nextShotCounter++ % 300 == 0;
	}

	Point3D TurretController::GetNearestEnemyLocation() {
		Point3D nearestEnemyLocation;
		MMGame::GameWorld *world = static_cast<MMGame::GameWorld *>(TheWorldMgr->GetWorld());
		nearestEnemyLocation = world->GetEnemyNearestToLocation(this->GetPosition());
		nearestEnemyLocation.z += 0.5f; //Aim slightly higher than ground
		return nearestEnemyLocation;
	}

	void TurretController::SetTurretBarrelNode() {
		//Find the marker on the Turret. It is the point from which to fire.
		Node *root = DefenceComponent::GetTargetNode();
		Node *thisnode = root;
		String<30> nodeName = "NoName";
		bool found = false;
		do
		{
			nodeName = "NoName";
			if (thisnode->GetNodeName() != NULL)
				nodeName = thisnode->GetNodeName();
			if ((Text::CompareText(nodeName, "TurretBarrel"))) {
				turretBarrel = thisnode;
				found = true;
			}
			thisnode = root->GetNextTreeNode(thisnode);
		} while (thisnode && !found);
	}

	void TurretController::SetMaxTurretFireRate(float newFireRate) {
		fireRate = newFireRate;
	}

	void TurretController::SetFiringRange(float newFiringRange) {
		firingRange = newFiringRange;
	}

	void TurretController::SetTargetToAimAt(Point3D newTarget) {
		targetToAimAt = newTarget;
	}


	void TurretController::SendTurretRotationMessage(Vector3D updatedView) {
		TheMessageMgr->SendMessageAll(TurretRotationMessage(kMessageTurretRotation, updatedView, GetControllerIndex()));
	}

	ControllerMessage *TurretController::CreateMessage(ControllerMessageType type) const {

		switch (type) {
			case kMessageTurretRotation:
				return new TurretRotationMessage(type, GetControllerIndex());
			case kMessageTurretHitEnemy:
				return new TurretHitEnemyMessage(type, GetControllerIndex());
		}

		return (Controller::CreateMessage(type));
	}


	void TurretController::ReceiveMessage(const ControllerMessage *message)
	{
		switch (message->GetControllerMessageType())
		{
		case kMessageTurretRotation:
		{
			const TurretRotationMessage *m = static_cast<const TurretRotationMessage *>(message);
			Vector3D view = m->GetView();
			SetRotation(view);
			break;
		}
		case kMessageTurretHitEnemy:
		{
			DisplayFireBurst();
			break;
		}

		default:
			Controller::ReceiveMessage(message);
			break;
		}
	}

	void TurretController::SendInitialStateMessages(Player *player) const
	{
		printf("Base Initialized and Created \n");
		const Point3D& pos = GetTargetNode()->GetWorldPosition();
		CreateCharacterMessage  message(kMessageCreateCharacter, GetControllerIndex(), kBaseTurretEntity, kPlayerServer, pos, -1);
		player->SendMessage(message);
	}



	TurretRotationMessage::TurretRotationMessage(ControllerMessageType type, int32 index) : Tombstone::ControllerMessage(type, index)
	{
	}

	TurretRotationMessage::TurretRotationMessage(ControllerMessageType type, const Vector3D& newView, int32 index, unsigned_int32 flags) : Tombstone::ControllerMessage(type, index, flags)
	{
		view = newView;
	}

	TurretRotationMessage::~TurretRotationMessage()
	{
	}

	void TurretRotationMessage::CompressMessage(Compressor& data) const
	{
		ControllerMessage::CompressMessage(data);
		data << view.x;
		data << view.y;
		data << view.z;
	}

	bool TurretRotationMessage::DecompressMessage(Decompressor& data)
	{
		if (ControllerMessage::DecompressMessage(data))
		{
			data >> view.x;
			data >> view.y;
			data >> view.z;
			return (true);
		}

		return (false);
	}



	TurretHitEnemyMessage::TurretHitEnemyMessage(ControllerMessageType type, int32 index, unsigned_int32 flags) : Tombstone::ControllerMessage(type, index, flags)
	{
	}

	TurretHitEnemyMessage::~TurretHitEnemyMessage()
	{
	}

	void TurretHitEnemyMessage::CompressMessage(Compressor& data) const
	{
		ControllerMessage::CompressMessage(data);
	}

	bool TurretHitEnemyMessage::DecompressMessage(Decompressor& data)
	{
		if (ControllerMessage::DecompressMessage(data))
		{
			return (true);
		}

		return (false);
	}


}