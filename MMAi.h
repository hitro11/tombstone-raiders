


//
//
//  Created by Martin v. Mohrenschildt on 14-11-19.
//  Copyright (c) 2017 McMaster. All rights reserved.
//

#ifndef C4_MMAI_h
#define C4_MMAI_h


#include "MMBase.h"
#ifndef M_PI //LB: added to get around M_PI undefined error
#define M_PI 3.14159265358979323846
#endif
namespace MMGame
{

    using namespace Tombstone;


// Super class for AI navigation
// Compute                  Vector2D force(0.0F, 0.0F);
    
    
    enum PFEnemyType{
        
        kAIEvade,
        kAIAttack,
        kAIRnd
    };

class Behavior
{

        public:
            Behavior();
            ~Behavior();
            virtual void Init(void);
            virtual Vector2D ComputeForce(Point3D position)=0;
    virtual float GetHeading(void){return(heading);};
    
        protected:
            float heading;
};


// Steer away from any Avatar
class  Evade: public Behavior
{
	 public:
        Evade();
        ~Evade();
        Vector2D ComputeForce(Point3D position);
    void Init(void);


};
    
    
    class  Attack: public Behavior
    {
    public:
        Attack();
        ~Attack();
        Vector2D ComputeForce(Point3D position);
        void Init(void);
        
        
    };
    
    
    
    class  RandWalkd: public Behavior
    {
    public:
        RandWalkd();
        ~RandWalkd();
        Vector2D ComputeForce(Point3D position);
        void Init(void);
        
        
    };

    
};

#endif
