// Fill out your copyright notice in the Description page of Project Settings.


#include "Player/HoloPawn.h"

#include "Blueprint/UserWidget.h"
#include "Components/CapsuleComponent.h"
#include "Core/HoloGameMode.h"
#include "GameFramework/CharacterMovementComponent.h"
#include "Net/UnrealNetwork.h"
#include "Player/HoloHealthComponent.h"
#include "Player/HoloPlayerController.h"
#include "UI/HoloGameLayoutWidget.h"
#include "Weapons/HoloWeapon.h"


// Sets default values
AHoloPawn::AHoloPawn()
{
	BaseEyeHeight = 18.0f;
	bUseControllerRotationPitch = true;
	PrimaryActorTick.bCanEverTick = true;

	// Change the default CharacterMovementComponent behavior: we want our player pawns to be able to fly around freely
	UCharacterMovementComponent* MovementComponent = GetCharacterMovement();
	if (MovementComponent)
	{
		MovementComponent->DefaultLandMovementMode = MOVE_Flying;
		MovementComponent->MaxAcceleration = 5000.0f;
		MovementComponent->MaxFlySpeed = 800.0f;
		MovementComponent->BrakingDecelerationFlying = 5000.0f;
	}

	// Create an additional SceneComponent and position it where we want the root of the Weapon actor to be attached.
	USkeletalMeshComponent* MeshComponent = GetMesh();
	WeaponHandle = CreateDefaultSubobject<USceneComponent>(TEXT("WeaponHandle"));
	WeaponHandle->SetUsingAbsoluteScale(true);
	WeaponHandle->SetupAttachment(MeshComponent ? MeshComponent : RootComponent);
	WeaponHandle->SetRelativeLocation(FVector(15.0f, 8.0f, 6.0f));

	HealthComponent = CreateDefaultSubobject<UHoloHealthComponent>(TEXT("HealthComponent"));
	HealthComponent->SetIsReplicated(true);
}

