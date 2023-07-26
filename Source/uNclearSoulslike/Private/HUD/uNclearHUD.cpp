// Fill out your copyright notice in the Description page of Project Settings.


#include "HUD/uNclearHUD.h"
#include "HUD/uNclearOverlay.h"

void AuNclearHUD::BeginPlay()
{
	Super::BeginPlay();

	UWorld* World = GetWorld();
	if (World)
	{
		APlayerController* Controller = World->GetFirstPlayerController();
		if (Controller && uNclearOverlayClass)
		{
			uNclearOverlay = CreateWidget<UuNclearOverlay>(Controller, uNclearOverlayClass);
			uNclearOverlay->AddToViewport();
		}
	}
}
