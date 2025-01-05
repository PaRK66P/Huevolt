// Fill out your copyright notice in the Description page of Project Settings.


#include "PlayerPowerComponent.h"
#include "BasicCharacter.h"

// Sets default values for this component's properties
UPlayerPowerComponent::UPlayerPowerComponent()
{
	// Set this component to be initialized when the game starts, and to be ticked every frame.  You can turn these features
	// off to improve performance if you don't need them.
	PrimaryComponentTick.bCanEverTick = true;

	// ...
}


// Called when the game starts
void UPlayerPowerComponent::BeginPlay()
{
	Super::BeginPlay();

	// ...
	CurrentColour = FLinearColor::Black;
	
}


// Called every frame
void UPlayerPowerComponent::TickComponent(float DeltaTime, ELevelTick TickType, FActorComponentTickFunction* ThisTickFunction)
{
	Super::TickComponent(DeltaTime, TickType, ThisTickFunction);

	// ...
}

void UPlayerPowerComponent::InitialiseComponent(ABasicCharacter* Reference)
{
	PlayerReference = Reference;
	ColourListSize = PlayerReference->GetColourList().Num();
}

void UPlayerPowerComponent::UpdateState(FLinearColor colourState)
{
	if (CompareColours(CurrentColour, colourState, 0.01f)) {
		return;
	}

	TransitionFunction(colourState);
}

void UPlayerPowerComponent::TransitionFunction(FLinearColor transitionColour)
{
	ResetState();

	CurrentColour = transitionColour;
	GEngine->AddOnScreenDebugMessage(-1, 15, FColor(transitionColour.R * 255, transitionColour.G * 255, transitionColour.B * 255), TEXT("StateChange"));

	/* Placeholder for future functionality
	* Would be good if you could assign each colour a function
	* With the function assigned being it's mechanic
	* However out of scope for now
	* As such hard coded mechanics
	*/
	if (CompareColours(transitionColour, PlayerReference->GetColourList()[0], 0.01f) && ColourListSize > 0) {
		GEngine->AddOnScreenDebugMessage(-1, 15, FColor(transitionColour.R * 255, transitionColour.G * 255, transitionColour.B * 255), TEXT("Speed"));
		PlayerReference->ModifySpeed(4.f);
		return;
	}
	else if (CompareColours(transitionColour, PlayerReference->GetColourList()[1], 0.01f) && ColourListSize > 1) {
		GEngine->AddOnScreenDebugMessage(-1, 15, FColor(transitionColour.R * 255, transitionColour.G * 255, transitionColour.B * 255), TEXT("Bounce"));
		PlayerReference->Bounce();
		return;
	}
	else if (CompareColours(transitionColour, PlayerReference->GetColourList()[2], 0.01f) && ColourListSize > 2) {
		GEngine->AddOnScreenDebugMessage(-1, 15, FColor(transitionColour.R * 255, transitionColour.G * 255, transitionColour.B * 255), TEXT("Slow"));
		PlayerReference->ModifySpeed(.25f);
		return;
	}

}

bool UPlayerPowerComponent::CompareColours(FLinearColor colour0, FLinearColor colour1, float bias)
{
	float doubleBias = bias * bias;
	if ((colour0.R - colour1.R) * (colour0.R - colour1.R) > doubleBias) {
		return false;
	}
	if ((colour0.G - colour1.G) * (colour0.G - colour1.G) > doubleBias) {
		return false;
	}
	if ((colour0.B - colour1.B) * (colour0.B - colour1.B) > doubleBias) {
		return false;
	}

	return true;
}

void UPlayerPowerComponent::ResetState()
{
	PlayerReference->ModifySpeed(1.0f);
	PlayerReference->BounceReset();

	CurrentColour = FLinearColor::Black;
}



