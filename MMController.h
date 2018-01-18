#pragma once
//
//  MMController.h
//  C4
//
//  Created by Martin v. Mohrenschildt on 14-09-23.
//  Copyright (c) 2014 McMaster. All rights reserved.
//

#ifndef TS_MMController_h
#define TS_MMController_h

#include "TSEngine.h"
#include "TSController.h"
#include "TSProperties.h"
#include "TSApplication.h"
#include "TSConfiguration.h"
#include "TSWorld.h"

#include "MMCharacter.h"



namespace Tombstone
{


	enum
	{
		kControllerTest = 'test'
	};



	class MMController : public   Controller //  GameCharacterController
	{
	private:

		float         testRate;             // In radians per millisecond
		float         testAngle;            // The current angle, in radians
		Transform4D   originalTransform;    // The target's original transform

											// MARTIN
		
		String<25> 		contName;
		Point3D		centerPosition;
		double pi = 3.1415926535897; //DG


		MMController(const MMController& testController);
		//Controller *Replicate(void) const;


	public:

		MMController();
		MMController(float rate);
		~MMController();

		float GetTestRate(void) const
		{
			return (testRate);
		}

		void SetTestRate(float rate)
		{
			testRate = rate;
		}

		static bool ValidNode(const Node *node);

		// Serialization functions
		void Pack(Packer& data, unsigned long packFlags) const;
		void Unpack(Unpacker& data, unsigned long unpackFlags);

		// User interface functions
		int32 GetSettingCount(void) const;
		Setting *GetSetting(int32 index) const;
		void SetSetting(const Setting *setting);

		void Preprocess(void);
		void Activate(Node *trigger, Node *activator);

		// The function that moves the target node
		void Move(void);

		void Kill(void) {};

		RigidBodyStatus HandleNewRigidBodyContact(const RigidBodyContact *contact, RigidBodyController *body1);


	};

}




#endif