// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "PlayerBaseComp.h"
#include "Components/ActorComponent.h"
#include "PlayerMoveComp.generated.h"


UCLASS(ClassGroup=(Custom), meta=(BlueprintSpawnableComponent))
class TPSPROJECT_API UPlayerMoveComp : public UPlayerBaseComp
{
	GENERATED_BODY()

public:
	// Sets default values for this component's properties
	UPlayerMoveComp();

protected:
	// Called when the game starts
	virtual void BeginPlay() override;

public:
	// Called every frame
	virtual void TickComponent(float DeltaTime, ELevelTick TickType,
	                           FActorComponentTickFunction* ThisTickFunction) override;
	virtual void SetupPlayerInputBinding(class UInputComponent* PlayerInputComponent) override;
	
	void Turn(float Value);
	void LookUp(float Value);
	
	void MoveForward(float Value);
	void MoveRight(float Value);
	
	void PlayerJump();
};
