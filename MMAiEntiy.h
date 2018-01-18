


#ifndef MMAIEnt_H
#define  MMAIEnt_H

#include "MMCharacter.h"
#include "MMAi.h"



namespace MMGame
{
        using namespace Tombstone;
    
    
    enum{
        kAiEntity  = 'gaie'
    };

    class AiEntity : public GameCharacterController
    {
        public:
            AiEntity(ControllerType type);
            ~AiEntity();
            void PreprocessController(void) override;
            void MoveController(void) override;
        
        private:
            int type;
            double heading;
            Behavior* theBehave;

        
    };


};

#endif
