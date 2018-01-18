

#include "MMGameWorld.h"
#include "MMAiEntiy.h"


using namespace  MMGame;



AiEntity::AiEntity(ControllerType _type):GameCharacterController(type,kCharacterAi)
{
    type=_type;
    heading=0.;
}


AiEntity::~AiEntity()
{
    
}


void AiEntity::PreprocessController(void)
{
    GameCharacterController::PreprocessController();
    SetFrictionCoefficient(0.0F);
    //SetFrictionCoefficient(1.0F);
    
    // Depending on type choose Behaviour
    if(type== kAIEvade)theBehave=new Evade();
    else if(type== kAIAttack)theBehave=new Attack();
    else theBehave=new RandWalkd();

}


void AiEntity::MoveController(void)
{
    GameCharacterController::MoveController();
    SetExternalForce(theBehave->ComputeForce(GetTargetNode()->GetWorldPosition()));
    SetCharacterOrientation(theBehave->GetHeading());

}

