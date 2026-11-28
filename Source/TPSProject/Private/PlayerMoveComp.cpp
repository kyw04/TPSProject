// Fill out your copyright notice in the Description page of Project Settings.


#include "PlayerMoveComp.h"


// Sets default values for this component's properties
UPlayerMoveComp::UPlayerMoveComp()
{
	// Set this component to be initialized when the game starts, and to be ticked every frame.  You can turn these features
	// off to improve performance if you don't need them.
	PrimaryComponentTick.bCanEverTick = true;

	// ...
}


// Called when the game starts
void UPlayerMoveComp::BeginPlay()
{
	Super::BeginPlay();

	// ...
	
}


// Called every frame
void UPlayerMoveComp::TickComponent(float DeltaTime, ELevelTick TickType, FActorComponentTickFunction* ThisTickFunction)
{
	Super::TickComponent(DeltaTime, TickType, ThisTickFunction);

	// ...
}

void UPlayerMoveComp::SetupPlayerInputBinding(class UInputComponent* PlayerInputComponent)
{
	Super::SetupPlayerInputBinding(PlayerInputComponent);
	
	PlayerInputComponent->BindAxis(TEXT("Turn"), this, &UPlayerMoveComp::Turn);
	PlayerInputComponent->BindAxis(TEXT("LookUp"), this, &UPlayerMoveComp::LookUp);
	PlayerInputComponent->BindAxis(TEXT("Horizontal"), this, &UPlayerMoveComp::MoveRight);
	PlayerInputComponent->BindAxis(TEXT("Vertical"), this, &UPlayerMoveComp::MoveForward);
	PlayerInputComponent->BindAction(TEXT("Jump"), IE_Pressed, this, &UPlayerMoveComp::PlayerJump);
}

void UPlayerMoveComp::Turn(float Value)
{
	Me->AddControllerYawInput(Value);
}

void UPlayerMoveComp::LookUp(float Value)
{
	Me->AddControllerPitchInput(Value);
}

void UPlayerMoveComp::MoveForward(float Value)
{
	FVector direction = FRotationMatrix(Me->GetControlRotation()).GetScaledAxis(EAxis::X) * Value;
	Me->AddMovementInput(direction);
}

void UPlayerMoveComp::MoveRight(float Value)
{
	FVector direction = FRotationMatrix(Me->GetControlRotation()).GetScaledAxis(EAxis::Y) * Value;
	Me->AddMovementInput(direction);
}

void UPlayerMoveComp::PlayerJump()
{
	Me->Jump();
}

