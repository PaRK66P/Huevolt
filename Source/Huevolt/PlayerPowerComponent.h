// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "Components/ActorComponent.h"
#include "PlayerPowerComponent.generated.h"

class ABasicCharacter;

UCLASS( ClassGroup=(Custom), meta=(BlueprintSpawnableComponent) )
class HUEVOLT_API UPlayerPowerComponent : public UActorComponent
{
	GENERATED_BODY()

public:	
	// Sets default values for this component's properties
	UPlayerPowerComponent();

	TArray<FLinearColor> ColoursForPainting;

protected:
	// Called when the game starts
	virtual void BeginPlay() override;
	FLinearColor CurrentColour;

	ABasicCharacter* PlayerReference;
	int ColourListSize;

public:	
	// Called every frame
	virtual void TickComponent(float DeltaTime, ELevelTick TickType, FActorComponentTickFunction* ThisTickFunction) override;
	void InitialiseComponent(ABasicCharacter* Reference);


	void UpdateState(FLinearColor colourState);

	void ClearState() { CurrentColour = FLinearColor::Black; }

	void TransitionFunction(FLinearColor transitionColour);

	bool CompareColours(FLinearColor colour0, FLinearColor colour1, float bias);

	void ResetState();
};
