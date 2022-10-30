// Fill out your copyright notice in the Description page of Project Settings.


#include "Player/HoloPlayerController.h"

#include "GameFramework/GameModeBase.h"

void AHoloPlayerController::Respawn()
{
	GetWorld()->GetAuthGameMode()->RestartPlayer(this);
}
