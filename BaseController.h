#ifndef BaseController_h
#define BaseController_h

#include "SimpleProjectile.h"
#include "DefenceComponent.h"


namespace MMGame
{
	using namespace Tombstone;

	enum
	{
		kModelBase = 'Base'
	};


	class BaseController : public DefenceComponent, public ListElement<BaseController>
	{
	private:

		int	playerPosition;
	public:

		BaseController();
		~BaseController();
		static bool ValidNode(const Node *node);
		void PreprocessController(void);
		void MoveController(void) override;

		void DestroySelf(void);

		void SetPlayerPosition(int position) {
			playerPosition = position;
		}

		ControllerMessage *CreateMessage(ControllerMessageType type) const override;
		void ReceiveMessage(const ControllerMessage *message) override;
		void SendInitialStateMessages(Player *player) const override;
	};

}

#endif