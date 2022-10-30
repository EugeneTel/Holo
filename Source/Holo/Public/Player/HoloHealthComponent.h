// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "Components/ActorComponent.h"
#include "HoloHealthComponent.generated.h"

UCLASS(ClassGroup=(Custom), meta=(BlueprintSpawnableComponent))
class HOLO_API UHoloHealthComponent : public UActorComponent
{
	GENERATED_BODY()

	/** On Health Changed Delegate */
	DECLARE_DYNAMIC_MULTICAST_DELEGATE_TwoParams(FOnHealthChanged, float, CurrentHealth, float, MaxHealth);

public:
	// Sets default values for this component's properties
	UHoloHealthComponent();
	
	virtual void BeginPlay() override;

	/** Delegate fire when Health Was changed */
	UPROPERTY(BlueprintAssignable)
	FOnHealthChanged OnHealthChangedDelegate;

	/** Max Health of an Element */
	UPROPERTY(Category=HealthComponent, EditAnywhere, BlueprintReadWrite)
	float MaxHealth;

	/** Current Health of an Element */
	UPROPERTY(ReplicatedUsing=OnRep_CurrentHealth, Category=HealthComponent, EditAnywhere, BlueprintReadWrite)
	float CurrentHealth;

	/** Take damage to a Health Component. Return taken damage */
	UFUNCTION(BlueprintCallable)
	float ApplyDamage(float Damage, FDamageEvent const& DamageEvent, AController* EventInstigator, AActor* DamageCauser);

private:
	UFUNCTION()
	void OnRep_CurrentHealth();
};
