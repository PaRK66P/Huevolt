// Fill out your copyright notice in the Description page of Project Settings.


#include "PaintingComponent.h"

#include "Materials\MaterialInstanceDynamic.h" // For accessing dynamic material methods
#include "Engine\TextureRenderTarget2D.h" // For accessing render target methods
#include "Kismet\KismetRenderingLibrary.h" // For accessing further rendering methods
#include "Components\SceneCaptureComponent2D.h"

#include "Engine\StaticMesh.h"
#include "Engine\StaticMeshActor.h"


// Sets default values for this component's properties
UPaintingComponent::UPaintingComponent()
{
	// Set this component to be initialized when the game starts, and to be ticked every frame.  You can turn these features
	// off to improve performance if you don't need them.
	PrimaryComponentTick.bCanEverTick = false;

	//Render target
	RenderTarget = NewObject<UTextureRenderTarget2D>();

	RenderTarget->InitAutoFormat(1024, 1024);
	RenderTarget->RenderTargetFormat = RTF_R16f;
	RenderTarget->ClearColor = FColor::Black;
	RenderTarget->bAutoGenerateMips = false;
	UKismetRenderingLibrary::ClearRenderTarget2D(this, RenderTarget, FColor::Black);

	//Scene capture
	/* Consider using a custom depth/stencil mask but be wary of performance, already using another render for each object
	*/
	if (SceneCapture) {
		SceneCapture->ProjectionType = ECameraProjectionMode::Orthographic;
		SceneCapture->OrthoWidth = SceneCaptureOrthoWidth;
		SceneCapture->AutoPlaneShift = false;
		SceneCapture->TextureTarget = RenderTarget;
		SceneCapture->CompositeMode = ESceneCaptureCompositeMode::SCCM_Additive;
		SceneCapture->bCaptureEveryFrame = false;
		SceneCapture->bCaptureOnMovement = false;
		SceneCapture->SetRelativeLocationAndRotation(FVector(0.0f, 0.0f, 200.f), FRotator(0.0f, -90.0f, -90.0f));
	}

	// Unwrap material
	if (UnwrapMaterial) {
		UnwrapMaterialInstance = UMaterialInstanceDynamic::Create(UnwrapMaterial, this);
		UnwrapMaterialInstance->SetScalarParameterValue(TEXT("CaptureSize"), SceneCaptureOrthoWidth);
		UnwrapMaterialInstance->SetVectorParameterValue(TEXT("UnwrapLocation"), UnwrapLocation);
	}
}


// Called when the game starts
void UPaintingComponent::BeginPlay()
{
	Super::BeginPlay();

}


// Called every frame
void UPaintingComponent::TickComponent(float DeltaTime, ELevelTick TickType, FActorComponentTickFunction* ThisTickFunction)
{
	Super::TickComponent(DeltaTime, TickType, ThisTickFunction);

	// ...
}

void UPaintingComponent::PaintActor(FVector HitLocation, float BrushRadius)
{
	GEngine->AddOnScreenDebugMessage(-1, 15, FColor::Green, TEXT("Painting"));


	UStaticMeshComponent* MeshToPaint = GetOwner()->GetComponentByClass<UStaticMeshComponent>();
	if (!MeshToPaint) {
		GEngine->AddOnScreenDebugMessage(-1, 15, FColor::Red, FString::Printf(TEXT("%s does not contain a static mesh"), *GetOwner()->GetName()));
		return;
	}

	if (!UnwrapMaterial) {
		GEngine->AddOnScreenDebugMessage(-1, 15, FColor::Red, FString::Printf(TEXT("%s does not reference the parent unwrap material"), *GetOwner()->GetName()));
		return;
	}

	UMaterialInterface* OriginalMaterial = MeshToPaint->GetMaterial(0);

	UnwrapMaterialInstance->SetVectorParameterValue(TEXT("HitLocation"), HitLocation);
	UnwrapMaterialInstance->SetScalarParameterValue(TEXT("BrushRadius"), BrushRadius);

	MeshToPaint->SetMaterial(0, UnwrapMaterialInstance);

	

	//After Testing
	/*SceneCapture->CaptureScene();
	MeshToPaint->SetMaterial(0, OriginalMaterial);*/
}

