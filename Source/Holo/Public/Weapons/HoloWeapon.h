// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "GameFramework/Actor.h"
#include "HoloWeapon.generated.h"

USTRUCT(BlueprintType)
struct FInstantHitInfo
{
	GENERATED_BODY()

	/** Game time on the server when this fire event occurred. */
	UPROPERTY()
	float ServerFireTime;

	/** Whether the weapon hit an actor and caused damage. */
	UPROPERTY()
	bool bCausedDamage;

	/** World-space location where the blocking hit occurred, if any. */
	UPROPERTY()
	FVector_NetQuantize ImpactPoint;

	/** World-space surface normal of the hit: if zero, no hit occurred. */
	UPROPERTY()
	FVector_NetQuantizeNormal ImpactNormal;
};

UCLASS()
class HOLO_API AHoloWeapon : public AActor
{
	GENERATED_BODY()

public:
	AHoloWeapon();
	virtual void BeginPlay() override;
	virtual void Tick(float DeltaTime) override;

	/** Update aiming location, find the closest blocking geometry that's centered in front of our view */
	void UpdateAimLocation(FVector& ViewLocation, FTransform& ViewTransform);

	/** Update the rotation of the weapon to show what the player is aiming at */
	void AdjustWeaponRotation();

	//////////////////////////////////////////////////////////////////////////
	// Weapon usage
	//////////////////////////////////////////////////////////////////////////

	void HandleFireInput();

	UFUNCTION(Server, Reliable)
	void Server_TryFire(const FVector& MuzzleLocation, const FVector& Direction);
	
protected:

	UPROPERTY(VisibleAnywhere, BlueprintReadOnly, Category="Components")
	UStaticMeshComponent* MeshComponent;

	/** Positioned at the end of the weapon, where line traces should originate from (and where muzzle flash effects etc. should be spawned).*/
	UPROPERTY(VisibleAnywhere, BlueprintReadOnly, Category="Components")
	USceneComponent* MuzzleHandle;

	/** How long we're required to wait between successive shots. */
	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category="Firing")
	float FireCooldown;

	/** How much base damage does a weapon do */
	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category="Firing")
	float BaseDamage;

	//////////////////////////////////////////////////////////////////////////
	// VFX & SFX
	//////////////////////////////////////////////////////////////////////////

	/** Visual effect to play (at the muzzle) when the weapon is fired. */
	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category="Effects")
	UParticleSystem* FireEffect;

	/** Particle system spawned when the weapon hits something (with +X oriented along the impact normal). */
	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category="Effects")
	UParticleSystem* ImpactEffect;

	/** Sound to play when the weapon is fired. */
	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category="Effects")
	USoundBase* FireSound;

	/** Sound to play when the weapon hits an actor and successfully deals damage. */
	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category="Effects")
	USoundBase* DamagingImpactSound;

	/** Sound to play when the weapon hits an inert surface. */
	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category="Effects")
	USoundBase* NonDamagingImpactSound;

	UPROPERTY(EditDefaultsOnly, Category=Effects)
	TSubclassOf<UCameraShakeBase> FireCameraShake;

	//////////////////////////////////////////////////////////////////////////
	// Aim
	//////////////////////////////////////////////////////////////////////////

	/** How quickly the weapon will rotate to orient itself toward the point where the player is aiming. */
	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category="Aiming")
	float AimInterpSpeed;

	/** Alternative rotation interp speed used when dropping the weapon (because the player is aiming at a point that's too close or is otherwise invalid). */
	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category="Aiming")
	float DropInterpSpeed;

	/** Local-space rotation that the weapon will adopt when it's not being aimed at a valid point in the world. */
	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category="Aiming")
	FRotator DropRotation;

	/** How far into the scene we'll trace in order to figure out what the player is aiming at with their weapon. */
	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category="Firing")
	float AimTraceDistance;

	/** World-space location representing where the player is aiming the weapon. */
	UPROPERTY(Transient, VisibleAnywhere, BlueprintReadOnly, Category="Aiming|State")
	FVector AimLocation;

	/** Indicates whether AimLocation is a point we should actually aim the weapon at: if not, the weapon will drop down (into the DropRotation) until the aim location becomes valid again. */
	UPROPERTY(Transient, VisibleAnywhere, BlueprintReadOnly, Category="Aiming|State")
	bool bAimLocationIsValid;

	/** Checks if we can fire */
	bool CanFire() const;

private:

	/** Replicated to non-owning clients: contains information about the last fire event that the server generated for this weapon. */
	UPROPERTY(Transient, ReplicatedUsing=OnRep_HitNotify)
	FInstantHitInfo HitNotify;

	/** Game time when the weapon was last fired, for cooldown checks. */
	float LastFireTime;

	// Effects
	void PlayFireEffects() const;
	void PlayImpactEffects(const FVector& ImpactPoint, const FVector& ImpactNormal, bool bCausedDamage);
	bool RunFireTrace(FHitResult& OutHit);

	UFUNCTION()
	void OnRep_HitNotify();
};
