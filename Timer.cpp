
#include <list>
#include <iostream>
#include "Timer.hpp"

using namespace MMGame;
  

TimerMgr *MMGame::TheTimerMgr = nullptr;


void TimerTask::updateContinuingTask()
{
    phase = 0;              // reset phase to repeat count
    if (numReps){
        numReps--;
        if (numReps <= 1){ // one more left: call func then delete
            repeat = false;
        }
    }
}



void TimerMgr::tick(void)
{
	std::list<TimerTask*>::iterator it = tasks.begin();
    int dt = TheTimeMgr->GetDeltaTime();    // or GetSystemDeltaTime()?
    
    while (it != tasks.end()){
        (*it)->phase += dt;
		if((*it)->phase > (*it)->duration){ // phase larger than duration
			(*it)->func();					// call the function
			if((*it)->repeat){              // if this is a recurring task
				(*it)->updateContinuingTask();
                it++;
			}else{
				it=tasks.erase(it);
			}
		}else{
			it++;
		}
		
	}

}
