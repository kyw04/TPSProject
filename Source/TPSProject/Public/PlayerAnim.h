// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "Animation/AnimInstance.h"
#include "PlayerAnim.generated.h"

/**
 * 
 */
UCLASS()
class TPSPROJECT_API UPlayerAnim : public UAnimInstance
{
	GENERATED_BODY()
	
public:
	UPROPERTY(BlueprintReadWrite, EditAnywhere, Category = PlayerAnim)
	float Speed;
	UPROPERTY(BlueprintReadWrite, EditAnywhere, Category = PlayerAnim)
	float Direction;
	
	UPROPERTY(BlueprintReadWrite, EditAnywhere, Category = PlayerAnim)
	bool IsInAir = false;
	
public:
	virtual void NativeUpdateAnimation(float DeltaSeconds) override;
	
	UFUNCTION(BlueprintImplementableEvent, BlueprintCallable, Category = PlayerAnim)
	void PlayFireAnimation();
	
};
