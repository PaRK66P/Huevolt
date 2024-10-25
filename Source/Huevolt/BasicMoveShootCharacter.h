// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "GameFramework/Character.h"
#include "BasicMoveShootCharacter.generated.h"

class UInputComponent;
class UCameraComponent;
class UInputAction;
class UInputMappingContext;
struct FInputActionValue;

DECLARE_LOG_CATEGORY_EXTERN(LogTemplateCharacter, Log, All);

UCLASS(Config = "Game")
class HUEVOLT_API ABasicMoveShootCharacter : public ACharacter
{
	GENERATED_BODY()

	// Camera
	UPROPERTY(VisibleAnywhere, BlueprintReadOnly, Category= "Camera", meta = (AllowPrivateAccess = "true"))
	UCameraComponent* CameraComponent;

	// Projectile class to spawn
	UPROPERTY(EditDefaultsOnly, Category = "Projectile")
	TSubclassOf<class ABaseSimpleProjectile> ProjectileClass;

	// Move Input Action
	UPROPERTY(EditAnywhere, BlueprintReadOnly, Category = "Input", meta = (AllowPrivateAccess = "true"));
	UInputAction* MoveAction;

	// Look Input Action
	UPROPERTY(EditAnywhere, BlueprintReadOnly, Category = "Input", meta = (AllowPrivateAccess = "true"));
	UInputAction* LookAction;

	// Shoot Input Action
	UPROPERTY(EditAnywhere, BlueprintReadOnly, Category = "Input", meta = (AllowPrivateAccess = "true"));
	UInputAction* ShootAction;

public:
	// Sets default values for this character's properties
	ABasicMoveShootCharacter();

protected:
	// Called when the game starts or when spawned
	virtual void BeginPlay() override;

	// Called for movement input
	void Move(const FInputActionValue& Value);

	// Called for looking input
	void Look(const FInputActionValue& Value);

	// Called for shooting input
	void Shoot(const FInputActionValue& Value);

	// APawn interface
	virtual void SetupPlayerInputComponent(UInputComponent* InputComponent) override;
	// End of APawnInterface

public:	
	// Returns FirstPersonCameraComponent subobject
	UCameraComponent* GetCameraComponent() const { return CameraComponent; }

};
