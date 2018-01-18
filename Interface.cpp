//
//  Interface.cpp
//  Tombstone
//
//

#include "Interface.hpp"
#include "MMGame.h"
#include "MMGameWorld.h"
#include "MMFighter.h"
#include "TSConfigData.h"
#include "TSPanels.h"
#include <stdio.h>


using namespace MMGame;

namespace
{
    
    
    const float kInterfaceScaleHeight = 1088.0F;
}




DisplayBoard *MMGame::TheDisplayBoard = nullptr;


//void DisplayBoard::ShowMessageText(const char* text)    //MVM's code
//{
//    myText->SetText(text);
//}

GameBoard::GameBoard()
{
}

GameBoard::~GameBoard()
{
}


GameWindow::GameWindow(const char *panelName) : Window(panelName)
{
}

GameWindow::~GameWindow()
{
}



DisplayBoard::DisplayBoard() : Global<DisplayBoard>(TheDisplayBoard)
{

    
    AppendSubnode(&healthDisplayGroup);
    healthDisplayGroup.LoadPanel("hud/Health");
    
    AppendSubnode(&coinsDisplayGroup);
    coinsDisplayGroup.LoadPanel("hud/Coins");

    AppendSubnode(&alertDisplayGroup);
    alertDisplayGroup.LoadPanel("hud/Alert");
    
    AppendSubnode(&crosshairsDisplayGroup);
    crosshairsDisplayGroup.LoadPanel("hud/Crosshairs");

    
    UpdateDisplayPosition();
    
}

DisplayBoard::~DisplayBoard()
{
   
}

void DisplayBoard::OpenBoard(void)
{
    if (!TheDisplayBoard)
    {
        TheGame->AddBoard(new DisplayBoard);
    }
    
//    TheDisplayBoard->UpdatePlayerScore();
//    TheDisplayBoard->UpdatePlayerHealth();


}

void DisplayBoard::PreprocessWidget(void)
{
    
    GameBoard::PreprocessWidget();
    
    coinsText = static_cast<TextWidget *>(FindWidget("Coins"));
    healthText = static_cast<TextWidget *>(FindWidget("Health"));
    healthProgress = static_cast<ProgressWidget *>(FindWidget("HealthBar"));
    healthGlow = FindWidget("HealthGlow");
    
    alertText = static_cast<TextWidget *>(FindWidget("Alert"));
    

}

void DisplayBoard::MoveWidget(void)
{
    
    Board::MoveWidget();
}

//ColorRGBA DisplayBoard::CalculateBarColor(float value)
//{
//    return (ColorRGBA(FmaxZero(Fmin(3.0F - value * 6.0F, 1.0F)), FmaxZero(Fmin(value * 6.0F - 1.0F, 1.0F)), 0.0F, 1.0F));
//}

void DisplayBoard::UpdateDisplayPosition(void)
{
    float displayWidth = float(TheDisplayMgr->GetDisplaySize().x);
    float displayHeight = float(TheDisplayMgr->GetDisplaySize().y);
    
    float scale = displayHeight / kInterfaceScaleHeight;
    Transform4D transform = Transform4D::MakeScale(scale);
    
    
    healthDisplayGroup.SetWidgetTransform(transform);
    
    healthDisplayGroup.SetWidgetPosition(Point3D(displayWidth  - scale * 300.0F, 0.0F, 0.0F));

    
    coinsDisplayGroup.SetWidgetTransform(transform);

    coinsDisplayGroup.SetWidgetPosition(Point3D(displayWidth  - scale * 300.0F, scale * 90.0F, 0.0F));
    
    
    alertDisplayGroup.SetWidgetTransform(transform);
    
    alertDisplayGroup.SetWidgetPosition(Point3D(displayWidth * 0.5F - scale * 144.0F, 0.0F, 0.0F));
    
    float scale2 = scale/4;
    Transform4D transform2 = Transform4D::MakeScale(scale2);
    
    crosshairsDisplayGroup.SetWidgetTransform(transform2);

    crosshairsDisplayGroup.SetWidgetPosition(Point3D(displayWidth * 0.5F - 20.0F, displayHeight * 0.5F - 20.0F, 0.0F));
    

    
    InvalidateWidget();
}



void DisplayBoard::UpdatePlayerScore(void)
{
    const Player *player = TheMessageMgr->GetLocalPlayer();
    if (player)
    {
//        const PlayerState *state = static_cast<const GamePlayer *>(player)->GetPlayerState();
//        scoreText->SetText(String<7>(state->playerScore));
        
        
        // JUST FOR TESTING - SCORE SHOULDNT BE CALCULATED HERE
        String<7> currScoreText = coinsText->GetText();
        int32 currScore = Text::StringToInteger(currScoreText);
        
//        printf(&"currScoreText = " [ currScore]);
        
        int32 newScore = currScore + 1;
        String<7> newScoreText;
        Text::IntegerToString(newScore, newScoreText, 7);
        
        coinsText->SetText(newScoreText);
        
        
        
    }
}

void DisplayBoard::UpdatePlayerHealth(void)
{
    const Player *player = TheMessageMgr->GetLocalPlayer();
    if (player)
    {
        
    
//        int32 currHealth = healthProgress->GetValue();
//        int32 newHealth = MaxZero(currHealth - 10);
//        
//        healthProgress->SetValue(newHealth);
//        
//        if (newHealth <= 0){
//            healthText->SetText(String<>("You're Dead :("));
//        }
        
        const PlayerState *state = static_cast<const GamePlayer *>(player)->GetPlayerState();
        
        int32 health = (state->playerHealth);
        
        healthProgress->SetValue(health);
        
        if (health <= 0){
            healthText->SetText(String<>("You're Dead :("));
//			GamePlayer *gPlayer = static_cast<GamePlayer *>(TheMessageMgr->GetLocalPlayer());
			//TODO: SendGameOverMessage
        }
        
        
//        
//        int32 health = (state->playerHealth + 0xFFFF) >> 16;
//        ColorRGBA color = CalculateBarColor(float(MaxZero(Min(health, 100))) * 0.01F);
//        
//        healthProgress->SetValue(health);
//        healthProgress->SetWidgetColor(color, kWidgetColorHilite);
//        healthText->SetText(String<7>(health));
//        
//        int32 previous = previousHealth;
//        previousHealth = health;
//        
//        if (health <= 20)
//        {
//            if (!healthGlow->WidgetVisible())
//            {
//                healthGlow->ShowWidget();
//                healthGlow->GetFirstMutator()->SetMutatorState(0);
//            }
//            
//            if ((previous > 20) && (health > 0))
//            {
//                Sound *sound = new Sound;
//                sound->LoadSound("sound/Bell");
//                sound->SetSoundPriority(kSoundPriorityDeath);
//                sound->PlaySound();
//            }
//        }
//        else if (healthGlow->WidgetVisible())
//        {
//            healthGlow->HideWidget();
//        }
    }
}

void DisplayBoard::UpdateAlert(void){
    
    alertText->SetText(String<>("Picked up health"));
    alertText->GetFirstMutator()->SetMutatorState(0);
    
}

void DisplayBoard::UpdateAlert(const char *str){
    alertText->SetText(str);
    alertText->GetFirstMutator()->SetMutatorState(0);

}


