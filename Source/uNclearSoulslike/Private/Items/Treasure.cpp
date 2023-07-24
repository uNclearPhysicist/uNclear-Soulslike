// Fill out your copyright notice in the Description page of Project Settings.


#include "Items/Treasure.h"
#include "Characters/uNclearCharacter.h"

void ATreasure::OnSphereOverlap(UPrimitiveComponent* OverlappedComponent, AActor* OtherActor, UPrimitiveComponent* OtherComp, int32 OtherBodyIndex, bool bFromSweep, const FHitResult& SweepResult)
{
	AuNclearCharacter* uNclearCharacter = Cast<AuNclearCharacter>(OtherActor);
	if (uNclearCharacter)
	{
		SpawnPickupSound();
		Destroy();
	}
}
