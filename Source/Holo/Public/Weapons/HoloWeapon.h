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

	//////////////////////////////////////////////////////////////////////////
	// VFX & SFX
	//////////////////////////////////////////////////////////////////////////

	/** Visual effect to play (at the muzzle) when the weapon is fired. */
	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category="Firing")
	UParticleSystem* FireEffect;

	/** Particle system spawned when the weapon hits something (with +X oriented along the impact normal). */
	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category="Firing")
	UParticleSystem* ImpactEffect;

	/** Sound to play when the weapon is fired. */
	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category="Firing")
	USoundBase* FireSound;

	/** Sound to play when the weapon hits an actor and successfully deals damage. */
	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category="Firing")
	USoundBase* DamagingImpactSound;

	/** Sound to play when the weapon hits an inert surface. */
	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category="Firing")
	USoundBase* NonDamagingImpactSound;

	//////////////////////////////////////////////////////////////////////////
	// Aim
	//////////////////////////////////////////////////////////////////////////

	/** How far into the scene we'll trace in order to figure out what the player is aiming at with their weapon. */
	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category="Firing")
	float AimTraceDistance;

private:

	/** Replicated to non-owning clients: contains information about the last fire event that the server generated for this weapon. */
	UPROPERTY(Transient, ReplicatedUsing=OnRep_HitNotify)
	FInstantHitInfo HitNotify;

	/** Game time when the weapon was last fired, for cooldown checks. */
	float LastFireTime;

	void PlayFireEffects();
	void PlayImpactEffects(const FVector& ImpactPoint, const FVector& ImpactNormal, bool bCausedDamage);
	bool RunFireTrace(FHitResult& OutHit);

	UFUNCTION()
	void OnRep_HitNotify();
};
