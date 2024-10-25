// Fill out your copyright notice in the Description page of Project Settings.


#include "BasicMoveShootPlayerController.h"
#include "EnhancedInputSubsystems.h"
#include "Engine\LocalPlayer.h"

void ABasicMoveShootPlayerController::BeginPlay() {
	Super::BeginPlay();

	// Get the enhance input subsystem
	if (UEnhancedInputLocalPlayerSubsystem* Subsystem = ULocalPlayer::GetSubsystem<UEnhancedInputLocalPlayerSubsystem>(GetLocalPlayer())) {
		// Add the mapping context so we get controls
		Subsystem->AddMappingContext(InputMappingContext, 0);
	}
}
