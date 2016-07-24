// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "GameFramework/Pawn.h"
#include "PlayerCharacter.generated.h"

UCLASS()
class FREEFALLINGGAME_API APlayerCharacter : public APawn
{
	GENERATED_BODY()

public:
	// Sets default values for this pawn's properties
	APlayerCharacter();

	// Called when the game starts or when spawned
	virtual void BeginPlay() override;
	
	// Called every frame
	virtual void Tick( float DeltaSeconds ) override;

	// Called to bind functionality to input
	virtual void SetupPlayerInputComponent(class UInputComponent* InputComponent) override;

	UFUNCTION() void BeginOverlap(class UPrimitiveComponent* thisguy, class AActor* OtherActor, class UPrimitiveComponent* OtherComp, int32 OtherBodyIndex, bool bFromSweep, const FHitResult &SweepResult);
	UFUNCTION() void EndOverlap(class UPrimitiveComponent* thisguy, class AActor* OtherActor, class UPrimitiveComponent* OtherComp, int32 OtherBodyIndex);

	UFUNCTION() void MoveX(float AxisValue);
	UFUNCTION() void MoveY(float AxisValue);
	UFUNCTION() void UseJetpack();

	UFUNCTION() void CheckCollisions(float DeltaTime);

	UFUNCTION(BlueprintCallable, Category = "Free Fall Player Functions") void Teleport(FVector NewLocation);
	UFUNCTION(BlueprintCallable, Category = "Free Fall Player Functions") void RedirectMomemtum(FVector Direction);
	UFUNCTION(BlueprintCallable, Category = "Free Fall Player Functions") void Respawn();

	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Components") USphereComponent* SphereComponent;
	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Components") USkeletalMeshComponent* Model;
	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Components") USpringArmComponent* SpringArm;
	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Components") UCameraComponent* Camera;
	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Components") UParticleSystemComponent* JetpackParticles;

	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Settings") float PlayerSize;
	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Settings") float DefaultCameraDistance;

	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Settings") float AccelRate;
	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Settings") float MaxVelocity;
	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Settings") FVector Gravity;
	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Settings") float JetpackPower;

	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "State") FVector RespawnPoint;
	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "State") FVector MovementInput;
	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "State") bool CanUseJetpack;
	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "State") bool Freeze;
	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "State") float TargetYaw;


};
