// Fill out your copyright notice in the Description page of Project Settings.

#include "FreefallingGame.h"
#include "PlayerCharacter.h"


// Sets default values
APlayerCharacter::APlayerCharacter()
{
 	// Set this pawn to call Tick() every frame.  You can turn this off to improve performance if you don't need it.
	PrimaryActorTick.bCanEverTick = true;

	PlayerSize = 45.0f;
	DefaultCameraDistance = 2000.0f;

	AccelRate = 2000.0f;
	MaxVelocity = 2000.0f;
	Gravity = FVector(0.0f, 0.0f, -980.0f);
	JetpackPower = 2000.0f;

	CanUseJetpack = true;

	SphereComponent = CreateDefaultSubobject<USphereComponent>(TEXT("Collision Sphere"));
	SphereComponent->SetupAttachment(RootComponent);
	SphereComponent->SetCollisionProfileName(TEXT("Pawn"));
	SphereComponent->SetCollisionResponseToChannel(ECC_Visibility, ECollisionResponse::ECR_Ignore);
	SphereComponent->OnComponentBeginOverlap.AddDynamic(this, &APlayerCharacter::BeginOverlap);
	SphereComponent->OnComponentEndOverlap.AddDynamic(this, &APlayerCharacter::EndOverlap);
	SphereComponent->SetSimulatePhysics(true);
	SphereComponent->SetEnableGravity(false);
	SphereComponent->SetLinearDamping(0.0f);
	SphereComponent->BodyInstance.bLockRotation = true;
	SphereComponent->BodyInstance.bLockXRotation = true;
	SphereComponent->BodyInstance.bLockYRotation = true;
	SphereComponent->BodyInstance.bLockZRotation = true;
	SphereComponent->BodyInstance.PositionSolverIterationCount = 16;
	SphereComponent->BodyInstance.VelocitySolverIterationCount = 16;
	SphereComponent->bShouldUpdatePhysicsVolume = true;

	Model = CreateDefaultSubobject<UStaticMeshComponent>(TEXT("Player Model"));
	const ConstructorHelpers::FObjectFinder<UStaticMesh> PlayerMeshFinder(TEXT("/Game/Models/Player/PlayerModel"));
	Model->SetStaticMesh(PlayerMeshFinder.Object);
	Model->SetCollisionEnabled(ECollisionEnabled::NoCollision);
	Model->SetupAttachment(SphereComponent);

	SpringArm = CreateDefaultSubobject<USpringArmComponent>(TEXT("SpringArm"));
	SpringArm->bDoCollisionTest = false;
	SpringArm->SetupAttachment(SphereComponent);

	Camera = CreateDefaultSubobject<UCameraComponent>(TEXT("Camera"));
	Camera->SetupAttachment(SpringArm, USpringArmComponent::SocketName);
}

// Called when the game starts or when spawned
void APlayerCharacter::BeginPlay()
{
	Super::BeginPlay();
	
	RespawnPoint = GetActorLocation();
	
	SphereComponent->InitSphereRadius(PlayerSize);
	SpringArm->AddLocalOffset(DefaultCameraDistance*FVector::RightVector);
	SpringArm->AddLocalRotation(FRotator(0.0f, -90.0f, 0.0f));

}

void print(FString text) {
	GEngine->AddOnScreenDebugMessage(-1, 3, FColor::Cyan, text);
}

