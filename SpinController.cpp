#include "TSInterface.h"
#include "SpinController.h"
#include "TSGeometryObjects.h"
#include "TSTime.h"
#include <math.h>
#include <stdio.h>
#include <string.h>
#include "MMGame.h"


using namespace Tombstone;

CustomSpinController::CustomSpinController() : Controller(kControllerCustomSpin)
{
	// Set a default value for the spin rate of one revolution per second
	spinRate = 2*pi / 1000.0F;
	spinAngle = 0.0F;
}

CustomSpinController::CustomSpinController(float rate) : Controller(kControllerCustomSpin)
{
	spinRate = rate;
	spinAngle = 0.0F;
}

CustomSpinController::~CustomSpinController()
{
}

CustomSpinController::CustomSpinController(const CustomSpinController& customSpinController) : Controller(customSpinController)
{
	spinRate = customSpinController.spinRate;
	spinAngle = 0.0F;
}

Controller *CustomSpinController::Replicate(void) const
{
	return (new CustomSpinController(*this));
}

bool CustomSpinController::ValidNode(const Tombstone::Node *node)
{
	//Can only assign to Node of type geometry
	return node->GetNodeType() == Tombstone::kNodeGeometry;
}

void CustomSpinController::Pack(Tombstone::Packer& data, unsigned_int32 packFlags) const
{
	Controller::Pack(data, packFlags);

	// Write the spin rate
	data << spinRate;

	// Write the current angle
	data << spinAngle;

	// Write the original transform
	data << originalTransform;
}

void CustomSpinController::Unpack(Tombstone::Unpacker& data, unsigned_int32 unpackFlags)
{
	Controller::Unpack(data, unpackFlags);

	// Read the spin rate
	data >> spinRate;

	// Read the current angle
	data >> spinAngle;

	// Read the original transform
	data >> originalTransform;
}

int32 CustomSpinController::GetSettingCount(void) const
{
	// There's only one setting
	return (1);
}

Setting *CustomSpinController::GetSetting(int32 index) const
{
	// Is it asking for the first setting?
	if (index == 0)
	{
		// Yes, return a new text setting and set its value in revolutions per second
		return (new TextSetting('rate', Text::FloatToString(spinRate * 1000.0F /2*pi),
			"Spin rate", 7, &EditTextWidget::FloatNumberFilter));
	}

	return (nullptr);
}

void CustomSpinController::SetSetting(const Setting *setting)
{
	// Are we setting the spin rate?
	if (setting->GetSettingIdentifier() == 'rate')
	{
		// Yes, grab the value from the setting and convert it back to radians per millisecond
		const char *text = static_cast<const TextSetting *>(setting)->GetText();
		spinRate = Text::StringToFloat(text) * 2*pi / 1000.0F;
	}
}


void CustomSpinController::PreprocessController(void)
{
	Tombstone::Controller::PreprocessController();

	const Tombstone::Node *target = GetTargetNode();
	const Tombstone::Node *node = target;
	originalTransform = node->GetNodeTransform();
	if (node->GetNodeType() == Tombstone::kNodeGeometry) {
		Tombstone::GeometryObject *gobj = static_cast<const Tombstone::Geometry *>(node)->GetObject();  // Ew dangling *.
		gobj->SetGeometryFlags(gobj->GetGeometryFlags() | Tombstone::kGeometryDynamic);  // Really hope GetGeometryFlags is inlined....
	}
}

void CustomSpinController::MoveController(void)
{
	
	Matrix3D    rotator;

	// Calculate the new spin angle based on how much time has passed
	float angle = spinAngle + spinRate * Tombstone::TheTimeMgr->GetFloatDeltaTime();

	// Make sure it's in the [-pi, pi] range
	if (angle > pi) angle -= 2*pi;
	else if (angle < -pi) angle += 2 * pi;
	spinAngle = angle;
	// Now make a 3x3 rotation matrix
	rotator.MakeRotationZ(angle);
	
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
	target->InvalidateNode();
	
}
