// Fill out your copyright notice in the Description page of Project Settings.


#include "Characters/uNclearAnimInstance.h"
#include "Characters/uNclearCharacter.h"
#include "GameFramework/CharacterMovementComponent.h"
#include "Kismet/KismetMathLibrary.h"

void UuNclearAnimInstance::NativeInitializeAnimation()
{
	Super::NativeInitializeAnimation();

	uNclearCharacter = Cast<AuNclearCharacter>(TryGetPawnOwner());
	if (uNclearCharacter)
	{
		uNclearCharacterMovement = uNclearCharacter->GetCharacterMovement();
	}
}

void UuNclearAnimInstance::NativeUpdateAnimation(float DeltaTime)
{
	Super::NativeUpdateAnimation(DeltaTime);

	if (uNclearCharacterMovement)
	{
		GroundSpeed = UKismetMathLibrary::VSizeXY(uNclearCharacterMovement->Velocity);
	}
}