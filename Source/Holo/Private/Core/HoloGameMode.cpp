// Fill out your copyright notice in the Description page of Project Settings.


#include "Core/HoloGameMode.h"
#include "Player/HoloPawn.h"

AHoloGameMode::AHoloGameMode()
{
	// Establish a sequence of arbitrary colors that we'll apply to each player
	// pawn, round-robin in the order in which they're spawned
	PlayerColors.Add(FLinearColor::Red);
	PlayerColors.Add(FLinearColor::Green);
	PlayerColors.Add(FLinearColor::Blue);
	PlayerColors.Add(FLinearColor::Yellow);
	
	LastPlayerColorIndex = -1;
}

void AHoloGameMode::SetPlayerDefaults(APawn* PlayerPawn)
{
	Super::SetPlayerDefaults(PlayerPawn);

	AHoloPawn* HoloPawn = Cast<AHoloPawn>(PlayerPawn);
	check(HoloPawn);

	SetPlayerColor(HoloPawn);
}

void AHoloGameMode::SetPlayerColor(AHoloPawn* HoloPawn)
{
	// Use the next color in our sequence
	const int32 PlayerColorIndex = (LastPlayerColorIndex + 1) % PlayerColors.Num();
	if (PlayerColors.IsValidIndex(PlayerColorIndex))
	{
		HoloPawn->Auth_SetColor(PlayerColors[PlayerColorIndex]);
		LastPlayerColorIndex = PlayerColorIndex;
	}
}
