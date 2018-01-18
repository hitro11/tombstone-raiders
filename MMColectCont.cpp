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


#include "MMColectCont.h"
#include "MMGame.h"
#include "MMMultiplayer.h"
#include <stdio.h>


using namespace  MMGame;




CollectableController::CollectableController() : Controller(kControllerCollectable)
{
	collectableFlags = kCollectableDelete;
    triggerNode = nullptr;

	respawnInterval = 12000;
	respawnTime = 0;

}

CollectableController::CollectableController(const CollectableController& collectableController) : Controller(collectableController)
{

	collectableFlags = collectableController.collectableFlags;
    triggerNode = nullptr;

	respawnInterval = collectableController.respawnInterval;
	respawnTime = 0;

}

CollectableController::~CollectableController()
{
	delete triggerNode;
	
}

Controller *CollectableController::Replicate(void) const
{
	return (new CollectableController(*this));
}


bool CollectableController::ValidNode(const Node *node)
{
	return (node->GetNodeType() == kNodeModel);
}




void CollectableController::PreprocessController(void)
{
	Controller::PreprocessController();

    Model *model = GetTargetNode();
    centerPosition = model->GetNodePosition();

    unsigned_int32 flags = collectableFlags;
    if (!(flags & kCollectableInitialized)){
        collectableFlags = flags | kCollectableInitialized;
        centerPosition = model->GetNodePosition();
    }
    

		if (TheMessageMgr->GetServerFlag())
		{
			Trigger *trigger = new  CylinderTrigger(Vector2D(0.3F, 0.3F), 1.0F);
            trigger->SetNodeFlags(kNodeNonpersistent );//| (model->GetNodeFlags() & kNodeDisabled));
			trigger->SetNodePosition(Point3D(centerPosition.x, centerPosition.y, centerPosition.z - 0.5F));
            
            
            printf("ADDING TRIGGER NODE  %f %f %f \n",centerPosition.x, centerPosition.y, centerPosition.z - 0.5F);

            
			trigger->AddConnector("ModelMM", model); //This was changed from "Model" to "ModelMM"
			triggerNode = trigger;

			TriggerObject *object = trigger->GetObject();
			object->SetActivationConnectorKey("ModelMM");
            
			object->SetTriggerFlags(kTriggerContinuouslyActivated);

			World *world = model->GetWorld();
            world->AddNewNode(trigger);
            
            //trigger->ActivateTrigger();
            
            trigger->EnableNode();
            
            
            //Node *node2 = trigger->GetConnectedNode(object->GetActivationConnectorKey());
            //node2->DisableNode();
            //model->GetController()->ActivateController(this, this);
        }

	
}

void CollectableController::MoveController(void)
{
	//Model *model = GetTargetNode();

}

void CollectableController::ActivateController(Node *initiator, Node *trigger)
{
    printf("I GOR ACTIVATED \n");
    
    //initiator->DisableNode();

	TheMessageMgr->SendMessageAll(ControllerMessage(kCollectableMessageCollect, GetControllerIndex()));
}


ControllerMessage *CollectableController::CreateMessage(ControllerMessageType type) const
{
	switch (type)
	{
		case kCollectableMessageCollect:
		case kCollectableMessageRespawn:

			return (new ControllerMessage(type, GetControllerIndex()));

		case kCollectableMessageState:

			return (new CollectableStateMessage(GetControllerIndex()));
	}

	return (Controller::CreateMessage(type));
}

void CollectableController::ReceiveMessage(const ControllerMessage *message)
{
	Model *model = GetTargetNode();

	switch (message->GetControllerMessageType())
	{
		case kCollectableMessageCollect:
		{

			if (!(collectableFlags & kCollectableDelete))
			{
				model->DisableNode();

				Trigger *trigger = triggerNode;
				if (trigger)
				{
					trigger->DisableNode();
				}

				respawnTime = respawnInterval;
			}
			else
			{
				delete model;
			}

			break;
		}

		case kCollectableMessageRespawn:
		{

			respawnTime = 0;
			break;
		}

		case kCollectableMessageState:
		{
			Trigger *trigger = triggerNode;
			if (static_cast<const CollectableStateMessage *>(message)->GetActiveFlag())
			{
				model->EnableNode();
				if (trigger)
				{
					trigger->EnableNode();
				}
			}
			else
			{
				model->DisableNode();
				if (trigger)
				{
					trigger->DisableNode();
				}
			}

			break;
		}

		default:

			Controller::ReceiveMessage(message);
			break;
	}
}

void CollectableController::SendInitialStateMessages(Player *player) const
{
	//player->SendMessage(CollectableStateMessage(GetControllerIndex(), GetTargetNode()->NodeEnabled()));
    printf("Collect Send Initial State Message CALLED \n");
    
    
    const Point3D& pos = GetTargetNode()->GetWorldPosition();
    // this -1 is a hack as -1 is the server, I 
    CreateCharacterMessage message(kMessageCreateCharacter,GetControllerIndex(),kCollectEntity,-1,pos,-1);
    player->SendMessage(message);
}


CollectableStateMessage::CollectableStateMessage(int32 controllerIndex) : ControllerMessage(CollectableController::kCollectableMessageState, controllerIndex)
{
}

CollectableStateMessage::CollectableStateMessage(int32 controllerIndex, bool active) : ControllerMessage(CollectableController::kCollectableMessageState, controllerIndex)
{
	activeFlag = active;
}

CollectableStateMessage::~CollectableStateMessage()
{
}

void CollectableStateMessage::CompressMessage(Compressor& data) const
{
	ControllerMessage::CompressMessage(data);

	data << activeFlag;
}

bool CollectableStateMessage::DecompressMessage(Decompressor& data)
{
	if (ControllerMessage::DecompressMessage(data))
	{
		data >> activeFlag;
		return (true);
	}

	return (false);
}



// ZUXSVMT
