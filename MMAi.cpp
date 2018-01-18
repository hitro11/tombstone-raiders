


//
//
//  Created by Martin v. Mohrenschildt on 14-11-19.
//  Copyright (c) 2017 McMaster. All rights reserved.
//


#include "MMAi.h"
#include "MMGameWorld.h"
#include <math.h>
#include <stdio.h>

#ifndef M_PI //LB: added to get around M_PI undefined error
#define M_PI 3.14159265358979323846
#endif

using namespace MMGame;


// Super class for AI navigation
// Compute                  Vector2D force(0.0F, 0.0F);

Behavior::Behavior()
{
}

Behavior::~Behavior()
{
}

void Behavior::Init(void)
{

}
/*------------------------------------------------------------------------------*/

// Steer away from any Avatar
Evade::Evade()
{
}

Evade::~Evade()
{
}


void Evade::Init(void)
{
}


#define d_PrivatRadius 10.f
#define d_EvaderSpeed 10.F


Vector2D Evade::ComputeForce(Point3D position)
{
    GameWorld *world = static_cast<GameWorld *>(TheWorldMgr->GetWorld());
    Point3D pos= world->GetClosestAvatarPosition(position);
    
    Vector3D dir  = pos-position;
    
    const float dist=Magnitude(dir);
    dir.Normalize();
    
    heading = M_PI+atan2(dir.y,dir.x);
    
    dir=dir*d_EvaderSpeed;
    
    if(dist<d_PrivatRadius){
        return(Vector2D(-1.f*dir.x,-1.f*dir.y));
    }else{
        return(Vector2D(0.0f,0.0f));
    }
}
/*------------------------------------------------------------------------------*/

// Steer away from any Avatar
Attack::Attack()
{
}

Attack::~Attack()
{
}


void Attack::Init(void)
{
}


#define d_PrivatRadius 10.f
#define d_AttackerSpeed 8.F


Vector2D Attack::ComputeForce(Point3D position)
{
    GameWorld *world = static_cast<GameWorld *>(TheWorldMgr->GetWorld());
    Point3D pos= world->GetClosestAvatarPosition(position);
    
    Vector3D dir  = pos-position;
    
    const float dist=Magnitude(dir);
    dir.Normalize();
    
    heading = atan2(dir.y,dir.x);
    
    dir=dir*d_AttackerSpeed;
    
    if(dist<d_PrivatRadius){
        return(Vector2D(dir.x,dir.y));
    }else{
        return(Vector2D(0.0f,0.0f));
    }
}

/*------------------------------------------------------------------------------*/

// Steer away from any Avatar
RandWalkd::RandWalkd()
{
}

RandWalkd::~RandWalkd()
{
}


void RandWalkd::Init(void)
{
}



#define kRandomSpeed 1.F


Vector2D RandWalkd::ComputeForce(Point3D position)
{
    Vector2D propel(0.0F, 0.0F);
    heading+=Math::RandomFloat(.2F)-.1F;
    propel.x = Cos(heading) * kRandomSpeed;
    propel.y = Sin(heading) * kRandomSpeed;
    
    
    return(propel);
    
   }

