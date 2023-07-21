// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "GameFramework/HUD.h"
#include "uNclearHUD.generated.h"

class UuNclearOverlay;

/**
 * 
 */
UCLASS()
class UNCLEARSOULSLIKE_API AuNclearHUD : public AHUD
{
	GENERATED_BODY()

protected:
	
	virtual void BeginPlay() override;
	
private:
	
	UPROPERTY(EditDefaultsOnly, Category = uNclear)
	TSubclassOf<UuNclearOverlay> uNclearOverlayClass;

	UPROPERTY()
	UuNclearOverlay* uNclearOverlay;

public:
	
	FORCEINLINE UuNclearOverlay* GetuNclearOverlay() const { return uNclearOverlay; }
};
