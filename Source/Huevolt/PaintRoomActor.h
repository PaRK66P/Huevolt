// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "GameFramework/Actor.h"
#include "PaintRoomActor.generated.h"

UCLASS()
class HUEVOLT_API APaintRoomActor : public AActor
{
	GENERATED_BODY()
	
public:	
	UPROPERTY(EditAnywhere, BlueprintReadOnly, Category = Painting, meta = (AllowPrivateAccess = "true"))
	UMaterial* RenderTextureMaterial;

	/** First person camera */
	UPROPERTY(VisibleAnywhere, BlueprintReadOnly, Category = Painting, meta = (AllowPrivateAccess = "true"))
	UStaticMeshComponent* Backdrop;
	
	/** First person camera */
	UPROPERTY(VisibleAnywhere, BlueprintReadOnly, Category = Painting, meta = (AllowPrivateAccess = "true"))
	UStaticMeshComponent* PreviousImage;


	// Sets default values for this actor's properties
	APaintRoomActor();

protected:
	// Called when the game starts or when spawned
	virtual void BeginPlay() override;
	virtual void EndPlay(EEndPlayReason::Type EndPlayReason) override;

	UMaterialInstanceDynamic* RenderTextureMaterialInstance;

public:	
	// Called every frame
	virtual void Tick(float DeltaTime) override;

	void InitialisePaintRoom(float size, FVector position);

	void SetPreviousTexture(UTextureRenderTarget2D* previousTexture);

};
