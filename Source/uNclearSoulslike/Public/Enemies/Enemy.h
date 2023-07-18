// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "GameFramework/Character.h"
#include "Interfaces/HitInterface.h"
#include "Characters/CharacterTypes.h"
#include "Enemy.generated.h"

class UAnimMontage;
class USoundBase;
class UAttributeComponent;
class UHealthBarComponent;
class UPawnSensingComponent;

UCLASS()
class UNCLEARSOULSLIKE_API AEnemy : public ACharacter, public IHitInterface
{
	GENERATED_BODY()

public:
	// Sets default values for this character's properties
	AEnemy();

	// Called every frame
	virtual void Tick(float DeltaTime) override;

	void CheckPatrolTarget();
	
	void CheckCombatTarget();

	// Called to bind functionality to input
	virtual void SetupPlayerInputComponent(class UInputComponent* PlayerInputComponent) override;
	
	virtual void GetHit_Implementation(const FVector& ImpactPoint) override;

	void DirectionalHitReact(const FVector& ImpactPoint);
	
	virtual float TakeDamage(float DamageAmount, struct FDamageEvent const& DamageEvent, class AController* EventInstigator, AActor* DamageCauser) override;

protected:
	// Called when the game starts or when spawned
	virtual void BeginPlay() override;

	void Die();
	
	bool InTargetRange(AActor* Target, double Radius);

	UPROPERTY(BlueprintReadOnly)
	EDeathPose DeathPose = EDeathPose::EDP_Alive;

	void MoveToTarget(AActor* Target);
	
	AActor* ChoosePatrolTarget();

	UFUNCTION()
	void PawnSeen(APawn* SeenPawn);

	/*
	* Play montage functions
	*/
	void PlayHitReactMontage(const FName& SectionName);

private:

	/*
	* Components
	*/
	
	UPROPERTY(VisibleAnywhere, BlueprintReadOnly, meta = (AllowPrivateAccess = "true"))
	UAttributeComponent* Attributes;

	UPROPERTY(VisibleAnywhere)
	UHealthBarComponent* HealthBarWidget;

	UPROPERTY(VisibleAnywhere)
	UPawnSensingComponent* PawnSensing;

	/*
	* Navigation
	*/
	
	UPROPERTY()
	AActor* CombatTarget;

	UPROPERTY(EditAnywhere)
	double CombatRadius = 500.f;

	UPROPERTY()
	class AAIController* EnemyController;

	// Current patrol target
	UPROPERTY(EditInstanceOnly, Category = "AI Navigation")
	AActor* PatrolTarget;

	UPROPERTY(EditInstanceOnly, Category = "AI Navigation")
	TArray<AActor*> PatrolTargets;

	UPROPERTY(EditAnywhere)
	double PatrolRadius = 200.f;

	FTimerHandle PatrolTimer;
	void PatrolTimerFinished();

	UPROPERTY(EditAnywhere, Category = "AI Navigation")
	float WaitMin = 5.f;

	UPROPERTY(EditAnywhere, Category = "AI Navigation")
	float WaitMax = 10.f;

	EEnemyState EnemyState = EEnemyState::EES_Patrolling;

	/**
	* Hit effects (animation montages, sound, particles)
	*/
	UPROPERTY(EditDefaultsOnly, Category = Montages)
	UAnimMontage* HitReactMontage;
	
	UPROPERTY(EditDefaultsOnly, Category = Montages)
	UAnimMontage* DeathMontage;

	UPROPERTY(EditAnywhere, Category = Sounds)
	USoundBase* HitSound;

	UPROPERTY(EditAnywhere, Category = VisualEffects)
	UParticleSystem* HitParticles;
};
