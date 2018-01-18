//
//  MMGamePlayer.cpp
//  Tombstone
//
//  Created by Martin on 2016-10-04.
//
//

#include "MMGamePlayer.h"
#include "MMGame.h"
#include "MMFighter.h"

using namespace MMGame;


/*---------------------------------------------------------------------*/


GamePlayer::GamePlayer(PlayerKey key) : Player(key)
{
    playerController = nullptr;
    
    
    Game::InitPlayerStyle(playerStyle);
    
    scoreUpdateTime = 0;
    deathTime = -1;
    shieldTime = -1;
    
}

GamePlayer::~GamePlayer()
{
}


void GamePlayer::SetPlayerController(FighterController *controller, const PlayerState *state)
{
    playerController = controller;
    if (controller)
    {
        controller->SetFighterPlayer(this);
        
         if (state)
         {
         playerState = *state;
         }
         else
         {
             
             playerState.playerHealth = 100;
             playerState.playerCoins = 0;
         
         }
        
    }
}




void GamePlayer::SetPlayerStyle(const int32 *style)
{
    for (machine a = 0; a < kPlayerStyleCount; a++)
    {
        playerStyle[a] = style[a];
    }
    
    if (playerController)
    {
        playerController->SetFighterStyle(style);
    }
}

void GamePlayer::AddPlayerScore(Fixed delta)
{
    
}




void GamePlayer::SetPlayerHealth(Fixed health)
{
    playerState.playerHealth = health;
    
    if (TheDisplayBoard)
    {
        TheDisplayBoard->UpdatePlayerHealth();
    }
}


void GamePlayer::AddPlayerKill(void)
{
}


CharacterStatus GamePlayer::Damage(Fixed damage, unsigned_int32 flags, GameCharacterController *attacker)
{
    printf("GamePlayer::DAMAGE() : Player takes damage\n");
    if (TheMessageMgr->GetServerFlag())
        {
            //GamePlayer *attackerPlayer = GetAttackerPlayer(attacker);
            
            int32 health = GetPlayerHealth();
            
            if (damage > 0){
                health -= damage;
            }
            
            if (health > 0){
                SetPlayerHealth(health);
                return (kCharacterDamaged);
            }
            else{
                Kill(this->GetPlayerController());             // originally: Kill(attacker);
                return (kCharacterKilled);
                
            }

        }
    
    
    return (kCharacterUnaffected);
}

void GamePlayer::Kill(GameCharacterController *attacker)
{
    if (TheMessageMgr->GetServerFlag())
    {
        SetPlayerHealth(0);
        
        if (playerController)
        {
            TheMessageMgr->SendMessageAll(ControllerMessage(FighterController::kFighterMessageDeath, playerController->GetControllerIndex()));
            TheMessageMgr->SendMessageAll(DeathMessage(GetPlayerKey(), GetAttackerKey(attacker)));
            
            scoreUpdateTime = 0;
            scoreboardTime = 0;
            //deathTime = kPlayerRespawnInterval;
        }
    }
}

GamePlayer *GamePlayer::GetAttackerPlayer(const GameCharacterController *attacker)
{
    if ((attacker) && (attacker->GetCharacterType() == kCharacterPlayer))
    {
        //return (static_cast<const FighterController *>(attacker)->GetFighterPlayer());
    }
    
    return (nullptr);
}

PlayerKey GamePlayer::GetAttackerKey(const GameCharacterController *attacker)
{
    if ((attacker) && (attacker->GetCharacterType() == kCharacterPlayer))
    {
        //return (static_cast<const FighterController *>(attacker)->GetFighterPlayer()->GetPlayerKey());
    }
    
    return (kPlayerNone);
}



