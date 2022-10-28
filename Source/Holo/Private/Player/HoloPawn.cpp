// Fill out your copyright notice in the Description page of Project Settings.


#include "Player/HoloPawn.h"

#include "Components/CapsuleComponent.h"
#include "GameFramework/CharacterMovementComponent.h"
#include "Net/UnrealNetwork.h"


// Sets default values
AHoloPawn::AHoloPawn()
{
	BaseEyeHeight = 18.0f;
	bUseControllerRotationPitch = true;
	
	PrimaryActorTick.bCanEverTick = true;

	UCapsuleComponent* CollisionComponent = GetCapsuleComponent();
	if (CollisionComponent)
	{
		CollisionComponent->SetCapsuleSize(32.0f, 32.0f, false);
	}

	UCharacterMovementComponent* MovementComponent = GetCharacterMovement();
	if (MovementComponent)
	{
		MovementComponent->DefaultLandMovementMode = MOVE_Flying;
		MovementComponent->MaxAcceleration = 5000.0f;
		MovementComponent->MaxFlySpeed = 800.0f;
		MovementComponent->BrakingDecelerationFlying = 5000.0f;
	}
}

// Called when the game starts or when spawned
void AHoloPawn::BeginPlay()
{
	Super::BeginPlay();
	
}

void AHoloPawn::PostInitializeComponents()
{
	Super::PostInitializeComponents();

	// Create a dynamic material instance so we can change the color of our pawn on the fly.
	USkeletalMeshComponent* MeshComponent = GetMesh();
	if (MeshComponent)
	{
		MeshMID = MeshComponent->CreateDynamicMaterialInstance(0);
	}
}

void AHoloPawn::OnRep_Color() const
{
	if (MeshMID)
	{
		// this is the sort of thing that's better done in Blueprints, but we're handling it here for simplicity.
		MeshMID->SetVectorParameterValue(TEXT("Color"), Color);
	}
}

void AHoloPawn::OnFire()
{
}

void AHoloPawn::OnMoveForward(float AxisValue)
{
	if (AxisValue != 0.0f)
	{
		const FRotator ControlRotation = Controller ? Controller->GetControlRotation() : FRotator::ZeroRotator;
		const FVector ViewForward = FRotationMatrix(ControlRotation).GetScaledAxis(EAxis::X);
		AddMovementInput(ViewForward, AxisValue);
	}
}

void AHoloPawn::OnMoveRight(float AxisValue)
{
	if (AxisValue != 0.0f)
	{
		const FRotator ControlRotation = Controller ? Controller->GetControlRotation() : FRotator::ZeroRotator;
		const FVector ViewRight = FRotationMatrix(ControlRotation).GetScaledAxis(EAxis::Y);
		AddMovementInput(ViewRight, AxisValue);
	}
}

void AHoloPawn::OnMoveUp(float AxisValue)
{
	if (AxisValue != 0.0f)
	{
		AddMovementInput(FVector::UpVector, AxisValue);
	}
}

void AHoloPawn::OnLookRight(float AxisValue)
{
	AddControllerYawInput(AxisValue);
}

void AHoloPawn::OnLookUp(float AxisValue)
{
	AddControllerPitchInput(AxisValue);
}

void AHoloPawn::OnLookRightRate(float AxisValue)
{
	const float BaseRate = 45.0f;
	const float ScaledRate = BaseRate * AxisValue;
	const float DeltaTime = GetWorld()->GetDeltaSeconds() * CustomTimeDilation;
	AddControllerYawInput(ScaledRate * DeltaTime);
}

void AHoloPawn::OnLookUpRate(float AxisValue)
{
	const float BaseRate = 45.0f;
	const float ScaledRate = BaseRate * AxisValue;
	const float DeltaTime = GetWorld()->GetDeltaSeconds() * CustomTimeDilation;
	AddControllerPitchInput(ScaledRate * DeltaTime);
}

// Called every frame
void AHoloPawn::Tick(float DeltaTime)
{
	Super::Tick(DeltaTime);
}

// Called to bind functionality to input
void AHoloPawn::SetupPlayerInputComponent(UInputComponent* PlayerInputComponent)
{
	check(PlayerInputComponent);

	// Bind weapon actions
	PlayerInputComponent->BindAction(TEXT("Fire"), IE_Pressed, this, &AHoloPawn::OnFire);

	// Bind movement inputs (mostly parroted from DefaultPawn.cpp)
	PlayerInputComponent->BindAxis(TEXT("MoveForward"), this, &AHoloPawn::OnMoveForward);
	PlayerInputComponent->BindAxis(TEXT("MoveRight"), this, &AHoloPawn::OnMoveRight);
	PlayerInputComponent->BindAxis(TEXT("MoveUp"), this, &AHoloPawn::OnMoveUp);
	PlayerInputComponent->BindAxis(TEXT("LookRight"), this, &AHoloPawn::OnLookRight);
	PlayerInputComponent->BindAxis(TEXT("LookUp"), this, &AHoloPawn::OnLookUp);
	PlayerInputComponent->BindAxis(TEXT("LookRightRate"), this, &AHoloPawn::OnLookRightRate);
	PlayerInputComponent->BindAxis(TEXT("LookUpRate"), this, &AHoloPawn::OnLookUpRate);
}

void AHoloPawn::Auth_SetColor(const FLinearColor& InColor)
{
	checkf(HasAuthority(), TEXT("AHoloPawn::Auth_SetColor called on client"));

	Color = InColor;

	OnRep_Color();
}

void AHoloPawn::GetLifetimeReplicatedProps(TArray<FLifetimeProperty>& OutLifetimeProps) const
{
	Super::GetLifetimeReplicatedProps(OutLifetimeProps);

	DOREPLIFETIME(AHoloPawn, Color);
}

