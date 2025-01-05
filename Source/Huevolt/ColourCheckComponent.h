// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "Components/ActorComponent.h"
#include "ColourCheckComponent.generated.h"

class UPlayerPowerComponent;

UCLASS( ClassGroup=(Custom), meta=(BlueprintSpawnableComponent) )
class HUEVOLT_API UColourCheckComponent : public UActorComponent
{
	GENERATED_BODY()

public:	
	UPROPERTY(EditAnywhere, BlueprintReadOnly, Category = Painting, meta = (AllowPrivateAccess = "true"))
	float RayDistance = 100.0f;

	// Sets default values for this component's properties
	UColourCheckComponent();

protected:
	// Called when the game starts
	virtual void BeginPlay() override;

	FHitResult RayCastResult;
	FCollisionQueryParams RayCastCollisionParams;
	FCollisionResponseParams RayCastResponseParams;

	FVector2D UVPosition;

	UPlayerPowerComponent* PowerComponentReference;

public:	
	void InitialiseReferences(UPlayerPowerComponent* playerPowerComponent);
	// Called every frame
	virtual void TickComponent(float DeltaTime, ELevelTick TickType, FActorComponentTickFunction* ThisTickFunction) override;

	void SetupLineTrace(AActor* player);
	FLinearColor CheckColour(bool &result, FVector start);
	void UpdateColour(FLinearColor colourValue);
		
};
