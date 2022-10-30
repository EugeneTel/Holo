// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "Blueprint/UserWidget.h"
#include "HoloPlayerHealthWidget.generated.h"

class UProgressBar;

UCLASS(BlueprintType, Blueprintable)
class HOLO_API UHoloPlayerHealthWidget : public UUserWidget
{
	GENERATED_BODY()

public:

	//~ Begin UUserWidget Interface
	virtual void NativeConstruct() override;
	//~ End UUserWidget Interface

protected:

	UPROPERTY(BlueprintReadWrite, meta = (BindWidgetOptional))
	UProgressBar* HealthProgressBar;

private:

	UFUNCTION()
	void ColorChanged(const FLinearColor& NewColor);
	
	UFUNCTION()
	void HealthChanged(float CurrentHealth, float MaxHealth);

};
