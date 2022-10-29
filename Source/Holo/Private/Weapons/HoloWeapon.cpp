// Fill out your copyright notice in the Description page of Project Settings.


#include "Weapons/HoloWeapon.h"

#include "Kismet/GameplayStatics.h"
#include "Net/UnrealNetwork.h"


AHoloWeapon::AHoloWeapon()
{
	PrimaryActorTick.bCanEverTick = true;
	bReplicates = true;
	bNetUseOwnerRelevancy = true;

	// Define default cooldown/firing properties
	FireCooldown = 0.4f;
	LastFireTime = TNumericLimits<float>::Lowest();
	AimTraceDistance = 5000.0f;

	// Create components
	RootComponent = CreateDefaultSubobject<USceneComponent>(TEXT("RootComponent"));

	MeshComponent = CreateDefaultSubobject<UStaticMeshComponent>(TEXT("MeshComponent"));
	MeshComponent->SetupAttachment(RootComponent);
	MeshComponent->SetCollisionProfileName(UCollisionProfile::NoCollision_ProfileName);
	MeshComponent->SetRelativeLocation(FVector(20.0f, 0.0f, 0.0f));
	MeshComponent->SetRelativeRotation(FRotator(0.0f, 0.0f, 45.0f));
	MeshComponent->SetRelativeScale3D(FVector(0.5f, 0.15f, 0.15f));

	MuzzleHandle = CreateDefaultSubobject<USceneComponent>(TEXT("MuzzleHandle"));
	MuzzleHandle->SetupAttachment(RootComponent);
	MuzzleHandle->SetRelativeLocation(FVector(50.0f, 0.0f, 0.0f));
}

// Called when the game starts or when spawned
void AHoloWeapon::BeginPlay()
{
	Super::BeginPlay();
	
}

// Called every frame
void AHoloWeapon::Tick(float DeltaTime)
{
	Super::Tick(DeltaTime);
}


void AHoloWeapon::HandleFireInput()
{
	const float CurrentTime = GetWorld()->GetTimeSeconds();
	const float ElapsedSinceLastFire = CurrentTime - LastFireTime;
	if (ElapsedSinceLastFire < FireCooldown)
	{
		return;
	}

	const FVector MuzzleLocation = MuzzleHandle->GetComponentLocation();
	const FVector Direction = MuzzleHandle->GetComponentQuat().Vector();
	Server_TryFire(MuzzleLocation, Direction);
	LastFireTime = CurrentTime;

	if (!HasAuthority())
	{
		PlayFireEffects();

		// Run a cosmetic line trace just to see whether we should spawn an impact effect
		FHitResult Hit;
		if (RunFireTrace(Hit))
		{
			const bool bWillProbablyCauseDamage = Hit.Actor.IsValid() && Hit.Actor->CanBeDamaged();
			PlayImpactEffects(Hit.ImpactPoint, Hit.ImpactNormal, bWillProbablyCauseDamage);
		}
	}
}

void AHoloWeapon::PlayFireEffects()
{
	if (FireEffect)
	{
		UGameplayStatics::SpawnEmitterAttached(FireEffect, MuzzleHandle);
	}

	if (FireSound)
	{
		UGameplayStatics::PlaySoundAtLocation(GetWorld(), FireSound, MuzzleHandle->GetComponentLocation(), MuzzleHandle->GetComponentRotation());
	}
}

void AHoloWeapon::PlayImpactEffects(const FVector& ImpactPoint, const FVector& ImpactNormal, bool bCausedDamage)
{
	const FRotator ImpactRotation = ImpactNormal.ToOrientationRotator();

	if (ImpactEffect)
	{
		UGameplayStatics::SpawnEmitterAtLocation(GetWorld(), FireEffect, ImpactPoint, ImpactRotation);
	}

	USoundBase* Sound = bCausedDamage ? DamagingImpactSound : NonDamagingImpactSound;
	if (Sound)
	{
		UGameplayStatics::PlaySoundAtLocation(GetWorld(), Sound, ImpactPoint, ImpactRotation);
	}
}

bool AHoloWeapon::RunFireTrace(FHitResult& OutHit)
{
	const FVector& TraceStart = MuzzleHandle->GetComponentLocation();
	const FVector TraceEnd = TraceStart + (MuzzleHandle->GetForwardVector() * AimTraceDistance);
	const FName ProfileName = UCollisionProfile::BlockAllDynamic_ProfileName;
	const FCollisionQueryParams QueryParams(TEXT("WeaponFire"), false, GetOwner());
	return GetWorld()->LineTraceSingleByProfile(OutHit, TraceStart, TraceEnd, ProfileName, QueryParams);
}

void AHoloWeapon::OnRep_HitNotify()
{
	PlayFireEffects();

	if (!HitNotify.ImpactNormal.IsZero())
	{
		PlayImpactEffects(HitNotify.ImpactPoint, HitNotify.ImpactNormal, HitNotify.bCausedDamage);
	}
}

void AHoloWeapon::Server_TryFire_Implementation(const FVector& MuzzleLocation, const FVector& Direction)
{
	const float CurrentTime = GetWorld()->GetTimeSeconds();
	const float ElapsedSinceLastFire = CurrentTime - LastFireTime;
	if (ElapsedSinceLastFire < FireCooldown)
	{
		return;
	}

	LastFireTime = CurrentTime;

	FHitResult Hit;
	if (RunFireTrace(Hit))
	{
		// If we hit a damageable actor, attempt to damage it
		float DamageCaused = 0.0f;
		if (Hit.Actor.IsValid() && Hit.Actor->CanBeDamaged())
		{
			const float BaseDamage = 1.0f;
			const FPointDamageEvent DamageEvent(BaseDamage, Hit, Direction, UDamageType::StaticClass());
			DamageCaused = Hit.Actor->TakeDamage(BaseDamage, DamageEvent, GetInstigatorController(), this);
		}
		
		PlayFireEffects();
		PlayImpactEffects(Hit.ImpactPoint, Hit.ImpactNormal, DamageCaused > 0.0f);

		// Propagate the details of our hit to non-owning clients
		HitNotify.bCausedDamage = DamageCaused > 0.0f;
		HitNotify.ImpactPoint = Hit.ImpactPoint;
		HitNotify.ImpactNormal = Hit.ImpactNormal;
	}
	else
	{
		// Set ImpactNormal to zero as a sentinel to indicate that this shot didn't hit anything
		HitNotify.ImpactNormal = FVector::ZeroVector;
	}
}

void AHoloWeapon::GetLifetimeReplicatedProps(TArray<FLifetimeProperty>& OutLifetimeProps) const
{
	Super::GetLifetimeReplicatedProps(OutLifetimeProps);

	DOREPLIFETIME_CONDITION(AHoloWeapon, HitNotify, COND_SkipOwner);
}
