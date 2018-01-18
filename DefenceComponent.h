#ifndef DefenceComponent_h
#define DefenceComponent_h

#include "TSWorld.h"


namespace Tombstone
{
	// Controller types
	enum defenceControllerType
	{
		kControllerDefenceComponent = 'DefC',
		kControllerTurret = 'Turr',
		kControllerBase = 'Base'
	};

	enum
	{
		kModelDefenceComponent = 'DefC'
	};

	class DefenceComponent : public Controller
	{
	private:
		
	protected:
		int					health;
		Transform4D        originalTransform;    // The target's original transform
		Vector3D        originalView;        // The direction turret is facing before updating each frame
	
	public:

		DefenceComponent(defenceControllerType);
		~DefenceComponent();
		static bool ValidNode(const Node *node);
		void PreprocessController(void);
		void MoveController(void) override;

		void InitializeView(void);
		void DestroySelf(void);
		
		Point3D GetPosition(void)
		{
			Node* node = GetTargetNode();
			return(node->GetWorldPosition());
		}
	};

}

#endif