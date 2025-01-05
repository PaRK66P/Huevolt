// Fill out your copyright notice in the Description page of Project Settings.


#include "ColourCheckComponent.h"
#include "PaintingComponent.h"
#include "PlayerPowerComponent.h"

#include "Kismet\KismetRenderingLibrary.h"
#include "Kismet/GameplayStatics.h"

// Sets default values for this component's properties
UColourCheckComponent::UColourCheckComponent()
{
	// Set this component to be initialized when the game starts, and to be ticked every frame.  You can turn these features
	// off to improve performance if you don't need them.
	PrimaryComponentTick.bCanEverTick = true;

	// ...
}


// Called when the game starts
void UColourCheckComponent::BeginPlay()
{
	Super::BeginPlay();
}


void UColourCheckComponent::InitialiseReferences(UPlayerPowerComponent* playerPowerComponent)
{
	PowerComponentReference = playerPowerComponent;
}

// Called every frame
void UColourCheckComponent::TickComponent(float DeltaTime, ELevelTick TickType, FActorComponentTickFunction* ThisTickFunction)
{
	Super::TickComponent(DeltaTime, TickType, ThisTickFunction);

}

void UColourCheckComponent::SetupLineTrace(AActor* player)
{
	RayCastCollisionParams = FCollisionQueryParams::DefaultQueryParam;
	RayCastCollisionParams.bTraceComplex = true;
	RayCastCollisionParams.bReturnFaceIndex = true;
	RayCastCollisionParams.AddIgnoredActor(player);
	RayCastResponseParams = FCollisionResponseParams::DefaultResponseParam;
}

FLinearColor UColourCheckComponent::CheckColour(bool &result, FVector start)
{
	GetWorld()->LineTraceSingleByChannel(RayCastResult, start, start + FVector::DownVector * RayDistance, ECollisionChannel::ECC_Visibility, RayCastCollisionParams, RayCastResponseParams);

	result = false;

	if (!RayCastResult.bBlockingHit) {
		PowerComponentReference->ResetState();
		return FLinearColor::Black;
	}

	if (UPaintingComponent* PaintComponent = RayCastResult.GetActor()->FindComponentByClass<UPaintingComponent>()) {

		UTextureRenderTarget2D* renderTarget = PaintComponent->GetRenderTarget();
		if (UGameplayStatics::FindCollisionUV(RayCastResult, 0, UVPosition)) {
			FLinearColor returnColour = UKismetRenderingLibrary::ReadRenderTargetUV(GetWorld(), renderTarget, UVPosition.X, UVPosition.Y);
			result = true;

			UpdateColour(returnColour);
			return returnColour;
		}

	}

	PowerComponentReference->ResetState();
	return FLinearColor::Black;
	
}

void UColourCheckComponent::UpdateColour(FLinearColor colourValue)
{
	PowerComponentReference->UpdateState(colourValue);
}

