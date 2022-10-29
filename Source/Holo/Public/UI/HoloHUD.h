// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "GameFramework/HUD.h"
#include "HoloHUD.generated.h"

/**
 * 
 */
UCLASS()
class HOLO_API AHoloHUD : public AHUD
{
	GENERATED_BODY()

public:
	//~ Begin AHUD Interface
	virtual void DrawHUD() override;
	//~ End AHUD Interface

private:

	/** Value between 0 and 1 indicating whether the crosshair is expanded */
	float CrosshairExpandWeight = 0.0f;

	void DrawCrosshair(const FLinearColor& Color, float TotalSize, float GapSize);
};
