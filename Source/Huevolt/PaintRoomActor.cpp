// Fill out your copyright notice in the Description page of Project Settings.


#include "PaintRoomActor.h"
#include "Engine\TextureRenderTarget2D.h" // For accessing render target methods

#include "Materials\MaterialInstanceDynamic.h" // For accessing dynamic material methods

// Sets default values
APaintRoomActor::APaintRoomActor()
{
 	// Set this actor to call Tick() every frame.  You can turn this off to improve performance if you don't need it.
	PrimaryActorTick.bCanEverTick = false;

	// Setup meshes [Better workflow to set mesh and material in BP]
	PreviousImage = CreateDefaultSubobject<UStaticMeshComponent>(TEXT("PreviousImage"));
	RootComponent = PreviousImage;
	PreviousImage->SetWorldLocation(FVector(0.0f, 0.0f, -5.f));
	PreviousImage->SetRelativeScale3D(FVector(1.0f, 1.0f, 0.01f));
	Backdrop = CreateDefaultSubobject<UStaticMeshComponent>(TEXT("Backdrop")); 
	Backdrop->SetupAttachment(PreviousImage);
	Backdrop->SetRelativeLocation(FVector(0.0f, 0.0f, -500.f));
	Backdrop->SetRelativeScale3D(FVector(1.0f, 1.0f, 1.0f));
	

	
}

// Called when the game starts or when spawned
void APaintRoomActor::BeginPlay()
{
	Super::BeginPlay();
	
	if (RenderTextureMaterial) {
		if (!RenderTextureMaterialInstance) {
			RenderTextureMaterialInstance = UMaterialInstanceDynamic::Create(RenderTextureMaterial, this);

			PreviousImage->SetMaterial(0, RenderTextureMaterialInstance);
		}
	}
}

void APaintRoomActor::EndPlay(EEndPlayReason::Type EndPlayReason)
{
	Super::EndPlay(EndPlayReason);

	/*if (RenderTextureMaterialInstance) {
		RenderTextureMaterialInstance->ConditionalBeginDestroy();
		RenderTextureMaterialInstance = nullptr;
	}*/
}

// Called every frame
void APaintRoomActor::Tick(float DeltaTime)
{
	Super::Tick(DeltaTime);

}

void APaintRoomActor::InitialisePaintRoom(float size, FVector position)
{
	PreviousImage->SetWorldLocation(FVector(position.X, position.Y, position.Z - 5.f));
	SetActorScale3D(FVector(size / 100.f, size / 100.f, 0.01f));
}

void APaintRoomActor::SetPreviousTexture(UTextureRenderTarget2D* previousTexture)
{
	RenderTextureMaterialInstance->UMaterialInstanceDynamic::SetTextureParameterValue(TEXT("RenderTargetCapture"), previousTexture);
}

