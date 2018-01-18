
#ifndef __CustomSpinController_H__
#define __CustomSpinController_H__

#include "TSCharacter.h"
#include "TSNode.h"

namespace Tombstone {
	enum
	{
		kControllerCustomSpin = 'CSPN'
	};

	class CustomSpinController : public Tombstone::Controller
	{
	private:

		float         spinRate;             // In radians per millisecond
		float         spinAngle;            // The current angle, in radians
		Transform4D   originalTransform;    // The target's original transform
		double pi = 3.1415926535897;

		CustomSpinController(const CustomSpinController& customSpinController);

		Controller *Replicate(void) const;

	public:

		CustomSpinController();
		CustomSpinController(float rate);
		~CustomSpinController();

		float GetSpinRate(void) const
		{
			return (spinRate);
		}

		void SetSpinRate(float rate)
		{
			spinRate = rate;
		}

		static bool ValidNode(const Node *node);

		// Serialization functions
		void Pack(Packer& data, unsigned_int32 packFlags) const;
		void Unpack(Unpacker& data, unsigned_int32 unpackFlags);

		// User interface functions
		
		int32 GetSettingCount(void) const;
		Setting *GetSetting(int32 index) const;
		void SetSetting(const Setting *setting);
		

		void PreprocessController(void);

		// The function that moves the target node
		void MoveController(void);
	};

}

#endif
