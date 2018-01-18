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


#include "MMBase.h"
#include "MMGame.h"


using namespace MMGame;


Tombstone::Application *CreateApplication(void)
{
	return (new Game);
}

// ZUXSVMT
