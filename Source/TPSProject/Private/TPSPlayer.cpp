// Fill out your copyright notice in the Description page of Project Settings.

#include "../Public/TPSPlayer.h"

#include "FrameTypes.h"
#include "Blueprint/UserWidget.h"
#include "Kismet/GameplayStatics.h"
#include "Runtime/Engine/Classes/Camera/CameraComponent.h"
#include "Runtime/Engine/Classes/GameFramework/SpringArmComponent.h"

// Sets default values
ATPSPlayer::ATPSPlayer()
{
	// Set this character to call Tick() every frame.  You can turn this off to improve performance if you don't need it.
	PrimaryActorTick.bCanEverTick = true;
	
	ConstructorHelpers::FObjectFinder<USkeletalMesh> TempMesh(TEXT("'/Game/Characters/Mannequins/Meshes/SKM_Manny_Simple.SKM_Manny_Simple'"));
	
	if (TempMesh.Succeeded())
	{
		GetMesh()->SetSkeletalMesh(TempMesh.Object);
		GetMesh()->SetRelativeLocation(FVector(0.0f, 0.0f, -90.0f));
		GetMesh()->SetRelativeRotation(FRotator(0.0f, -90.0f, 0.0f));
	}
	
	SpringArm = CreateDefaultSubobject<USpringArmComponent>(TEXT("SpringArm"));
	SpringArm->SetupAttachment(RootComponent);
	SpringArm->SetRelativeLocation(FVector(0.0f, 0.0f, 60.0f));
	
	Camera = CreateDefaultSubobject<UCameraComponent>("Camera");
	Camera->SetupAttachment(SpringArm);
	
	SniperGunComp = CreateDefaultSubobject<UStaticMeshComponent>(TEXT("SniperGun"));
	SniperGunComp->SetupAttachment(GetMesh());
	ConstructorHelpers::FObjectFinder<UStaticMesh> TempGun(TEXT("/Game/SniperGun/sniper11.sniper11"));
	if (TempGun.Succeeded())
	{
		SniperGunComp->SetStaticMesh(TempGun.Object);
		SniperGunComp->SetRelativeLocation(FVector(-50, 50, 100));
		SniperGunComp->SetRelativeRotation(FRotator(0, 0, 0));
		SniperGunComp->SetRelativeScale3D(FVector(0.15f, 0.15f, 0.15f));
	}

	ConstructorHelpers::FClassFinder<UUserWidget> TempUI(TEXT("/Game/UI/WBP_Crosshair.WBP_Crosshair_C")); // 경로 뒤에 _C
	if (TempUI.Succeeded())
	{
		CrosshairUIWidget = TempUI.Class;
	}
	
	AutoPossessPlayer = EAutoReceiveInput::Player0;
	
	JumpMaxCount = 2;
}

// Called when the game starts or when spawned
void ATPSPlayer::BeginPlay()
{
	Super::BeginPlay();
	
  	CrosshairUI = CreateWidget(GetWorld(), CrosshairUIWidget);
	CrosshairUI->AddToViewport();
}

// Called every frame
void ATPSPlayer::Tick(float DeltaTime)
{
	Super::Tick(DeltaTime);
}

// Called to bind functionality to input
void ATPSPlayer::SetupPlayerInputComponent(UInputComponent* PlayerInputComponent)
{
	Super::SetupPlayerInputComponent(PlayerInputComponent);
	
	PlayerInputComponent->BindAxis(TEXT("Turn"), this, &ATPSPlayer::Turn);
	PlayerInputComponent->BindAxis(TEXT("LookUp"), this, &ATPSPlayer::LookUp);
	PlayerInputComponent->BindAxis(TEXT("Horizontal"), this, &ATPSPlayer::MoveRight);
	PlayerInputComponent->BindAxis(TEXT("Vertical"), this, &ATPSPlayer::MoveForward);
	PlayerInputComponent->BindAction(TEXT("Jump"), IE_Pressed, this, &ATPSPlayer::PlayerJump);
	PlayerInputComponent->BindAction(TEXT("Fire"), IE_Pressed, this, &ATPSPlayer::Fire);
	PlayerInputComponent->BindAction(TEXT("Zoom"), IE_Pressed, this, &ATPSPlayer::ZoomIn);
	PlayerInputComponent->BindAction(TEXT("Zoom"), IE_Released, this, &ATPSPlayer::ZoomOut);
}

void ATPSPlayer::Turn(float Value)
{
	AddControllerYawInput(Value);
}

void ATPSPlayer::LookUp(float Value)
{
	AddControllerPitchInput(Value);
}

void ATPSPlayer::MoveForward(float Value)
{
	FVector direction = FRotationMatrix(GetControlRotation()).GetScaledAxis(EAxis::X) * Value;
	AddMovementInput(direction);
}

void ATPSPlayer::MoveRight(float Value)
{
	FVector direction = FRotationMatrix(GetControlRotation()).GetScaledAxis(EAxis::Y) * Value;
	AddMovementInput(direction);
}

void ATPSPlayer::PlayerJump()
{
	Jump();
}

void ATPSPlayer::Fire()
{
	FHitResult HitInfo;
	FVector StartLocation = Camera->GetComponentLocation();
	FVector EndLocation = StartLocation + Camera->GetForwardVector() * 50000.0f;
	FCollisionQueryParams Params;
	Params.AddIgnoredActor(this);
	
	bool bHit = GetWorld()->LineTraceSingleByChannel(HitInfo, StartLocation, EndLocation, ECC_Visibility, Params);
	if (bHit)
	{
		if (BulletEffect)
			UGameplayStatics::SpawnEmitterAtLocation(GetWorld(), BulletEffect, HitInfo.Location);
		
		if (HitInfo.GetComponent()->IsSimulatingPhysics())
		{
			HitInfo.GetComponent()->AddImpulseAtLocation(Camera->GetForwardVector() * 100000.0f, HitInfo.Location);
		}
	}
}

void ATPSPlayer::ZoomIn()
{
	ZoomLerp(45);
}

void ATPSPlayer::ZoomOut()
{
	ZoomLerp(90);
}

void ATPSPlayer::ZoomLerp(float Value)
{
	GetWorld()->GetTimerManager().ClearTimer(ZoomTimerHandle);
	TargetFOV = Value;
	GetWorld()->GetTimerManager().SetTimer(ZoomTimerHandle, this, &ATPSPlayer::UpdateZoom, 0.01f, true);
}

void ATPSPlayer::UpdateZoom()
{
	float CurrentFOV = Camera->FieldOfView;
	if (FMath::IsNearlyEqual(CurrentFOV, TargetFOV, 0.1f))
	{
		Camera->SetFieldOfView(TargetFOV);
		GetWorld()->GetTimerManager().ClearTimer(ZoomTimerHandle);
		return;
	}

	float NewFOV = FMath::Lerp(CurrentFOV, TargetFOV, 0.5);
	Camera->SetFieldOfView(NewFOV);
}
