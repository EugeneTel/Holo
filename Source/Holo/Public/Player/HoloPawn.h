// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "GameFramework/Character.h"
#include "HoloPawn.generated.h"

class UHoloGameLayoutWidget;
class UHoloHealthComponent;

DECLARE_DYNAMIC_MULTICAST_DELEGATE(FOnPawnDying);
DECLARE_DYNAMIC_MULTICAST_DELEGATE_OneParam(FOnPawnColorChanged, const FLinearColor&, Color);

UCLASS()
class HOLO_API AHoloPawn : public ACharacter
{
	GENERATED_BODY()

public:
	// Sets default values for this character's properties
	AHoloPawn();

	//~ Begin AActor Interface
	virtual void Tick(float DeltaTime) override;
	virtual void BeginPlay() override;
	virtual void PostInitializeComponents() override;
	//~ End AActor Interface

	//~ Begin APawn Interface
	virtual float TakeDamage(float DamageAmount, FDamageEvent const& DamageEvent, AController* EventInstigator, AActor* DamageCauser) override;
	//~ End APawn Interface

	//~ Begin ACharacter Interface
	virtual void SetupPlayerInputComponent(class UInputComponent* PlayerInputComponent) override;
	//~ End ACharacter Interface

	// Getters & Setters
	void Auth_SetColor(const FLinearColor& InColor);
	FLinearColor& GetColor() { return Color; };
	UHoloHealthComponent* GetHealthComponent() const;

	/**
	* Kills pawn.  Server/authority only.
	* @param KillingDamage - Damage amount of the killing blow
	* @param DamageEvent - Damage event of the killing blow
	* @param Killer - Who killed this pawn
	* @param DamageCauser - the Actor that directly caused the damage (i.e. the Projectile that exploded, the Weapon that fired, etc)
	* @returns true if allowed
	*/
	virtual bool Die(float KillingDamage, FDamageEvent const& DamageEvent, AController* Killer, AActor* DamageCauser);

	/** Returns True if the pawn can die in the current state */
	virtual bool CanDie(float KillingDamage, FDamageEvent const& DamageEvent, AController* Killer, AActor* DamageCauser) const;

	/** Delegate fire when the Pawn dies */
	UPROPERTY(BlueprintAssignable)
	FOnPawnDying OnDyingDelegate;
	
	UPROPERTY(BlueprintAssignable)
	FOnPawnColorChanged OnColorChangedDelegate;

	/** Identifies if pawn is in its dying state */
	UPROPERTY(ReplicatedUsing = OnRep_IsDying, BlueprintReadOnly, Category = Health)
	uint32 bIsDying : 1;

	UFUNCTION()
	void OnRep_IsDying();

	/** Destroy and restart player */
	void RestartPlayer();

protected:

	/** Scene component indicating where the pawn's Weapon should be attached. */
	UPROPERTY(VisibleAnywhere, BlueprintReadOnly, Category="Components")
	class USceneComponent* WeaponHandle;

	UPROPERTY(VisibleAnywhere, BlueprintReadOnly, Category="Components")
	UHoloHealthComponent* HealthComponent;

	/** The weapon that this player is holding, if any. */
	UPROPERTY(ReplicatedUsing=OnRep_Weapon, Transient, BlueprintReadOnly, Category="Weapon")
	class AHoloWeapon* Weapon;

	/** The weapon class that will be created on spawn */
	UPROPERTY(EditDefaultsOnly, Category="Weapon")
	TSubclassOf<AHoloWeapon> DefaultWeaponClass;

	UPROPERTY(EditDefaultsOnly, Category="Widgets")
	TSubclassOf<UHoloGameLayoutWidget> GameLayoutWidgetClass;
	
	UPROPERTY(EditDefaultsOnly, Category=Effects)
	TSubclassOf<UCameraShakeBase> DamageCameraShake;
	
	UPROPERTY(EditDefaultsOnly, Category=Effects)
	TSubclassOf<UCameraShakeBase> DeathCameraShake;

	UPROPERTY(BlueprintReadOnly, Category="Widgets")
	UHoloGameLayoutWidget* GameLayoutWidget;

	/** Material instance assigned to the character mesh, giving us control over the shader parameters at runtime. */
	UPROPERTY(BlueprintReadOnly, Category="Player")
	UMaterialInstanceDynamic* MeshMID;
	
	/** An arbitrary color that identifies this player; assigned by the game mode on spawn. Controls the color of the mesh. */
	UPROPERTY(ReplicatedUsing=OnRep_Color, Transient, BlueprintReadOnly, Category="Player")
	FLinearColor Color;

	/** Spawn specific weapon and attach to the Pawn */
	void Auth_SpawnWeapon(TSubclassOf<AHoloWeapon> WeaponClass);

	/** notification when killed, for both the server and client. */
	virtual void OnDeath(float KillingDamage, struct FDamageEvent const& DamageEvent, class APawn* InstigatingPawn, class AActor* DamageCauser);

	/** switch to ragdoll */
	void SetRagdollPhysics();

	void PlayCameraShake(TSubclassOf<UCameraShakeBase> CameraShake) const;

	UFUNCTION(client, unreliable)
	void Client_SimulateDamage();

private:

	/** Updates the MeshMID's color parameter to match our current Color property. */
	UFUNCTION()
	void OnRep_Color();

	/** For client-side Pawns, ensures that the Weapon is attached to the WeaponHandle. */
	UFUNCTION()
	void OnRep_Weapon();

	//////////////////////////////////////////////////////////////////////////
	// Input handling
	//////////////////////////////////////////////////////////////////////////

	UFUNCTION()
	void OnFire();
	
	UFUNCTION()
	void OnMoveForward(float AxisValue);
	
	UFUNCTION()
	void OnMoveRight(float AxisValue);
	
	UFUNCTION()
	void OnMoveUp(float AxisValue);
	
	UFUNCTION()
	void OnLookRight(float AxisValue);
	
	UFUNCTION()
	void OnLookUp(float AxisValue);
	
	UFUNCTION()
	void OnLookRightRate(float AxisValue);
	
	UFUNCTION()
	void OnLookUpRate(float AxisValue);

};
