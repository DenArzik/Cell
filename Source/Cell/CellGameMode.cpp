// Copyright 1998-2017 Epic Games, Inc. All Rights Reserved.

#include "CellGameMode.h"
#include "Pawns/PlayerPawn.h"

ACellGameMode::ACellGameMode()
{
	// set default pawn class to our character class
	DefaultPawnClass = APlayerPawn::StaticClass();
}

