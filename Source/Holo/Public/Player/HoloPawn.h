// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "GameFramework/Character.h"
#include "HoloPawn.generated.h"

UCLASS()
class HOLO_API AHoloPawn : public ACharacter
{
	GENERATED_BODY()

public:
	// Sets default values for this character's properties
	AHoloPawn();

	//~ Begin AActor Interface
	virtual void Tick(float DeltaTime) override;
	virtual void BeginPlay() override;
	//~ End AActor Interface

	//~ Begin APawn Interface
	virtual void PostInitializeComponents() override;
	//~ End APawn Interface

	//~ Begin ACharacter Interface
	virtual void SetupPlayerInputComponent(class UInputComponent* PlayerInputComponent) override;
	//~ End ACharacter Interface

	void Auth_SetColor(const FLinearColor& InColor);

protected:

	/** Material instance assigned to the character mesh, giving us control over the shader parameters at runtime. */
	UPROPERTY(VisibleAnywhere, BlueprintReadOnly, Category="Player")
	UMaterialInstanceDynamic* MeshMID;
	
	/** An arbitrary color that identifies this player; assigned by the game mode on spawn. Controls the color of the mesh. */
	UPROPERTY(ReplicatedUsing=OnRep_Color, Transient, BlueprintReadOnly, Category="Player")
	FLinearColor Color;

private:

	/** Updates the MeshMID's color parameter to match our current Color property. */
	UFUNCTION()
	void OnRep_Color() const;

	//////////////////////////////////////////////////////////////////////////
	// Input handling
	//////////////////////////////////////////////////////////////////////////

	UFUNCTION()
	void OnFire();
	
	UFUNCTION()
	void OnMoveForward(float AxisValue);
	
	UFUNCTION()
	void OnMoveRight(float AxisValue);
	
	UFUNCTION()
	void OnMoveUp(float AxisValue);
	
	UFUNCTION()
	void OnLookRight(float AxisValue);
	
	UFUNCTION()
	void OnLookUp(float AxisValue);
	
	UFUNCTION()
	void OnLookRightRate(float AxisValue);
	
	UFUNCTION()
	void OnLookUpRate(float AxisValue);

};
