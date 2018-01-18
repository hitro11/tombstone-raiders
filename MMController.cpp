//
//  MMController.cpp
//  C4
//
//  Created by Martin v. Mohrenschildt on 14-09-23.
//  Copyright (c) 2014 McMaster. All rights reserved.
//

//DG
#include "MMGame.h"
//#include "SimpleChar.h"
#include "MMController.h"

#include "TSConfiguration.h"
#include "TSInterface.h"

//DG
#include <math.h>
#include <stdio.h>
#include <string.h>

using namespace Tombstone;


/* REGISTRATION

// Define the controller registration
this is added to SimpkeChar.h and SimpleChar.cpp
ControllerReg<MMController> testControllerReg;

testControllerReg(kControllerTest, "Test"),


*/

MMController::MMController() //: Controller(kControllerTest)
{
	// Set a default value for the test rate of one revolution per second
	//testRate = K::two_pi / 1000.0F;
	testRate = 2*pi / 1000.0F; //DG
	testAngle = 0.0F;
}

MMController::MMController(float rate) : Controller(kControllerTest)
{
	testRate = rate;
	testAngle = 0.0F;
}

MMController::~MMController()
{
}


MMController::MMController(const MMController& testController) : Controller(testController)
{
	testRate = testController.testRate;
	testAngle = 0.0F;
}
/*
Controller *MMController::Replicate(void) const
{
return (new MMController(*this));
}
*/

bool MMController::ValidNode(const Node *node)
{
	return (node->GetNodeType() == kNodeGeometry);
}

// Serialization



void MMController::Pack(Packer& data, unsigned long packFlags) const
{
	Controller::Pack(data, packFlags);

	// Write the test rate
	data << testRate;

	// Write the current angle
	data << testAngle;

	// Write the original transform
	data << originalTransform;

	data << contName;



}

void MMController::Unpack(Unpacker& data, unsigned long unpackFlags)
{
	Controller::Unpack(data, unpackFlags);

	// Read the test rate
	data >> testRate;

	// Read the current angle
	data >> testAngle;

	// Read the original transform
	data >> originalTransform;


	data >> contName;

}

// User Interface

int32 MMController::GetSettingCount(void) const
{
	// There are Two settings in the interface
	printf("ASKED FOR NUMBER SETTTINGS \n");
	return (2);
}

Setting *MMController::GetSetting(int32 index) const
{
	printf("Asked for value of setting %d \n", index);
	// Is it asking for the first setting?
	if (index == 0) {
		// Yes, return a new text setting and set its value in revolutions per second

		//return (new TextSetting('rate', Text::FloatToString(testRate * 1000.0F / K::two_pi), "Test rate", 7, &EditTextWidget::NumberFilter));
		return (new TextSetting('rate', Text::FloatToString(testRate * 1000.0F / 2*pi), "Test rate", 7, &EditTextWidget::NumberFilter)); //DG
	}

	//MARTIN
	if (index == 1) {
		return (new TextSetting('name', contName, "Name", 7, nullptr));
	}

	return (nullptr);
}

void MMController::SetSetting(const Setting *setting)
{
	// Are we setting the test rate?
	if (setting->GetSettingIdentifier() == 'rate')
	{
		// Yes, grab the value from the setting and convert it back to radians per millisecond
		const char *text = static_cast<const TextSetting *>(setting)->GetText();
		//testRate = Text::StringToFloat(text) * K::two_pi / 1000.0F;
		testRate = Text::StringToFloat(text) * 2*pi / 1000.0F; //DG
	}
	if (setting->GetSettingIdentifier() == 'name')
	{
		// Yes, grab the value from the setting and convert it back to radians per millisecond
		const char *text = static_cast<const TextSetting *>(setting)->GetText();
		strcpy(contName, text);
	}
}



// Moving


void MMController::Preprocess(void)
{
	//Controller::Preprocess();
	Controller::PreprocessController(); //DG

	//Node *entity = GetTargetNode();
	// Grab the original transform of the target node
	const Node *target = GetTargetNode();
	originalTransform = target->GetNodeTransform();

	// Set the kGeometryDynamic flag for any geometry nodes
	const Node *node = target;

	//DG - unsure if this is correct
	if (node->GetNodeType() == kNodeGeometry)
	{
		// Node is a geometry, so grab its object
		GeometryObject *object = static_cast<const Geometry *>(node)->GetObject();

		// Set the kGeometryDynamic flag
		object->SetGeometryFlags(object->GetGeometryFlags() | kGeometryDynamic);
	}

	/*
	do
	{
		if (node->GetNodeType() == kNodeGeometry)
		{
			// Node is a geometry, so grab its object
			GeometryObject *object = static_cast<const Geometry *>(node)->GetObject();

			// Set the kGeometryDynamic flag
			object->SetGeometryFlags(object->GetGeometryFlags() | kGeometryDynamic);
		}

		// Iterate through entire subtree
		node = target->GetNextNode(node);
	} while (node);
	*/



}



void MMController::Move(void)
{
	Matrix3D    rotator;

	// Calculate the new test angle based on how much time has passed

	float angle = testAngle + testRate * TheTimeMgr->GetFloatDeltaTime();

	// Make sure it's in the [-pi, pi] range
	//if (angle > K::pi) angle -= K::two_pi;
	if (angle > pi) angle -= 2 * pi;
	//else if (angle < -K::pi) angle += K::two_pi;
	else if (angle < -pi) angle += 2*pi;

	testAngle = angle;

	// Now make a 3x3 rotation matrix
	//rotator.SetRotationAboutZ(angle);
	rotator.MakeRotationZ(angle); //DG - might need to be just the change in angle (i.e. testRate)

	// We'll rotate about the center of the target node's bounding sphere
	Node *target = GetTargetNode();
	const Point3D& worldCenter = target->GetBoundingSphere()->GetCenter();
	Point3D objectCenter = target->GetInverseWorldTransform() * worldCenter;

	// Make a 3x4 transform that rotates about the center point
	Transform4D transform(rotator, objectCenter - rotator * objectCenter);

	// Apply the rotation transform to the original transform and
	// assign it to the node as its new transform
	target->SetNodeTransform(originalTransform * transform);

	// Invalidate the target node so that it gets updated properly
	//target->Invalidate();
	target->InvalidateNode(); //DG
}

void MMController::Activate(Node *trigger, Node *activator)
{
	printf("Activated \n");

	/*
	Sound* activeSound = new Sound;
	if(activeSound!=NULL){
	activeSound->Load("sound/Teleport");
	activeSound->Play();
	}
	*/

}

RigidBodyStatus MMController::HandleNewRigidBodyContact
(const RigidBodyContact *contact, RigidBodyController *body1)
{
	printf("COntact \n");

	return (kRigidBodyUnchanged);
}




