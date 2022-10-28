// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "GameFramework/GameMode.h"
#include "HoloGameMode.generated.h"

/**
 * 
 */
UCLASS()
class HOLO_API AHoloGameMode : public AGameMode
{
	GENERATED_BODY()

public:
	AHoloGameMode();

	//~ Begin AGameModeBase interface
	virtual void SetPlayerDefaults(APawn* PlayerPawn) override;
	//~ End AGameModeBase interface

	/** If we're initializing a newly-spawned player pawn, assign it a color */
	void SetPlayerColor(class AHoloPawn* HoloPawn);

protected:

	/** A sequence of arbitrary color values that will be assigned to newly-spawned player pawns. */
	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category="Players")
	TArray<FLinearColor> PlayerColors;

	/** Index into PlayerColors indicating the last color value we assigned to a pawn. */
	int32 LastPlayerColorIndex;
};
