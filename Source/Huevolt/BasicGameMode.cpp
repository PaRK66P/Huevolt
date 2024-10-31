// Fill out your copyright notice in the Description page of Project Settings.


#include "BasicGameMode.h"
#include "BasicCharacter.h"
#include "UObject/ConstructorHelpers.h"

ABasicGameMode::ABasicGameMode()
	: Super()
{
	// set default pawn class to our Blueprinted character
	static ConstructorHelpers::FClassFinder<APawn> PlayerPawnClassFinder(TEXT("/Game/Characters/BP_BasicCharacter"));
	DefaultPawnClass = PlayerPawnClassFinder.Class;
}
