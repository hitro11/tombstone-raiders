
#include "TSGeometryObjects.h"
#include "TSTime.h"
#include "T2BoxController.h"
//#include "MMGame.h"
//#include "MMMultiplayer.h"

using MMGame::T2BoxController;

T2BoxController::T2BoxController() : Tombstone::RigidBodyController(Tombstone::kControllerBox) {

}

T2BoxController::~T2BoxController() {

}

void T2BoxController::PreprocessController() {
	Tombstone::RigidBodyController::PreprocessController();

	const Tombstone::Node *target = GetTargetNode();
	const Tombstone::Node *node = target;
	originalTransform = node->GetNodeTransform();
	if (node->GetNodeType() == Tombstone::kNodeGeometry) {
		Tombstone::GeometryObject *gobj = static_cast<const Tombstone::Geometry *>(node)->GetObject();  // Ew dangling *.
		gobj->SetGeometryFlags(gobj->GetGeometryFlags() | Tombstone::kGeometryDynamic);  // Really hope GetGeometryFlags is inlined....
	}
}

void T2BoxController::MoveController() {
	float step = static_cast<float>(Tombstone::TheTimeMgr->GetMillisecondCount()%1000)/1000;
	// Single jump for now. Double jump later.
	// Gravity hardcoded because this isn't handled by the physics engine yet.
	Tombstone::Transform4D transform(1, 0, 0, 0, 0, 1, 0, 0, 0, 0, 1, 0);
	transform.SetTranslation(0, 0, 4.91*step - 4.91*step*step);

	Tombstone::Node *target = GetTargetNode();

	target->SetNodeTransform(originalTransform * transform);

	target->InvalidateNode();
}

bool T2BoxController::ValidNode(const Tombstone::Node *node) {
	return node->GetNodeType() == Tombstone::kNodeGeometry;
}

Tombstone::RigidBodyStatus T2BoxController::HandleNewRigidBodyContact(const Tombstone::RigidBodyContact *contact, Tombstone::RigidBodyController *other) {
	return Tombstone::kRigidBodyUnchanged;
}

void T2BoxController::Pack(Tombstone::Packer& data, unsigned_int32 packFlags) const {
	data << originalTransform;
}
void T2BoxController::Unpack(Tombstone::Unpacker& data, unsigned_int32 unpackFlags) {
	data >> originalTransform;
}
