// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "GameFramework/Character.h"
#include "CharacterTypes.h"
#include "uNclearCharacter.generated.h"

class USpringArmComponent;
class UCameraComponent;
class UGroomComponent;
class AItem;
class UAnimMontage;
class AWeapon;
class UInputMappingContext;
class UInputAction;
struct FInputActionValue;

UCLASS()
class UNCLEARSOULSLIKE_API AuNclearCharacter : public ACharacter
{
	GENERATED_BODY()

public:
	// Sets default values for this character's properties
	AuNclearCharacter();
	
	// Called every frame
	virtual void Tick(float DeltaTime) override;

	// Called to bind functionality to input
	virtual void SetupPlayerInputComponent(class UInputComponent* PlayerInputComponent) override;

	UFUNCTION(BlueprintCallable)
	void SetWeaponCollisionEnabled(ECollisionEnabled::Type CollisionEnabled);
	
protected:
	// Called when the game starts or when spawned
	virtual void BeginPlay() override;

	/** 
	* Enhanced Input Mapping Context & Actions
	*/

	UPROPERTY(EditAnywhere, BlueprintReadOnly, Category = Input)
	UInputMappingContext* uNclearCharacterContext;

	UPROPERTY(EditAnywhere, BlueprintReadOnly, Category = Input)
	UInputAction* MovementAction;

	UPROPERTY(EditAnywhere, BlueprintReadOnly, Category = Input)
	UInputAction* LookingAction;

	UPROPERTY(EditAnywhere, BlueprintReadOnly, Category = Input)
	UInputAction* JumpAction;

	UPROPERTY(EditAnywhere, BlueprintReadOnly, Category = Input)
	UInputAction* EKeyAction;

	UPROPERTY(EditAnywhere, BlueprintReadOnly, Category = Input)
	UInputAction* AttackAction;

	/** 
	* Callbacks for input
	**/
	
	void Movement(const FInputActionValue& Value);

	void Looking(const FInputActionValue& Value);

	virtual void Jump() override;

	void EKeyPressed();

	void Attack();

	/** 
	* Play montage functions
	**/
	
	void PlayAttackMontage();

	UFUNCTION(BlueprintCallable)
	void AttackEnd();
	
	bool CanAttack();

	void PlayEquipMontage(FName SectionName);
	
	bool CanDisarm();
	
	bool CanArm();

	UFUNCTION(BlueprintCallable)
	void Disarm();

	UFUNCTION(BlueprintCallable)
	void Arm();

	UFUNCTION(BlueprintCallable)
	void FinishEquipping();
	
private:
	UPROPERTY(VisibleAnywhere)
	USpringArmComponent* SpringArm;

	UPROPERTY(VisibleAnywhere)
	UCameraComponent* ViewCamera;
	
	UPROPERTY(VisibleAnywhere, Category = Hair)
    UGroomComponent* Hair;
    
    UPROPERTY(VisibleAnywhere, Category = Hair)
    UGroomComponent* Eyebrows;

	UPROPERTY(VisibleInstanceOnly)
	AItem* OverlappingItem;

	ECharacterState CharacterState = ECharacterState::ECS_Unequipped;

	UPROPERTY(BlueprintReadWrite, meta = (AllowPrivateAccess = "true"))
	EActionState ActionState = EActionState::EAS_Unoccupied;

	UPROPERTY(VisibleAnywhere, Category = Weapon)
	AWeapon* EquippedWeapon;

	EAttackComboState AttackComboState = EAttackComboState::EACS_1;

	/**
	 * Animation Montages
	 **/

	UPROPERTY(EditDefaultsOnly, Category = Montages)
	UAnimMontage* AttackMontage;
	
	UPROPERTY(EditDefaultsOnly, Category = Montages)
	UAnimMontage* EquipMontage;
	
public:
	FORCEINLINE void SetOverlappingItem(AItem* Item) { OverlappingItem = Item; }

	FORCEINLINE ECharacterState GetCharacterState() const { return CharacterState; }
};
