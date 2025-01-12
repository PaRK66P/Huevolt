// Fill out your copyright notice in the Description page of Project Settings.


#include "PaintingComponent.h"

#include "BasicCharacter.h"
#include "PaintRoomActor.h"

#include "Materials\MaterialInstanceDynamic.h" // For accessing dynamic material methods
#include "Engine\TextureRenderTarget2D.h" // For accessing render target methods
#include "Kismet\KismetRenderingLibrary.h" // For accessing further rendering methods
#include "Components\SceneCaptureComponent2D.h"

#include "Engine\StaticMesh.h"
#include "Engine\StaticMeshActor.h"

#include "Kismet/GameplayStatics.h"         // For GameplayStatics and line tracing

#include "Async\Async.h"
#include "RenderCommandFence.h"
#include "Runtime/RenderCore/Public/RenderingThread.h"


// Sets default values for this component's properties
UPaintingComponent::UPaintingComponent()
{
	// Set this component to be initialized when the game starts, and to be ticked every frame.  You can turn these features
	// off to improve performance if you don't need them.
	PrimaryComponentTick.bCanEverTick = false;

	//Scene capture
	/* Consider using a custom depth/stencil mask but be wary of performance, already using another render for each object
	*/
	SceneCapture = CreateDefaultSubobject<USceneCaptureComponent2D>(TEXT("Scene Capture Component Code"));
	if (GetOwner()) {
		SceneCapture->SetupAttachment(GetOwner()->GetRootComponent());
	}
	SceneCapture->ProjectionType = ECameraProjectionMode::Orthographic;
	SceneCapture->AutoPlaneShift = false;
	SceneCapture->CompositeMode = ESceneCaptureCompositeMode::SCCM_Overwrite;
	SceneCapture->bCaptureEveryFrame = false;
	SceneCapture->bCaptureOnMovement = false;
	//SceneCapture->SetWorldLocationAndRotation(FVector(PaintRoomPosition.X, PaintRoomPosition.Y, PaintRoomPosition.Z + 200.f), FRotator(-90.f, -90.f, 0.f));
	SceneCapture->SetWorldRotation(FRotator(-90.f, -90.f, 0.f));


}


// Called when the game starts
void UPaintingComponent::BeginPlay()
{

	Super::BeginPlay();

	if (!FirstRenderTarget) {
		FirstRenderTarget = NewObject<UTextureRenderTarget2D>();
		FirstRenderTarget->InitAutoFormat(1024, 1024);
		FirstRenderTarget->RenderTargetFormat = RTF_RGBA16f;
		FirstRenderTarget->ClearColor = FColor::Black;
		FirstRenderTarget->bAutoGenerateMips = false;
		UKismetRenderingLibrary::ClearRenderTarget2D(this, FirstRenderTarget, FColor::Black);
	}
	if (!SecondRenderTarget) {
		SecondRenderTarget = NewObject<UTextureRenderTarget2D>();
		SecondRenderTarget->InitAutoFormat(1024, 1024);
		SecondRenderTarget->RenderTargetFormat = RTF_RGBA16f;
		SecondRenderTarget->ClearColor = FColor::Black;
		SecondRenderTarget->bAutoGenerateMips = false;
		UKismetRenderingLibrary::ClearRenderTarget2D(this, SecondRenderTarget, FColor::Black);
	}

	TArray<AActor*> playerSearch;
	UGameplayStatics::GetAllActorsOfClass(GetWorld(), ABasicCharacter::StaticClass(), playerSearch);
	if (playerSearch.Num() < 1) {
		return;
	}
	ABasicCharacter* playerReference = (ABasicCharacter*) playerSearch[0];

	TArray<AActor*> paintRoomSearch;
	UGameplayStatics::GetAllActorsOfClass(GetWorld(), APaintRoomActor::StaticClass(), paintRoomSearch);
	if (paintRoomSearch.Num() < 1) {
		GEngine->AddOnScreenDebugMessage(-1, 15, FColor::Red, FString::Printf(TEXT("Missing paint room actor")));
		return;
	}
	PaintRoomActor = (APaintRoomActor*)paintRoomSearch[0];

	PaintRoomPosition = playerReference->GetPaintRoomPosition();

	SceneCapture->OrthoWidth = playerReference->GetSceneCaptureOrthoWidth();

	// Unwrap material
	if (!UnwrapMaterialInstance) {
		UnwrapMaterial = playerReference->GetUnwrapMaterialReference();

		UnwrapMaterialInstance = UMaterialInstanceDynamic::Create(UnwrapMaterial, this);
		UnwrapMaterialInstance->UMaterialInstanceDynamic::SetScalarParameterValue(TEXT("CaptureSize"), playerReference->GetSceneCaptureOrthoWidth());
		UnwrapMaterialInstance->UMaterialInstanceDynamic::SetVectorParameterValue(TEXT("UnwrapLocation"), PaintRoomPosition);
	}

	if (!BaseMaterialInstance) {
		BaseMaterial = playerReference->GetPaintingMaterialReference();

		BaseMaterialInstance = UMaterialInstanceDynamic::Create(BaseMaterial, this);
		BaseMaterialInstance->UMaterialInstanceDynamic::SetVectorParameterValue(TEXT("BaseColour"), FVector(1.0f, 1.0f, 1.0f));

		UStaticMeshComponent* MeshToPaint = GetOwner()->GetComponentByClass<UStaticMeshComponent>();
		MeshToPaint->SetMaterial(0, BaseMaterialInstance);
	}

	UsingFirstRenderTarget = true;
	ChangeRenderTarget(FirstRenderTarget);

	//PaintActor(FVector::Zero(), 0, FVector::Zero());
}

