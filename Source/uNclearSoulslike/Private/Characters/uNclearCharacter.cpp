// Fill out your copyright notice in the Description page of Project Settings.


#include "Characters/uNclearCharacter.h"

// Sets default values
AuNclearCharacter::AuNclearCharacter()
{
 	// Set this character to call Tick() every frame.  You can turn this off to improve performance if you don't need it.
	PrimaryActorTick.bCanEverTick = true;

}

// Called when the game starts or when spawned
void AuNclearCharacter::BeginPlay()
{
	Super::BeginPlay();
	
}

// Called every frame
void AuNclearCharacter::Tick(float DeltaTime)
{
	Super::Tick(DeltaTime);

}

// Called to bind functionality to input
void AuNclearCharacter::SetupPlayerInputComponent(UInputComponent* PlayerInputComponent)
{
	Super::SetupPlayerInputComponent(PlayerInputComponent);

}