// Called every frame
void APlayerCharacter::Tick( float DeltaTime )
{
	Super::Tick( DeltaTime );

	if (Freeze) {
		SphereComponent->SetPhysicsLinearVelocity(FVector::ZeroVector);
		return;
	}

	// Clamp movement input.
	MovementInput = MovementInput.GetClampedToMaxSize(1.0f);

	// Check collision with the ground.
	CheckCollisions(DeltaTime);

	// Apply acceleration due to movement inputs.
	if (FMath::Abs(SphereComponent->GetPhysicsLinearVelocity().X) / MaxVelocity <= 1.0f) {
		SphereComponent->AddForce(MovementInput.X*AccelRate*FVector::ForwardVector, NAME_None, true);
	}

	// Apply smooth deceleration.
	FVector Velocity = SphereComponent->GetPhysicsLinearVelocity();
	Velocity.Z = 0.0f;
	float mult = FMath::Lerp(FMath::Pow(Velocity.Size() / MaxVelocity,16), 1.0f,FMath::Clamp(FMath::Pow(Velocity.Size()/MaxVelocity,4),0.0f,1.0f));
	SphereComponent->AddForce(-Velocity.GetSafeNormal()*AccelRate*mult, NAME_None, true);

	// Apply gravity.
	SphereComponent->AddForce(Gravity, NAME_None, true);

}

void APlayerCharacter::CheckCollisions(float DeltaTime)
{

	FHitResult TraceResult;
	FCollisionShape TraceShape = FCollisionShape::MakeSphere(PlayerSize/2.0f);
	GetWorld()->SweepSingleByChannel(TraceResult, GetActorLocation(), GetActorLocation() + (PlayerSize + 10.0f)*Gravity.GetSafeNormal(), FQuat::Identity, ECC_Visibility, TraceShape);
	if (TraceResult.bBlockingHit && (Gravity.GetSafeNormal() | TraceResult.ImpactNormal) < -0.7f) {
		Respawn();
	}
}

void APlayerCharacter::Respawn() {
	SetActorLocation(RespawnPoint, false, nullptr, ETeleportType::TeleportPhysics);
	SphereComponent->SetPhysicsLinearVelocity(FVector::ZeroVector);
	CanUseJetpack = true;
}

void APlayerCharacter::Teleport(FVector NewLocation) {
	SetActorLocation(NewLocation, false, nullptr, ETeleportType::TeleportPhysics);
}

void APlayerCharacter::RedirectMomemtum(FVector Direction) {
	Direction = Direction.GetSafeNormal();
	SphereComponent->SetPhysicsLinearVelocity(Direction*SphereComponent->GetPhysicsLinearVelocity().Size());
}

// Called to bind functionality to input
void APlayerCharacter::SetupPlayerInputComponent(class UInputComponent* InputComponent)
{
	Super::SetupPlayerInputComponent(InputComponent);
	InputComponent->BindAxis("MoveX", this, &APlayerCharacter::MoveX);
	InputComponent->BindAxis("MoveY", this, &APlayerCharacter::MoveY);
	InputComponent->BindAction("Jetpack", IE_Pressed, this, &APlayerCharacter::UseJetpack);
}

void APlayerCharacter::MoveX(float AxisValue) {
	MovementInput.X = AxisValue;
}

void APlayerCharacter::MoveY(float AxisValue) {
	MovementInput.Z = AxisValue;
}

void APlayerCharacter::UseJetpack() {
	if (CanUseJetpack) {
		//SphereComponent->SetPhysicsLinearVelocity(FVector::ZeroVector);
		//SphereComponent->SetPhysicsLinearVelocity(SphereComponent->GetPhysicsLinearVelocity()/2.0f);
		SphereComponent->SetPhysicsLinearVelocity(FVector::ForwardVector * (SphereComponent->GetPhysicsLinearVelocity() | FVector::ForwardVector));
		SphereComponent->AddImpulse(JetpackPower*MovementInput.GetSafeNormal(), NAME_None, true);
		CanUseJetpack = false;
	}
}

void APlayerCharacter::BeginOverlap(UPrimitiveComponent * thisguy, AActor * OtherActor, UPrimitiveComponent * OtherComp, int32 OtherBodyIndex, bool bFromSweep, const FHitResult & SweepResult)
{
}

void APlayerCharacter::EndOverlap(UPrimitiveComponent * thisguy, AActor * OtherActor, UPrimitiveComponent * OtherComp, int32 OtherBodyIndex)
{
}

