//=============================================================
//
// Tombstone Engine version 1.0

// CHANGED MVM

// Unauthorized redistribution of source code is strictly
// prohibited. Violators will be prosecuted.
//
//=============================================================


#ifndef MMCollectControllers_h
#define  MMCollectControllers_h


#include "TSTriggers.h"

#include "MMCharacter.h"




namespace MMGame 
{
	using namespace Tombstone;

/*
	enum : ControllerType
	{
		kControllerCollectable		= 'clct',
	};
*/

	enum : LocatorType
	{
		kLocatorCenter				= 'cent'
	};




	// This controller makes a model such as a gun or power-up
	// rotate and float up and down.

	class CollectableController final : public Controller
	{
		private:


			enum
			{
				kCollectableInitialized		= 1 << 0, 
				kCollectableDelete			= 1 << 1 
			};
            unsigned_int32              collectableFlags;
 
			int32						respawnInterval;
			int32						respawnTime;
            Trigger                     *triggerNode;

			Point3D						centerPosition;

			CollectableController(const CollectableController& collectableController);

			Controller *Replicate(void) const override;

		public:

			enum
			{
				kCollectableMessageCollect,
				kCollectableMessageRespawn,
				kCollectableMessageState
			};

			CollectableController();
			~CollectableController();

			Model *GetTargetNode(void) const
			{
				return (static_cast<Model *>(Controller::GetTargetNode()));
			}

			static bool ValidNode(const Node *node);

			void PreprocessController(void) override;
			void MoveController(void) override;
			void ActivateController(Node *initiator, Node *trigger) override;

			ControllerMessage *CreateMessage(ControllerMessageType type) const override;
			void ReceiveMessage(const ControllerMessage *message) override;
			void SendInitialStateMessages(Player *player) const override;
        
            static void Tick(void);
	};


	class CollectableStateMessage : public ControllerMessage
	{
		friend class CollectableController;

		private:

			bool	activeFlag;

			CollectableStateMessage(int32 controllerIndex);

		public:

			CollectableStateMessage(int32 controllerIndex, bool active);
			~CollectableStateMessage();

			bool GetActiveFlag(void) const
			{
				return (activeFlag);
			}

			void CompressMessage(Compressor& data) const override;
			bool DecompressMessage(Decompressor& data) override;
	};


}


#endif

// ZUXSVMT
