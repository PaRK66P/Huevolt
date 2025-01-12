// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "Components/ActorComponent.h"
#include "PaintingComponent.generated.h"

class USceneCaptureComponent2D;
class APaintRoomActor;

UCLASS( ClassGroup=(Custom), meta=(BlueprintSpawnableComponent) )
class HUEVOLT_API UPaintingComponent : public UActorComponent
{
	GENERATED_BODY()

public:	
	
	UPROPERTY(VisibleAnywhere, BlueprintReadOnly, Category = Painting, meta = (AllowPrivateAccess = "true"))
	UTextureRenderTarget2D* FirstRenderTarget;

	UPROPERTY(VisibleAnywhere, BlueprintReadOnly, Category = Painting, meta = (AllowPrivateAccess = "true"))
	UTextureRenderTarget2D* SecondRenderTarget;

	UPROPERTY(VisibleAnywhere, BlueprintReadOnly, Category = Painting, meta = (AllowPrivateAccess = "true"))
	USceneCaptureComponent2D* SceneCapture;

	// Sets default values for this component's properties
	UPaintingComponent();

protected:
	// Called when the game starts
	virtual void BeginPlay() override;
	virtual void EndPlay(EEndPlayReason::Type EndPlayReason) override;

	bool UsingFirstRenderTarget = true;

	APaintRoomActor* PaintRoomActor;
	FVector PaintRoomPosition;

	UMaterial* UnwrapMaterial;
	UMaterial* BaseMaterial;

	UPROPERTY()
	UMaterialInstanceDynamic* UnwrapMaterialInstance;
	UPROPERTY()
	UMaterialInstanceDynamic* BaseMaterialInstance;

	void ChangeRenderTarget(UTextureRenderTarget2D* renderTarget);

public:	
	// Called every frame
	virtual void TickComponent(float DeltaTime, ELevelTick TickType, FActorComponentTickFunction* ThisTickFunction) override;

	UFUNCTION()
	void PaintActor(FVector HitLocation, float BrushRadius, FVector projectilePaintColour);

	UTextureRenderTarget2D* GetRenderTarget();

};
