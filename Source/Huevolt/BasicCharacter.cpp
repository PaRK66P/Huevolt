// Fill out your copyright notice in the Description page of Project Settings.


#include "BasicCharacter.h"

#include "PaintRoomActor.h"

#include "Components/CapsuleComponent.h"
#include "Camera/CameraComponent.h"

#include "EnhancedInputComponent.h"
#include "GameFramework\CharacterMovementComponent.h"

#include "BasicProjectile.h"
#include "ColourCheckComponent.h"
#include "PlayerPowerComponent.h"

#include "Kismet/GameplayStatics.h" 


// Sets default values
ABasicCharacter::ABasicCharacter()
{
 	// Set this character to call Tick() every frame.  You can turn this off to improve performance if you don't need it.
	PrimaryActorTick.bCanEverTick = true;

	// Set size for collision capsule
	GetCapsuleComponent()->InitCapsuleSize(55.f, 96.0f);

	// Create a CameraComponent	
	FirstPersonCameraComponent = CreateDefaultSubobject<UCameraComponent>(TEXT("FirstPersonCamera"));
	FirstPersonCameraComponent->SetupAttachment(GetCapsuleComponent());
	FirstPersonCameraComponent->SetRelativeLocation(FVector(-10.f, 0.f, 60.f)); // Position the camera
	FirstPersonCameraComponent->bUsePawnControlRotation = true;

	// Create a ColourCheckComponent
	ColourCheckComponent = CreateDefaultSubobject<UColourCheckComponent>(TEXT("ColourCheck"));

	// Create a ColourCheckComponent
	PlayerPowerComponent = CreateDefaultSubobject<UPlayerPowerComponent>(TEXT("PlayerPower"));

	// Default offset from the character location for projectiles to spawn
	MuzzleOffset = FVector(100.0f, 0.0f, 10.0f);


	//GEngine->AddOnScreenDebugMessage(-1, 15, FColor::Green, FString::Printf(TEXT("%f, %f, %f, %f"), pixelColour.R, pixelColour.G, pixelColour.B, pixelColour.A));
}

// Called when the game starts or when spawned
void ABasicCharacter::BeginPlay()
{
	Super::BeginPlay();

	BaseAcceleration = GetCharacterMovement()->GetMaxAcceleration();
	BaseSpeed = GetCharacterMovement()->GetMaxSpeed();
	ShootDelay = 0.0f;

	TArray<AActor*> paintRoomSearch;
	UGameplayStatics::GetAllActorsOfClass(GetWorld(), APaintRoomActor::StaticClass(), paintRoomSearch);
	if (paintRoomSearch.Num() < 1) {
		GEngine->AddOnScreenDebugMessage(-1, 15, FColor::Red, FString::Printf(TEXT("Missing paint room actor")));
		return;
	}
	APaintRoomActor* paintRoomReference = (APaintRoomActor*)paintRoomSearch[0];

	paintRoomReference->InitialisePaintRoom(SceneCaptureOrthoWidth, PaintRoomPosition);

	ColourCheckComponent->SetupLineTrace(Cast<AActor>(this));

	ColourCheckComponent->InitialiseReferences(PlayerPowerComponent);

	PlayerPowerComponent->InitialiseComponent(this);

	NumberOfColours = ColoursForPainting.Max();
	if (NumberOfColours > 0) {
		CurrentColour = ColoursForPainting[0];
	}
}

// Called every frame
void ABasicCharacter::Tick(float DeltaTime)
{
	Super::Tick(DeltaTime);

	if (ShootDelay > 0.f) {
		ShootDelay -= DeltaTime;
	}
}

// Called to bind functionality to input
void ABasicCharacter::SetupPlayerInputComponent(UInputComponent* PlayerInputComponent)
{
	// Set up action bindings
	if (UEnhancedInputComponent* EnhancedInputComponent = Cast<UEnhancedInputComponent>(PlayerInputComponent))
	{
		// Moving
		EnhancedInputComponent->BindAction(MoveAction, ETriggerEvent::Triggered, this, &ABasicCharacter::Move);

		// Looking
		EnhancedInputComponent->BindAction(LookAction, ETriggerEvent::Triggered, this, &ABasicCharacter::Look);

		// Shooting
		EnhancedInputComponent->BindAction(ShootAction, ETriggerEvent::Triggered, this, &ABasicCharacter::Shoot);

		// Shooting
		EnhancedInputComponent->BindAction(ColourChangeAction, ETriggerEvent::Started, this, &ABasicCharacter::ChangeColour);
	}
}

void ABasicCharacter::Move(const FInputActionValue& Value)
{
	// input is a Vector2D
	FVector2D MovementVector = Value.Get<FVector2D>();


	if (Controller != nullptr)
	{
		// add movement 
		AddMovementInput(GetActorForwardVector(), MovementVector.Y);
		AddMovementInput(GetActorRightVector(), MovementVector.X);

		bool result = false;
		FLinearColor pixelColour = ColourCheckComponent->CheckColour(result, GetActorLocation() + FVector(0, 0, -90));
		
	}
}

void ABasicCharacter::Look(const FInputActionValue& Value)
{
	// input is a Vector2D
	FVector2D LookAxisVector = Value.Get<FVector2D>();

	if (Controller != nullptr)
	{
		// add yaw and pitch input to controller
		AddControllerYawInput(LookAxisVector.X);
		AddControllerPitchInput(LookAxisVector.Y);
	}
}

void ABasicCharacter::Shoot(const FInputActionValue& Value)
{
	if (ShootDelay > 0.0f) {
		return;
	}

	if (ProjectileClass != nullptr) 
	{
		UWorld* const World = GetWorld();
		if (World != nullptr)
		{
			APlayerController* PlayerController = Cast<APlayerController>(this->GetController());
			const FRotator SpawnRotation = PlayerController->PlayerCameraManager->GetCameraRotation();
			// MuzzleOffset is in camera space, so transform it to world space before offsetting from the character location to find the final muzzle position
			const FVector SpawnLocation = GetActorLocation() + SpawnRotation.RotateVector(MuzzleOffset);

			//Set Spawn Collision Handling Override
			FActorSpawnParameters ActorSpawnParams;
			ActorSpawnParams.SpawnCollisionHandlingOverride = ESpawnActorCollisionHandlingMethod::AdjustIfPossibleButDontSpawnIfColliding;

			// Spawn the projectile at the muzzle
			ABasicProjectile* projectileRef = World->SpawnActor<ABasicProjectile>(ProjectileClass, SpawnLocation, SpawnRotation, ActorSpawnParams);
			if (projectileRef) {
				projectileRef->ProjectileColour = FVector(CurrentColour.R, CurrentColour.G, CurrentColour.B);

				ShootDelay = ShootFireRate;
			}
		}
	}
}

void ABasicCharacter::ChangeColour(const FInputActionValue& Value)
{
	CurrentColourIndex++;
	if (CurrentColourIndex >= NumberOfColours) {
		CurrentColourIndex = 0;
	}
	CurrentColour = ColoursForPainting[CurrentColourIndex];
}

void ABasicCharacter::ModifySpeed(float modifier)
{
	GetCharacterMovement()->MaxAcceleration = BaseAcceleration * modifier;
	GetCharacterMovement()->MaxWalkSpeed = BaseSpeed * modifier;
}

void ABasicCharacter::Bounce()
{
	Jump();
}

void ABasicCharacter::BounceReset()
{
	StopJumping();
}
