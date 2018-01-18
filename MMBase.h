//=============================================================
//
// Tombstone Engine version 1.0
// Copyright 2016, by Terathon Software LLC
//
// This file is part of the Tombstone Engine and is provided under the
// terms of the license agreement entered by the registed user.
//
// Unauthorized redistribution of source code is strictly
// prohibited. Violators will be prosecuted.
//
//=============================================================


#ifndef MMBase_h
#define MMBase_h


#include "TSSound.h"
#include "TSZones.h"
#include "TSApplication.h"
#include <stdio.h>
#include <math.h>


#define MMCHEATS	1


extern "C"
{
	TOMBSTONE_MODULE_EXPORT Tombstone::Application *CreateApplication(void);
}


namespace MMGame
{
	using namespace Tombstone;
	using Tombstone::Fixed;
	using Tombstone::UnsignedFixed;
	using Tombstone::ChunkHeader;
	using Tombstone::Zone;
	using Tombstone::Marker;


	enum
	{
		kInputXInverted				= 1 << 0,
		kInputYInverted				= 1 << 1
	};


	enum
	{
		kPlayerColorCount			= 11
	};


	enum : LocatorType
	{
		kLocatorSpawn		=	'spwn',
		kLoactorColl        =	'colc',
        kLocatorPys         =   'plxc',
		kLocatorAttack		=	'pa3t',
		kLocatorEvade		=	'pgve',
		kLocatorRand		=	'rnde',
		kNPCLocatorSpawn	=	'nspn',
		kLocatorBase		=	'bslc',
		kLocatorBasicTurret =	'bTrt'
	};


    enum
    {
        kControllerCollectable  ='1111'
    };
    
    
    
    enum
    {
        kModelApple     ='appl'
    };
    
    
    // THESE ARE THE "ENTITIES" WE CAN ADD TO THE WORLD
    enum{
        kSoldierEntity      ='csol',	// THE AVATAR
        kCollectEntity      ='ccol',	// SOMETHING WE CAN COLLECT
        kPhysEntiy          ='cphy',	// SOMETHING WITH PHYSICS
        kPhysEvade          ='cevy',	// SOMETHING WITH PHYSICS
        kPhysAttack         ='cary',    // SOMETHING WITH PHYSICS
        kPhysRand           ='crnd',    // SOMETHING WITH PHYSICS
		kNPCEntity			='cnpc',
		kBaseTurretEntity	='cbtr',
		kBaseEntity			='cbas',
		kGoblinEnemy		='gble'
    };
}

#endif

// ZUXSVMT
