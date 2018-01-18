#include "DefenceComponent.h"

using namespace Tombstone;

DefenceComponent::DefenceComponent(defenceControllerType defControllerType) : Controller(defControllerType)
{
}

DefenceComponent::~DefenceComponent()
{
}

bool DefenceComponent::ValidNode(const Node *node)
{
	return (node->GetNodeType() == kNodeModel || node->GetNodeType() == kNodeGeometry);
}

void DefenceComponent::PreprocessController(void)
{
	Controller::PreprocessController();
	InitializeView();
}

void DefenceComponent::InitializeView()
{
	const Node *target = GetTargetNode();
	originalTransform = target->GetWorldTransform();
	Point3D originalDirection = Point3D(originalTransform[0].x, originalTransform[0].y, originalTransform[0].z);
	originalView = (originalDirection).Normalize();
}


void DefenceComponent::MoveController(void)
{
	Controller::MoveController();
}