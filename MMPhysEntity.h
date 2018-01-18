


#ifndef MMPhysEnt_H
#define  MMPhysEnt_H


#include "TSTriggers.h"

#include "MMCharacter.h"




namespace MMGame
{
        using namespace Tombstone;
    
    
    enum{
        kPhysEnity  = 'gphy'
    };

    class PhysEntity : public RigidBodyController
    {
        public:
            PhysEntity();
            ~PhysEntity();

        
    };


};

#endif
