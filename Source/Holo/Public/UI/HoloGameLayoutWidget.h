// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "Blueprint/UserWidget.h"
#include "HoloGameLayoutWidget.generated.h"

UCLASS(BlueprintType, Blueprintable)
class HOLO_API UHoloGameLayoutWidget : public UUserWidget
{
	GENERATED_BODY()

protected:

	UPROPERTY(BlueprintReadWrite, meta = (BindWidgetOptional))
	class UHoloPlayerHealthWidget* PlayerHealthWidget;
};
