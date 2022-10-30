// Fill out your copyright notice in the Description page of Project Settings.


#include "UI/HoloHUD.h"
#include "Engine/Canvas.h"
#include "Player/HoloPawn.h"

void AHoloHUD::DrawHUD()
{
	Super::DrawHUD();

	AHoloPawn* Pawn = Cast<AHoloPawn>(GetOwningPawn());
	if (!Pawn || Pawn->bIsDying || !Canvas)
	{
		return;
	}
	
	// Draw a static crosshair in the center of the screen
	const float CrosshairSize = FMath::Lerp(16.0f, 22.0f, CrosshairExpandWeight);
	const float CrosshairGapSize = FMath::Lerp(6.0f, 20.0f, CrosshairExpandWeight);
	DrawCrosshair(Pawn->GetColor() * 1.33f, CrosshairSize, CrosshairGapSize);
}

void AHoloHUD::DrawCrosshair(const FLinearColor& Color, float TotalSize, float GapSize)
{
	check(Canvas);

	const float CenterX = Canvas->SizeX * 0.5f;
	const float CenterY = Canvas->SizeY * 0.5f;

	const float ArmOffset = TotalSize * 0.5f;
	const float GapOffset = GapSize * 0.5f;
	const float Thickness = 2.0f;

	DrawLine(CenterX - ArmOffset, CenterY, CenterX - GapOffset, CenterY, Color, Thickness);
	DrawLine(CenterX + GapOffset, CenterY, CenterX + ArmOffset, CenterY, Color, Thickness);
	DrawLine(CenterX, CenterY - ArmOffset, CenterX, CenterY - GapOffset, Color, Thickness);
	DrawLine(CenterX, CenterY + GapOffset, CenterX, CenterY + ArmOffset, Color, Thickness);
}
