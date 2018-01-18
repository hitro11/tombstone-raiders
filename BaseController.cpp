#include "BaseController.h"
#include "TSCameras.h"
#include "MMCameras.h"
#include "MMGameWorld.h"
#include "Math.h"
#include "MMMultiplayer.h"

namespace MMGame {
	using namespace Tombstone;
	using Tombstone::ControllerType;

	BaseController::BaseController() : DefenceComponent(defenceControllerType::kControllerBase)
	{
		health = 100;
	}

	BaseController::~BaseController()
	{
	}

	bool BaseController::ValidNode(const Node *node)
	{
		return DefenceComponent::ValidNode(node);
	}

	void BaseController::PreprocessController(void)
	{
		DefenceComponent::PreprocessController();
	}

	void BaseController::MoveController(void)
	{
	}

	void BaseController::DestroySelf() {
		//SendPlayerLoss message
	}

	ControllerMessage *BaseController::CreateMessage(ControllerMessageType type) const {

		switch (type) {
		case kMessageTurretRotation:
			//return new TurretRotationMessage(type, GetControllerIndex());
			return (Controller::CreateMessage(type));
		}
		return (Controller::CreateMessage(type));
	}


	void BaseController::ReceiveMessage(const ControllerMessage *message)
	{
		switch (message->GetControllerMessageType())
		{
		case kMessageTurretRotation:
			break;
		default:
			Controller::ReceiveMessage(message);
			break;
		}
	}

	void BaseController::SendInitialStateMessages(Player *player) const
	{
		printf("Base Initialized and Created \n");
		const Point3D& pos = GetTargetNode()->GetWorldPosition();
		CreateCharacterMessage  message(kMessageCreateCharacter, GetControllerIndex(), kBaseEntity, kPlayerServer, pos, -1);
		player->SendMessage(message);
	}

}