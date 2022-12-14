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

	//~ Begin AActor interface
	virtual void BeginPlay() override;
	//~ End AActor interface

	//~ Begin AGameModeBase interface
	virtual void SetPlayerDefaults(APawn* PlayerPawn) override;
	virtual AActor* FindPlayerStart_Implementation(AController* Player, const FString& IncomingName) override;
	//~ End AGameModeBase interface

	/** If we're initializing a newly-spawned player pawn, assign it a color */
	void SetPlayerColor(class AHoloPawn* HoloPawn);

protected:

	/** A sequence of arbitrary color values that will be assigned to newly-spawned player pawns. */
	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category="Players")
	TArray<FLinearColor> PlayerColors;

private:
	
	/** Index into PlayerColors indicating the last color value we assigned to a pawn. */
	int32 LastPlayerColorIndex;

	/** Cached start actors */
	UPROPERTY(Transient)
	TArray<AActor*> StartActors;
};
