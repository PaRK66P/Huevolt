// Fill out your copyright notice in the Description page of Project Settings.


#include "PaintingComponent.h"

#include "Materials\MaterialInstanceDynamic.h" // For accessing dynamic material methods
#include "Engine\TextureRenderTarget2D.h" // For accessing render target methods
#include "Kismet\KismetRenderingLibrary.h" // For accessing further rendering methods
#include "Components\SceneCaptureComponent2D.h"

#include "Engine\StaticMesh.h"
#include "Engine\StaticMeshActor.h"

#include "Kismet/GameplayStatics.h"         // For GameplayStatics and line tracing


// Sets default values for this component's properties
UPaintingComponent::UPaintingComponent()
{
	// Set this component to be initialized when the game starts, and to be ticked every frame.  You can turn these features
	// off to improve performance if you don't need them.
	PrimaryComponentTick.bCanEverTick = false;

	float SceneCaptureOrthoWidth = 500.f;

	//Scene capture
	/* Consider using a custom depth/stencil mask but be wary of performance, already using another render for each object
	*/
	SceneCapture = CreateDefaultSubobject<USceneCaptureComponent2D>(TEXT("Scene Capture Component Code"));
	if (GetOwner()) {
		SceneCapture->SetupAttachment(GetOwner()->GetRootComponent());
	}
	SceneCapture->ProjectionType = ECameraProjectionMode::Orthographic;
	SceneCapture->OrthoWidth = SceneCaptureOrthoWidth;
	SceneCapture->AutoPlaneShift = false;
	SceneCapture->TextureTarget = RenderTarget;
	SceneCapture->CompositeMode = ESceneCaptureCompositeMode::SCCM_Additive;
	SceneCapture->bCaptureEveryFrame = false;
	SceneCapture->bCaptureOnMovement = false;
	SceneCapture->SetWorldLocationAndRotation(FVector(PaintRoomPosition.X, PaintRoomPosition.Y, PaintRoomPosition.Z + 200.f), FRotator(-90.f, -90.f, 0.f));


}


// Called when the game starts
void UPaintingComponent::BeginPlay()
{
	float SceneCaptureOrthoWidth = 500.f;

	//SceneCapture->SetWorldLocationAndRotation(FVector(PaintRoomPosition.X, PaintRoomPosition.Y, PaintRoomPosition.Z + 200.f), FRotator(-90.f, -90.f, 0.f));

	Super::BeginPlay();

	if (!RenderTarget) {
		RenderTarget = NewObject<UTextureRenderTarget2D>();
		RenderTarget->InitAutoFormat(1024, 1024);
		RenderTarget->RenderTargetFormat = RTF_R16f;
		RenderTarget->ClearColor = FColor::Black;
		RenderTarget->bAutoGenerateMips = false;
		UKismetRenderingLibrary::ClearRenderTarget2D(this, RenderTarget, FColor::Black);

		SceneCapture->TextureTarget = RenderTarget;
	}

	// Unwrap material
	if (UnwrapMaterial) {
		if (!UnwrapMaterialInstance) {
			UnwrapMaterialInstance = UMaterialInstanceDynamic::Create(UnwrapMaterial, this);
			UnwrapMaterialInstance->SetScalarParameterValue(TEXT("CaptureSize"), SceneCaptureOrthoWidth);
			UnwrapMaterialInstance->SetVectorParameterValue(TEXT("UnwrapLocation"), PaintRoomPosition);
		}
	}

	if (BaseMaterial) {
		if (!BaseMaterialInstance) {
			BaseMaterialInstance = UMaterialInstanceDynamic::Create(BaseMaterial, this);
			BaseMaterialInstance->SetVectorParameterValue(TEXT("BaseColour"), BaseColour);
			BaseMaterialInstance->SetVectorParameterValue(TEXT("PaintColour"), PaintColour);
			BaseMaterialInstance->SetTextureParameterValue(TEXT("PaintCapture"), RenderTarget);

			UStaticMeshComponent* MeshToPaint = GetOwner()->GetComponentByClass<UStaticMeshComponent>();
			MeshToPaint->SetMaterial(0, BaseMaterialInstance);
		}
	}
	
}

void UPaintingComponent::EndPlay(EEndPlayReason::Type EndPlayReason)
{
	Super::EndPlay(EndPlayReason);

	if (RenderTarget) {
		RenderTarget->ConditionalBeginDestroy();
		RenderTarget = nullptr;
	}

	if (UnwrapMaterialInstance) {
		UnwrapMaterialInstance->ConditionalBeginDestroy();
		UnwrapMaterialInstance = nullptr;
	}

	if (BaseMaterialInstance) {
		BaseMaterialInstance->ConditionalBeginDestroy();
		BaseMaterialInstance = nullptr;
	}
}




// Called every frame
void UPaintingComponent::TickComponent(float DeltaTime, ELevelTick TickType, FActorComponentTickFunction* ThisTickFunction)
{
	Super::TickComponent(DeltaTime, TickType, ThisTickFunction);

	// ...
}

void UPaintingComponent::PaintActor(FVector HitLocation, float BrushRadius)
{

	//// Painting

	FVector originalPosition = GetOwner()->GetActorLocation();

	GetOwner()->SetActorLocation(PaintRoomPosition);
	SceneCapture->SetWorldLocationAndRotation(FVector(PaintRoomPosition.X, PaintRoomPosition.Y, PaintRoomPosition.Z + 200.f), FRotator(-90.f, -90.f, 0.f));

	UStaticMeshComponent* MeshToPaint = GetOwner()->GetComponentByClass<UStaticMeshComponent>();
	if (!MeshToPaint) {
		GEngine->AddOnScreenDebugMessage(-1, 15, FColor::Red, FString::Printf(TEXT("%s does not contain a static mesh"), *GetOwner()->GetName()));
		return;
	}

	if (!UnwrapMaterialInstance) {
		GEngine->AddOnScreenDebugMessage(-1, 15, FColor::Red, FString::Printf(TEXT("%s does not reference the parent unwrap material"), *GetOwner()->GetName()));
		return;
	}

	UMaterialInterface* OriginalMaterial = MeshToPaint->GetMaterial(0);

	MeshToPaint->SetMaterial(0, UnwrapMaterialInstance);

	//GEngine->AddOnScreenDebugMessage(-1, 15, FColor::Blue, FString::Printf(TEXT("UV position: %f, %f"), uvHitLocation.X, uvHitLocation.Y));


	UnwrapMaterialInstance->SetVectorParameterValue(TEXT("HitLocation"), HitLocation + PaintRoomPosition - originalPosition);
	UnwrapMaterialInstance->SetScalarParameterValue(TEXT("BrushRadius"), 10);

	SceneCapture->CaptureScene();
	MeshToPaint->SetMaterial(0, OriginalMaterial);

	GetOwner()->SetActorLocation(originalPosition);
}