// Called when the game starts or when spawned
void AHoloPawn::BeginPlay()
{
	Super::BeginPlay();

	if (HasAuthority())
	{
		// Spawn default weapon
		checkf(DefaultWeaponClass, TEXT("DefaultWeaponClass is not set"));
		Auth_SpawnWeapon(DefaultWeaponClass);
	}

	if (IsLocallyControlled())
	{
		checkf(GameLayoutWidgetClass, TEXT("GameLayoutWidgetClass is not set!"));

		GameLayoutWidget = CreateWidget<UHoloGameLayoutWidget>(GetGameInstance(), GameLayoutWidgetClass);
		GameLayoutWidget->AddToViewport();
	}
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

float AHoloPawn::TakeDamage(float DamageAmount, FDamageEvent const& DamageEvent, AController* EventInstigator, AActor* DamageCauser)
{
	const float ActualDamage = Super::TakeDamage(DamageAmount, DamageEvent, EventInstigator, DamageCauser);
	if (ActualDamage > 0.0f && HealthComponent)
	{
		HealthComponent->ApplyDamage(ActualDamage, DamageEvent, EventInstigator, DamageCauser);
	}

	return ActualDamage;
}

void AHoloPawn::OnRep_Color()
{
	if (MeshMID)
	{
		// this is the sort of thing that's better done in Blueprints, but we're handling it here for simplicity.
		MeshMID->SetVectorParameterValue(TEXT("Color"), Color);
	}

	if (OnColorChangedDelegate.IsBound())
	{
		OnColorChangedDelegate.Broadcast(Color);
	}
}

void AHoloPawn::OnRep_Weapon()
{
	if (Weapon)
	{
		Weapon->AttachToComponent(WeaponHandle, FAttachmentTransformRules::SnapToTargetIncludingScale);
		Weapon->AddTickPrerequisiteActor(this);
	}
}

void AHoloPawn::OnFire()
{
	if (Weapon && !bIsDying)
	{
		Weapon->HandleFireInput();
	}
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

	// Update weapon aim location
	if (Weapon)
	{
		FVector ViewLocation = GetPawnViewLocation();
		FTransform ViewTransform = GetMesh() ? GetMesh()->GetComponentTransform() : GetActorTransform();
		Weapon->UpdateAimLocation(ViewLocation, ViewTransform);
	}
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

UHoloHealthComponent* AHoloPawn::GetHealthComponent() const
{
	return HealthComponent;
}

void AHoloPawn::Auth_SpawnWeapon(TSubclassOf<AHoloWeapon> WeaponClass)
{
	checkf(HasAuthority(), TEXT("AHoloPawn::Auth_SpawnWeapon called on client"));

	FActorSpawnParameters SpawnInfo;
	SpawnInfo.Owner = this;
	SpawnInfo.Instigator = this;
	SpawnInfo.SpawnCollisionHandlingOverride = ESpawnActorCollisionHandlingMethod::AlwaysSpawn;

	const FVector SpawnLocation = WeaponHandle->GetComponentLocation();
	const FRotator SpawnRotation = WeaponHandle->GetComponentRotation();
	Weapon = GetWorld()->SpawnActor<AHoloWeapon>(WeaponClass, SpawnLocation, SpawnRotation, SpawnInfo);

	OnRep_Weapon();
}


bool AHoloPawn::Die(float KillingDamage, FDamageEvent const& DamageEvent, AController* Killer, AActor* DamageCauser)
{
	if (!CanDie(KillingDamage, DamageEvent, Killer, DamageCauser))
	{
		return false;
	}

	OnDeath(KillingDamage, DamageEvent, Killer ? Killer->GetPawn() : nullptr, DamageCauser);

	return true;
}

bool AHoloPawn::CanDie(float KillingDamage, FDamageEvent const& DamageEvent, AController* Killer, AActor* DamageCauser) const
{
	if (bIsDying										// already dying
		|| IsPendingKill()								// already destroyed
		|| GetLocalRole() != ROLE_Authority				// not authority
		|| GetWorld()->GetAuthGameMode<AHoloGameMode>() == nullptr)
	{
		return false;
	}

	return true;
}

void AHoloPawn::OnRep_IsDying()
{
	GetCapsuleComponent()->SetCollisionEnabled(ECollisionEnabled::NoCollision);
	
	if (Weapon)
	{
		Weapon->SetActorTickEnabled(false);
	}

	// visual effects
	SetRagdollPhysics();
	GetMesh()->AddTorqueInRadians(FVector(10000000.0));

	if (DeathCameraShake)
	{
		PlayCameraShake(DeathCameraShake);
	}
}

void AHoloPawn::RestartPlayer()
{
	AHoloPlayerController* PC = CastChecked<AHoloPlayerController>(GetController());

	if (Weapon)
	{
		Weapon->Destroy();
	}
	
	Destroy();
	
	PC->Respawn();
}

void AHoloPawn::OnDeath(float KillingDamage, FDamageEvent const& DamageEvent, APawn* InstigatingPawn, AActor* DamageCauser)
{
	bIsDying = true;
	OnRep_IsDying();

	UCharacterMovementComponent* MoveComponent = GetCharacterMovement();
	MoveComponent->StopMovementImmediately();
	MoveComponent->DisableMovement();
	MoveComponent->SetComponentTickEnabled(false);
	
	FTimerHandle TimerHandle_Restart;
	GetWorldTimerManager().SetTimer(TimerHandle_Restart, this, &AHoloPawn::RestartPlayer, 3.0f, false);
}

void AHoloPawn::SetRagdollPhysics()
{
	if (IsPendingKill() || !GetMesh() || !GetMesh()->GetPhysicsAsset())
	{
		return;
	}

	// initialize physics/etc
	GetMesh()->SetSimulatePhysics(true);
	GetMesh()->WakeAllRigidBodies();
	GetMesh()->bBlendPhysics = true;

	static FName CollisionProfileName(TEXT("Ragdoll"));
	GetMesh()->SetCollisionProfileName(CollisionProfileName);
}

void AHoloPawn::PlayCameraShake(TSubclassOf<UCameraShakeBase> CameraShake) const
{
	AHoloPlayerController* PC = Cast<AHoloPlayerController>(GetController());
	if (PC && PC->IsLocalController())
	{
		PC->ClientStartCameraShake(CameraShake, 1);
	}
}

void AHoloPawn::GetLifetimeReplicatedProps(TArray<FLifetimeProperty>& OutLifetimeProps) const
{
	Super::GetLifetimeReplicatedProps(OutLifetimeProps);

	DOREPLIFETIME(AHoloPawn, Weapon);
	DOREPLIFETIME(AHoloPawn, Color);
	DOREPLIFETIME(AHoloPawn, bIsDying);
}

