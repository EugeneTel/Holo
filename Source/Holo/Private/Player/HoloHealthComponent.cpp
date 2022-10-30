// Fill out your copyright notice in the Description page of Project Settings.


#include "Player/HoloHealthComponent.h"

#include "Net/UnrealNetwork.h"
#include "Player/HoloPawn.h"


// Sets default values for this component's properties
UHoloHealthComponent::UHoloHealthComponent()
{
	MaxHealth = 100.0f;
	CurrentHealth = 100.0f;
}


// Called when the game starts
void UHoloHealthComponent::BeginPlay()
{
	Super::BeginPlay();

	// ...
	
}

float UHoloHealthComponent::ApplyDamage(float Damage, FDamageEvent const& DamageEvent, AController* EventInstigator, AActor* DamageCauser)
{
	CurrentHealth = FMath::Max<float>(CurrentHealth - Damage, 0.0f);
	OnRep_CurrentHealth();
	
	if (CurrentHealth <= 0.0f)
	{
		// Death
		AHoloPawn* Pawn = Cast<AHoloPawn>(GetOwner());
		if (Pawn)
		{
			Pawn->Die(Damage, DamageEvent, EventInstigator, DamageCauser);
		}
	}

	return Damage;
}

void UHoloHealthComponent::OnRep_CurrentHealth()
{
	if (OnHealthChangedDelegate.IsBound())
	{
		OnHealthChangedDelegate.Broadcast(CurrentHealth, MaxHealth);
	}
}

void UHoloHealthComponent::GetLifetimeReplicatedProps(TArray<FLifetimeProperty>& OutLifetimeProps) const
{
	Super::GetLifetimeReplicatedProps(OutLifetimeProps);

	DOREPLIFETIME(UHoloHealthComponent, CurrentHealth);
}


