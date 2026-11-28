// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "Runtime/Engine/Classes/GameFramework/Character.h"
#include "TPSPlayer.generated.h"

UCLASS()
class TPSPROJECT_API ATPSPlayer : public ACharacter
{
	GENERATED_BODY()

public:
	// Sets default values for this character's properties
	ATPSPlayer();
	
	
protected:
	// Called when the game starts or when spawned
	virtual void BeginPlay() override;
	
	UPROPERTY(VisibleAnywhere)
	class USpringArmComponent* SpringArm;
	UPROPERTY(VisibleAnywhere, BlueprintReadOnly)
	class UCameraComponent* Camera;
	UPROPERTY(VisibleAnywhere)
	class UStaticMeshComponent* SniperGunComp;
	
	UPROPERTY(EditDefaultsOnly, Category = Effect)
	class UParticleSystem* BulletEffect;
	UPROPERTY(EditDefaultsOnly, Category = UI)
	TSubclassOf<class UUserWidget> CrosshairUIWidget;
	UPROPERTY()
	class UUserWidget* CrosshairUI;
	UPROPERTY(EditDefaultsOnly, Category = UI)
	TSubclassOf<class UUserWidget> SniperUIWidget;
	UPROPERTY()
	class UUserWidget* SniperUI;
	
	float LeanDirection;
	
	FTimerHandle ZoomTimerHandle;
	
	FVector SpringArmDefaultPosition;
	FVector TargetSpringArmPosition;
	bool bSpringArmPosSettingEnd;
	
	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = Zoom)
	float DefaultFOV = 90.0f;
	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = Zoom)
	float ShoulderingFOV = 75.0f;
	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = Zoom)
	float ScopeFOV = 45.0f;
	
	bool bFOVSettingEnd;
	float TargetFOV;
	float ZoomHoldDelay = 0.1f;
	float ZoomHoldTime;
	
	bool bZoomPressed;
	bool bZoomIn;

public:
	virtual void Tick(float DeltaTime) override;
	virtual void SetupPlayerInputComponent(class UInputComponent* PlayerInputComponent) override;
	
	void Fire();
	void SetLean(float Value);
	void Shouldering();
	void ZoomIn();
	void ZoomOut();
	void ZoomLerp(float Value, const FVector& Pos);
	void UpdateZoom();
	
	UPROPERTY(VisibleAnywhere)
	class UPlayerBaseComp* PlayerMoveComp;
};
