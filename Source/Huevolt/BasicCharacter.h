// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "GameFramework/Character.h"

#include "BasicCharacter.generated.h"

class APaintRoomActor;
class UCameraComponent;
class UInputAction;
class UColourCheckComponent;
class UPlayerPowerComponent;
struct FInputActionValue;

UCLASS()
class HUEVOLT_API ABasicCharacter : public ACharacter
{
	GENERATED_BODY()

public:

	/** First person camera */
	UPROPERTY(VisibleAnywhere, BlueprintReadOnly, Category = Camera, meta = (AllowPrivateAccess = "true"))
	UCameraComponent* FirstPersonCameraComponent;

	/** Move Input Action */
	UPROPERTY(EditAnywhere, BlueprintReadOnly, Category = Input, meta = (AllowPrivateAccess = "true"))
	UInputAction* MoveAction;

	/** Look Input Action */
	UPROPERTY(EditAnywhere, BlueprintReadOnly, Category = Input, meta = (AllowPrivateAccess = "true"))
	UInputAction* LookAction;

	/** Shoot Input Action */
	UPROPERTY(EditAnywhere, BlueprintReadOnly, Category = Input, meta = (AllowPrivateAccess = "true"))
	UInputAction* ShootAction;

	/** Colour change Input Action */
	UPROPERTY(EditAnywhere, BlueprintReadOnly, Category = Input, meta = (AllowPrivateAccess = "true"))
	UInputAction* ColourChangeAction;

	/** Projectile class to spawn */
	UPROPERTY(EditDefaultsOnly, Category = Projectile)
	TSubclassOf<class ABasicProjectile> ProjectileClass;

	/** Gun muzzle's offset from the characters location */
	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = Gameplay)
	FVector MuzzleOffset;

	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = Painting)
	UMaterial* UnwrapMaterial;

	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = Painting)
	UMaterial* PaintingMaterial;

	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = Painting)
	float SceneCaptureOrthoWidth;

	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = Painting)
	FVector PaintRoomPosition;

	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = Painting)
	TArray<FLinearColor> ColoursForPainting;

	UPROPERTY(VisibleAnywhere, BlueprintReadOnly, Category = Painting, meta = (AllowPrivateAccess = "true"))
	UColourCheckComponent* ColourCheckComponent;

	UPROPERTY(VisibleAnywhere, BlueprintReadOnly, Category = Painting, meta = (AllowPrivateAccess = "true"))
	UPlayerPowerComponent* PlayerPowerComponent;

public:
	// Sets default values for this character's properties
	ABasicCharacter();

protected:
	// Called when the game starts or when spawned
	virtual void BeginPlay() override;

	int32 NumberOfColours;
	int CurrentColourIndex = 0;

	FLinearColor CurrentColour;

	float BaseAcceleration;
	float BaseSpeed;

	float ShootDelay;
	float ShootFireRate = 0.1f;

	APaintRoomActor* PaintRoomActorReference;

public:	
	// Called every frame
	virtual void Tick(float DeltaTime) override;

	// Called to bind functionality to input
	virtual void SetupPlayerInputComponent(class UInputComponent* PlayerInputComponent) override;

	/** Called for movement input */
	void Move(const FInputActionValue& Value);

	/** Called for looking input */
	void Look(const FInputActionValue& Value);

	/** Called for shooting input */
	void Shoot(const FInputActionValue& Value);

	void ChangeColour(const FInputActionValue& Value);

	void ModifySpeed(float modifier);

	void Bounce();
	void BounceReset();

	UFUNCTION(BlueprintCallable)
	FLinearColor GetColour() { return CurrentColour; }
	APaintRoomActor* GetPaintRoomActorReference() { return PaintRoomActorReference; }
	FVector GetPaintRoomPosition() { return PaintRoomPosition; }
	UMaterial* GetUnwrapMaterialReference() { return UnwrapMaterial; }
	UMaterial* GetPaintingMaterialReference() { return PaintingMaterial; }
	float GetSceneCaptureOrthoWidth() { return SceneCaptureOrthoWidth; }
	TArray<FLinearColor> GetColourList() { return ColoursForPainting; }


};
