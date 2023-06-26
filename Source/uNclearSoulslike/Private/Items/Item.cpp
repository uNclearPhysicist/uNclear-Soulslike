// Fill out your copyright notice in the Description page of Project Settings.


#include "Items/Item.h"
#include "uNclearSoulslike/DebugMacros.h"


// Sets default values
AItem::AItem()
{
 	// Set this actor to call Tick() every frame.  You can turn this off to improve performance if you don't need it.
	PrimaryActorTick.bCanEverTick = true;

}

// Called when the game starts or when spawned
void AItem::BeginPlay()
{
	Super::BeginPlay();
}

// Called every frame
void AItem::Tick(float DeltaTime)
{
	Super::Tick(DeltaTime);

	// Movement Rate (cm/s)
	float MovementRate = 50.f;

	// Rotation Rate (deg/s)
	float RotationRate = 45.f;

	// Movement Rate * Delta Time (cm/s * s/frame = cm/frame)
	AddActorWorldOffset(FVector(MovementRate*DeltaTime,0.f,0.f));
	
	// Rotation Rate * Delta Time (deg/s * s/frame = deg/frame)
	AddActorWorldRotation(FRotator(0.f,RotationRate*DeltaTime,0.f));
	
	DRAW_SPHERE_SingleFrame(GetActorLocation());
	DRAW_VECTOR_SingleFrame(GetActorLocation(), GetActorLocation() + GetActorForwardVector()*100.f);
}