void UPaintingComponent::EndPlay(EEndPlayReason::Type EndPlayReason)
{
	Super::EndPlay(EndPlayReason);

	/*if (RenderTarget) {
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
	}*/
}

void UPaintingComponent::ChangeRenderTarget(UTextureRenderTarget2D* renderTarget)
{
	SceneCapture->TextureTarget = renderTarget;

	BaseMaterialInstance->UMaterialInstanceDynamic::SetTextureParameterValue(TEXT("PaintablePaintCapture"), renderTarget);
}

// Called every frame
void UPaintingComponent::TickComponent(float DeltaTime, ELevelTick TickType, FActorComponentTickFunction* ThisTickFunction)
{
	Super::TickComponent(DeltaTime, TickType, ThisTickFunction);

	// ...
}

void UPaintingComponent::PaintActor(FVector HitLocation, float BrushRadius, FVector projectilePaintColour)
{
	if (UsingFirstRenderTarget) {
		PaintRoomActor->SetPreviousTexture(FirstRenderTarget);
		ChangeRenderTarget(SecondRenderTarget);
	}
	else{
		PaintRoomActor->SetPreviousTexture(SecondRenderTarget);
		ChangeRenderTarget(FirstRenderTarget);
	}

	UsingFirstRenderTarget = !UsingFirstRenderTarget;

	//// Painting

	FVector originalPosition = GetOwner()->GetActorLocation();
	//FVector originalScale = GetOwner()->GetActorScale3D();
	//FRotator originalRotation = GetOwner()->GetActorRotation();

	GetOwner()->SetActorLocation(PaintRoomPosition);
	//GetOwner()->SetActorScale3D(originalScale / (originalScale.X > originalScale.Y ? originalScale.X > originalScale.Z ? originalScale.X : originalScale.Z : originalScale.Y > originalScale.Z ? originalScale.Y : originalScale.Z));
	//GetOwner()->SetActorRotation(FRotator::ZeroRotator);

	SceneCapture->SetWorldLocation(FVector(PaintRoomPosition.X, PaintRoomPosition.Y, PaintRoomPosition.Z + 200.f));
	SceneCapture->SetWorldRotation(FRotator(-90.f, -90.f, 0.f));
	SceneCapture->SetWorldScale3D(FVector(1.0f, 1.0f, 1.0f));
	
	UStaticMeshComponent* MeshToPaint = GetOwner()->GetComponentByClass<UStaticMeshComponent>();
	if (!MeshToPaint) {
		GEngine->AddOnScreenDebugMessage(-1, 15, FColor::Red, FString::Printf(TEXT("%s does not contain a static mesh"), *GetOwner()->GetName()));
		return;
	}

	if (!UnwrapMaterialInstance) {
		GEngine->AddOnScreenDebugMessage(-1, 15, FColor::Red, FString::Printf(TEXT("%s does not reference the parent unwrap material"), *GetOwner()->GetName()));
		return;
	}


	FVector HitDrawLocation = HitLocation + PaintRoomPosition - originalPosition;


	UnwrapMaterialInstance->UMaterialInstanceDynamic::SetVectorParameterValue(TEXT("HitLocation"), HitDrawLocation);
	UnwrapMaterialInstance->UMaterialInstanceDynamic::SetScalarParameterValue(TEXT("BrushRadius"), BrushRadius);
	UnwrapMaterialInstance->UMaterialInstanceDynamic::SetVectorParameterValue(TEXT("PaintColour"), projectilePaintColour);

	MeshToPaint->SetMaterial(0, UnwrapMaterialInstance);

	SceneCapture->CaptureScene();

	MeshToPaint->SetMaterial(0, BaseMaterialInstance);

	/*AsyncTask(ENamedThreads::GameThread, [this, HitDrawLocation, BrushRadius, projectilePaintColour, MeshToPaint]() {

		FRenderCommandFence RenderFence;

		

		});*/
	

	GetOwner()->SetActorLocation(originalPosition);
	//GetOwner()->SetActorScale3D(originalScale);
	//GetOwner()->SetActorRotation(originalRotation);
}

UTextureRenderTarget2D* UPaintingComponent::GetRenderTarget()
{
	return UsingFirstRenderTarget ? FirstRenderTarget : SecondRenderTarget;
}

