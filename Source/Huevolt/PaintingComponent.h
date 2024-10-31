// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "Components/ActorComponent.h"
#include "PaintingComponent.generated.h"

class USceneCaptureComponent2D;

UCLASS( ClassGroup=(Custom), meta=(BlueprintSpawnableComponent) )
class HUEVOLT_API UPaintingComponent : public UActorComponent
{
	GENERATED_BODY()

public:	
	UPROPERTY(EditAnywhere, BlueprintReadOnly, Category = Painting, meta = (AllowPrivateAccess = "true"))
	UMaterial* UnwrapMaterial;

	UPROPERTY(EditAnywhere, BlueprintReadOnly, Category = Painting, meta = (AllowPrivateAccess = "true"))
	USceneCaptureComponent2D* SceneCaptureComponent;

	// Sets default values for this component's properties
	UPaintingComponent();

protected:
	// Called when the game starts
	virtual void BeginPlay() override;

	UMaterialInstanceDynamic* UnwrapMaterialInstance;

	UTextureRenderTarget2D* RenderTarget;

	USceneCaptureComponent2D* SceneCapture;

	float SceneCaptureOrthoWidth = 500.f;
	FVector UnwrapLocation = FVector::Zero();

public:	
	// Called every frame
	virtual void TickComponent(float DeltaTime, ELevelTick TickType, FActorComponentTickFunction* ThisTickFunction) override;

	UFUNCTION()
	void PaintActor(FVector HitLocation, float BrushRadius);
		
};
