// Fill out your copyright notice in the Description page of Project Settings.


#include "UI/HoloPlayerHealthWidget.h"

#include "Components/ProgressBar.h"
#include "Kismet/KismetMathLibrary.h"
#include "Player/HoloHealthComponent.h"
#include "Player/HoloPawn.h"

void UHoloPlayerHealthWidget::NativeConstruct()
{
	Super::NativeConstruct();

	AHoloPawn* Pawn = CastChecked<AHoloPawn>(GetOwningPlayerPawn());
	ColorChanged(Pawn->GetColor());

	UHoloHealthComponent* HealthComponent = Pawn->GetHealthComponent();
	check(HealthComponent);
	HealthChanged(HealthComponent->CurrentHealth, HealthComponent->MaxHealth);
	
	// Subscribe on delegates
	Pawn->OnColorChangedDelegate.AddDynamic(this, &UHoloPlayerHealthWidget::ColorChanged);
	HealthComponent->OnHealthChangedDelegate.AddDynamic(this, &UHoloPlayerHealthWidget::HealthChanged);
}

void UHoloPlayerHealthWidget::ColorChanged(const FLinearColor& NewColor)
{
	if (HealthProgressBar)
	{
		HealthProgressBar->SetFillColorAndOpacity(NewColor);
	}
}

void UHoloPlayerHealthWidget::HealthChanged(float CurrentHealth, float MaxHealth)
{
	if (HealthProgressBar)
	{
		HealthProgressBar->SetPercent(UKismetMathLibrary::NormalizeToRange(CurrentHealth, 0.0f, MaxHealth));
	}
}
