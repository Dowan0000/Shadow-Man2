// Copyright Epic Games, Inc. All Rights Reserved.

#include "T1GameMode.h"
#include "T1Character.h"
#include "UObject/ConstructorHelpers.h"

AT1GameMode::AT1GameMode()
{
	// set default pawn class to our Blueprinted character
	static ConstructorHelpers::FClassFinder<APawn> PlayerPawnBPClass(TEXT("/Game/ThirdPerson/Blueprints/BP_ThirdPersonCharacter"));
	if (PlayerPawnBPClass.Class != NULL)
	{
		DefaultPawnClass = PlayerPawnBPClass.Class;
	}
}
