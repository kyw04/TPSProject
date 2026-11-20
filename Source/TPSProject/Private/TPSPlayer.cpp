// Fill out your copyright notice in the Description page of Project Settings.

#include "../Public/TPSPlayer.h"

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
	SpringArm->bUsePawnControlRotation = true;
	SpringArm->TargetArmLength = 100.0f;
	SpringArm->SetRelativeLocation(FVector(0.0f, 30.0f, 80.0f));
	
	Camera = CreateDefaultSubobject<UCameraComponent>("Camera");
	Camera->SetupAttachment(SpringArm);
	
	SniperGunComp = CreateDefaultSubobject<UStaticMeshComponent>(TEXT("SniperGunMesh"));
	SniperGunComp->SetupAttachment(GetMesh(), FName("HandGrip_R"));
	// SniperGunComp->SetupAttachment(GetMesh());
	ConstructorHelpers::FObjectFinder<UStaticMesh> TempGun(TEXT("/Game/SniperGun/sniper11.sniper11"));
	if (TempGun.Succeeded())
	{
		SniperGunComp->SetStaticMesh(TempGun.Object);
		SniperGunComp->SetRelativeLocation(FVector(7.000000,45.000000,-3.000000));
		SniperGunComp->SetRelativeRotation(FRotator(19.000000,-8.000000,11.000000));
		SniperGunComp->SetRelativeScale3D(FVector(0.150000,0.150000,0.150000));
	}

	ConstructorHelpers::FClassFinder<UUserWidget> TempUI(TEXT("/Game/UI/WBP_Crosshair.WBP_Crosshair_C")); // 경로 뒤에 _C
	if (TempUI.Succeeded())
		CrosshairUIWidget = TempUI.Class;
	ConstructorHelpers::FClassFinder<UUserWidget> TempSniperUI(TEXT("/Game/UI/WBP_Sniper.WBP_Sniper_C"));
	if (TempSniperUI.Succeeded())
		SniperUIWidget = TempSniperUI.Class;

	ConstructorHelpers::FClassFinder<UAnimInstance> TempAnim(TEXT("/Game/Animations/ABP_Player.ABP_Player_C"));
	if (TempAnim.Succeeded())
		GetMesh()->SetAnimInstanceClass(TempAnim.Class);
	
	
	AutoPossessPlayer = EAutoReceiveInput::Player0;
	JumpMaxCount = 2;
}

// Called when the game starts or when spawned
void ATPSPlayer::BeginPlay()
{
	Super::BeginPlay();
	
	SpringArmDefaultPosition = SpringArm->GetRelativeLocation();
	
  	CrosshairUI = CreateWidget(GetWorld(), CrosshairUIWidget);
	CrosshairUI->AddToViewport();
	SniperUI = CreateWidget(GetWorld(), SniperUIWidget);
}

// Called every frame
void ATPSPlayer::Tick(float DeltaTime)
{
	Super::Tick(DeltaTime);
	
	if (bZoomPressed && !bZoomIn)
	{
		ZoomHoldTime += DeltaTime;
		if (ZoomHoldTime >= ZoomHoldDelay)
			bZoomIn = true;
	}
}

// Called to bind functionality to input
void ATPSPlayer::SetupPlayerInputComponent(UInputComponent* PlayerInputComponent)
{
	Super::SetupPlayerInputComponent(PlayerInputComponent);
	
	PlayerInputComponent->BindAxis(TEXT("Turn"), this, &ATPSPlayer::Turn);
	PlayerInputComponent->BindAxis(TEXT("LookUp"), this, &ATPSPlayer::LookUp);
	PlayerInputComponent->BindAxis(TEXT("Horizontal"), this, &ATPSPlayer::MoveRight);
	PlayerInputComponent->BindAxis(TEXT("Vertical"), this, &ATPSPlayer::MoveForward);
	PlayerInputComponent->BindAxis(TEXT("Lean"), this, &ATPSPlayer::SetLean);
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

void ATPSPlayer::SetLean(float Value)
{
	float newValue = Value ? Value : LeanDirection;
	if (newValue != LeanDirection)
	{
		LeanDirection = newValue;
		SpringArmDefaultPosition.Y *= -1.0f;
		
		if (!bZoomIn)
			ZoomLerp(DefaultFOV, SpringArmDefaultPosition);
		else if (ZoomHoldTime >= ZoomHoldDelay)
			Shouldering();
	}
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

void ATPSPlayer::Shouldering()
{
	FVector springArmPosition = SpringArmDefaultPosition;
	springArmPosition.Y *= 2.0f;
	ZoomLerp(ShoulderingFOV, springArmPosition);
	SpringArm->TargetArmLength = 75.0f;
	CrosshairUI->AddToViewport();
	SniperUI->RemoveFromParent();
}

void ATPSPlayer::ZoomIn()
{
	Shouldering();
	bZoomPressed = true;
	ZoomHoldTime = 0.0f;
}

void ATPSPlayer::ZoomOut()
{
	bZoomPressed = false;
	SpringArm->TargetArmLength = 100.0f;
	if (bZoomIn)
	{
		bZoomIn = false;
		ZoomLerp(DefaultFOV, SpringArmDefaultPosition);
		CrosshairUI->AddToViewport();
		SniperUI->RemoveFromParent();
	}
	else
	{
		bZoomIn = true;
		ZoomLerp(ScopeFOV, SpringArmDefaultPosition);
		CrosshairUI->RemoveFromParent();
		SniperUI->AddToViewport();
	}
}

void ATPSPlayer::ZoomLerp(float Value, const FVector& Pos)
{
	GetWorld()->GetTimerManager().ClearTimer(ZoomTimerHandle);
	TargetFOV = Value;
	bFOVSettingEnd = false;
	TargetSpringArmPosition = Pos;
	bSpringArmPosSettingEnd = false;
	GetWorld()->GetTimerManager().SetTimer(ZoomTimerHandle, this, &ATPSPlayer::UpdateZoom, 0.01f, true);
}

void ATPSPlayer::UpdateZoom()
{
	if (!bFOVSettingEnd)
	{
		float CurrentFOV = Camera->FieldOfView;
		if (FMath::IsNearlyEqual(CurrentFOV, TargetFOV, 0.1f))
		{
			bFOVSettingEnd = true;
			Camera->SetFieldOfView(TargetFOV);
		}
		else
			Camera->SetFieldOfView(FMath::Lerp(CurrentFOV, TargetFOV, 0.5f));
	}

	if (!bSpringArmPosSettingEnd)
	{
		FVector CurrentLocation = SpringArm->GetRelativeLocation();
		if (CurrentLocation.Equals(TargetSpringArmPosition, 0.1f))
		{
			bSpringArmPosSettingEnd = true;
			SpringArm->SetRelativeLocation(TargetSpringArmPosition);
		}
		else
			SpringArm->SetRelativeLocation(FMath::Lerp(CurrentLocation, TargetSpringArmPosition, 0.5f));
	}

	if (bFOVSettingEnd && bSpringArmPosSettingEnd)
		GetWorld()->GetTimerManager().ClearTimer(ZoomTimerHandle);
}
