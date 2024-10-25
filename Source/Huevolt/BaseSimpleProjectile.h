// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "GameFramework/Actor.h"
#include "BaseSimpleProjectile.generated.h"

class USphereComponent;
class UProjectileMovementComponent;

UCLASS(Config = "Game")
class HUEVOLT_API ABaseSimpleProjectile : public AActor
{
	GENERATED_BODY()
	
	// Sphere collision component
	UPROPERTY(VisibleDefaultsOnly, Category = "Projectile")
	USphereComponent* CollisionComp;

	// Projectile movement component
	UPROPERTY(EditAnywhere, BlueprintReadOnly, Category = "Movement", meta = (AllowPrivateAccess = "true"))
	UProjectileMovementComponent* ProjectileMovement;

	UPROPERTY(EditAnywhere)
	class UStaticMeshComponent* ProjectileMesh;

public:	
	// Sets default values for this actor's properties
	ABaseSimpleProjectile();

	// Called when projectile hits something
	UFUNCTION()
	void OnHit(UPrimitiveComponent* HitComp, AActor* OtherActor, UPrimitiveComponent* OtherComp, FVector NormalImpulse, const FHitResult& Hit);

	// Returns CollisionComp subobject
	USphereComponent* GetCollisionComp() const { return CollisionComp; }
	// Returns ProjectileMovement subobject
	UProjectileMovementComponent* GetProjectileMovement() const { return ProjectileMovement; }

	
};
