//
//  Interface.hpp
//  Tombstone
//
//

#ifndef Interface_hpp
#define Interface_hpp

#include <stdio.h>
#include "TSInterface.h"

namespace MMGame
{
    using namespace Tombstone;
    using Tombstone::Window;
    
    
    class PlayerSettingsWindow;
    
    
    class GameBoard : public Board, public ListElement<GameBoard>
    {
    protected:
        
        GameBoard();
        
    public:
        
        ~GameBoard();
    };
    
    
    class GameWindow : public Window, public ListElement<GameWindow>
    {
    protected:
        
        GameWindow(const char *panelName);
        
    public:
        
        virtual ~GameWindow();
    };
    
    
    
    class DisplayBoard : public GameBoard, public Global<DisplayBoard>
    {
    private:
        
        TextWidget			*coinsText;
        TextWidget			*healthText;
		TextWidget			*enemyHealthText;
        ProgressWidget		*healthProgress;
        Widget				*healthGlow;
        
        TextWidget          *alertText;
        
        
        Widget				healthDisplayGroup;
        Widget				coinsDisplayGroup;
        Widget              alertDisplayGroup;
        Widget              crosshairsDisplayGroup;

        
//        static ColorRGBA CalculateBarColor(float value);
        
//        TextWidget						*myText;    //MVM's code
        
        
    public:
        
        DisplayBoard();
        ~DisplayBoard();
        
//        void ShowMessageText(const char* text);     //MVM's code
        
        static void OpenBoard(void);
        
        
        void PreprocessWidget(void) override;
        void MoveWidget(void) override;
        
        void UpdateDisplayPosition(void);
        
        void UpdatePlayerScore(void);
        void UpdatePlayerHealth(void);
        void UpdateAlert(void);
        void UpdateAlert(const char *str);

    };
    

    

    extern DisplayBoard *TheDisplayBoard;

}



#endif /* Interface_hpp */
