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
	
	FTimerHandle ZoomTimerHandle;
	float TargetFOV;

public:
	virtual void Tick(float DeltaTime) override;
	virtual void SetupPlayerInputComponent(class UInputComponent* PlayerInputComponent) override;
	
	void Turn(float Value);
	void LookUp(float Value);
	
	void MoveForward(float Value);
	void MoveRight(float Value);
	
	void PlayerJump();
	void Fire();
	void ZoomIn();
	void ZoomOut();
	void ZoomLerp(float Value);
	void UpdateZoom();
};
