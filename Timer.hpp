#ifndef Timer_hpp
#define Timer_hpp

#include <cassert>
#include <stdio.h>
#include <list>
#include <iostream>
#include "TSTime.h"

namespace MMGame
{
   
    using namespace Tombstone;
    

    class TimerTask
    {
        
        
        
    public:
        // allows specification of how many repetitions (should be used with repeat=true)
        template <typename F>
        TimerTask(int intervalMS ,bool isRepeated ,int numOfReps ,F callback)
        :   duration(intervalMS), // duration is in milliseconds
            repeat(isRepeated),
            numReps(numOfReps),
            func(callback),
            phase(0)
        {
            assert(func);
        };
        template <typename F>
        TimerTask(int intervalMS, bool isRepeated, F callback)
        :   duration(intervalMS),
            repeat(isRepeated),
            func(callback),
            phase(0)
        {
            assert(func);
        };
        
        
        int duration;
        bool repeat;
        int numReps;
        int phase;
        std::function<void()> func;
        
        void updateContinuingTask();
        
    };
    
    
    /* HOW TO ADD A TASK:
     
     Static Function:
     
     TheTimerMgr->addTask( new TimerTask(3000,true,5, &TheGame->DoIt) );
     
     TheTimerMgr->addTask( new TimerTask(2000,true, &TheGame->DoIt2) );
     
     Non-Static Member Function:
     
     TheTimerMgr->addTask( new TimerTask(2000,true,5, std::bind(&Game::DoIt3,this)));
     */
    
    class TimerMgr
    {
    public:
        TimerMgr(){};
        
        void addTask(TimerTask * t) { tasks.push_back(t); }
        
        void tick(void); // this method is called every frame
        
        
    private:
        std::list<TimerTask*> tasks;
        
        
    };  
    
  extern TimerMgr *TheTimerMgr;
    
}



#endif /* Timer_hpp */
