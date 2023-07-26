// Fill out your copyright notice in the Description page of Project Settings.


#include "Characters/uNclearCharacter.h"
#include "GameFramework/SpringArmComponent.h"
#include "Camera/CameraComponent.h"
#include "GameFramework/CharacterMovementComponent.h"
#include "GroomComponent.h"
#include "Items/Item.h"
#include "Items/Weapons/Weapon.h"
#include "Animation/AnimMontage.h"
#include "Components/StaticMeshComponent.h"
#include "Components/AttributeComponent.h"
#include "HUD/uNclearHUD.h"
#include "HUD/uNclearOverlay.h"
#include "Items/Soul.h"
#include "Items/Treasure.h"
#include "Components/InputComponent.h"
#include "EnhancedInputSubsystems.h"
#include "EnhancedInputComponent.h"
#include "InputActionValue.h"

AuNclearCharacter::AuNclearCharacter()
{
	PrimaryActorTick.bCanEverTick = true;

	// Set up Spring Arm Component
	SpringArm = CreateDefaultSubobject<USpringArmComponent>(TEXT("SpringArm"));
	SpringArm->SetupAttachment(GetRootComponent());
	SpringArm->TargetArmLength = 300.f;
	SpringArm->bUsePawnControlRotation = true;

	// Set up Camera Component
	ViewCamera = CreateDefaultSubobject<UCameraComponent>(TEXT("ViewCamera"));
	ViewCamera->SetupAttachment(SpringArm);

	// Set up Hair Component
	Hair = CreateDefaultSubobject<UGroomComponent>(TEXT("Hair"));
	Hair->SetupAttachment(GetMesh());
	Hair->AttachmentName = FString("head");

	// Set up Eyebrows Component
	Eyebrows = CreateDefaultSubobject<UGroomComponent>(TEXT("Eyebrows"));
	Eyebrows->SetupAttachment(GetMesh());
	Eyebrows->AttachmentName = FString("head");

	// Do not use Controller Rotation
	bUseControllerRotationPitch = false;
	bUseControllerRotationYaw = false;
	bUseControllerRotationRoll = false;

	// Orient Character Rotation to Movement
	GetCharacterMovement()->bOrientRotationToMovement = true;
	GetCharacterMovement()->RotationRate = FRotator(0.f, 400.f, 0.f);

	// Configure Collision Settings
	GetMesh()->SetCollisionObjectType(ECollisionChannel::ECC_WorldDynamic);
	GetMesh()->SetCollisionResponseToAllChannels(ECollisionResponse::ECR_Ignore);
	GetMesh()->SetCollisionResponseToChannel(ECollisionChannel::ECC_Visibility, ECollisionResponse::ECR_Block);
	GetMesh()->SetCollisionResponseToChannel(ECollisionChannel::ECC_WorldDynamic, ECollisionResponse::ECR_Overlap);
	GetMesh()->SetGenerateOverlapEvents(true);
}

void AuNclearCharacter::Tick(float DeltaTime)
{
	if (Attributes && uNclearOverlay)
	{
		Attributes->RegenStamina(DeltaTime);
		uNclearOverlay->SetStaminaBarPercent(Attributes->GetStaminaPercent());
	}
}

void AuNclearCharacter::BeginPlay()
{
	Super::BeginPlay();

	// Add Input Mapping Context
	if (APlayerController* PlayerController = Cast<APlayerController>(GetController()))
	{
		if (UEnhancedInputLocalPlayerSubsystem* Subsystem = ULocalPlayer::GetSubsystem<UEnhancedInputLocalPlayerSubsystem>(PlayerController->GetLocalPlayer()))
		{
			Subsystem->AddMappingContext(uNclearCharacterContext, 0);
		}
	}

	// Tag for Enemy Pawn Sensing
	Tags.Add(FName("EngageableTarget"));

	InitializeuNclearOverlay();
}

void AuNclearCharacter::Movement(const FInputActionValue& Value)
{
	if (ActionState != EActionState::EAS_Unoccupied) return;
	
	const FVector2D MovementVector = Value.Get<FVector2D>();
	if (GetController())
	{
		// Find Controller Rotation
		const FRotator ControlRotation = GetControlRotation();
		const FRotator YawRotation(0.f, ControlRotation.Yaw, 0.f);

		// Find Forward and Right Directions
		const FVector ForwardDirection = FRotationMatrix(YawRotation).GetUnitAxis(EAxis::X);
		const FVector RightDirection = FRotationMatrix(YawRotation).GetUnitAxis(EAxis::Y);

		// Move Character accordingly
		AddMovementInput(ForwardDirection, MovementVector.Y);
		AddMovementInput(RightDirection, MovementVector.X);
	}
}

void AuNclearCharacter::Looking(const FInputActionValue& Value)
{
	const FVector2D LookAxisValue = Value.Get<FVector2D>();
	if (GetController())
	{
		AddControllerYawInput(LookAxisValue.X);
		AddControllerPitchInput(LookAxisValue.Y);
	}
}

void AuNclearCharacter::Jump()
{
	if (IsOccupied() || !HasEnoughStamina(Attributes->GetJumpCost())) return;
	Super::Jump();
	if (Attributes && uNclearOverlay)
	{
		Attributes->UseStamina(Attributes->GetJumpCost());
		uNclearOverlay->SetStaminaBarPercent(Attributes->GetStaminaPercent());
	}
}

void AuNclearCharacter::EKeyPressed()
{
	AWeapon* OverlappingWeapon = Cast<AWeapon>(OverlappingItem);
	if (OverlappingWeapon)
	{
		if (EquippedWeapon)
		{
			EquippedWeapon->Destroy();
		}
		EquipWeapon(OverlappingWeapon);
	}
	else
	{
		if (CanDisarm())
		{
			Disarm();
		}
		else if (CanArm())
		{
			Arm();
		}
	}
}

void AuNclearCharacter::Attack()
{
	Super::Attack();
	
	if (!CanAttack() || !HasEnoughStamina(Attributes->GetAttackCost())) return;
	PlayAttackMontage();
	ActionState = EActionState::EAS_Attacking;
	if (Attributes && uNclearOverlay)
	{
		Attributes->UseStamina(Attributes->GetAttackCost());
		uNclearOverlay->SetStaminaBarPercent(Attributes->GetStaminaPercent());
	}
}

void AuNclearCharacter::Dodge()
{
	if (IsOccupied() || !HasEnoughStamina(Attributes->GetDodgeCost())) return;
	PlayDodgeMontage();
	ActionState = EActionState::EAS_Dodging;
	if (Attributes && uNclearOverlay)
	{
		Attributes->UseStamina(Attributes->GetDodgeCost());
		uNclearOverlay->SetStaminaBarPercent(Attributes->GetStaminaPercent());
	}
}

void AuNclearCharacter::AttackEnd()
{
	ActionState = EActionState::EAS_Unoccupied;
}

void AuNclearCharacter::DodgeEnd()
{
	Super::DodgeEnd();

	ActionState = EActionState::EAS_Unoccupied;
}

bool AuNclearCharacter::CanAttack()
{
	return ActionState == EActionState::EAS_Unoccupied &&
		CharacterState != ECharacterState::ECS_Unequipped;
}

void AuNclearCharacter::EquipWeapon(AWeapon* Weapon)
{
	Weapon->Equip(GetMesh(), FName("RightHandSocket"), this, this);
	CharacterState = ECharacterState::ECS_EquippedOneHandedWeapon;
	OverlappingItem = nullptr;
	EquippedWeapon = Weapon;
}

void AuNclearCharacter::PlayEquipMontage(const FName& SectionName)
{
	UAnimInstance* AnimInstance = GetMesh()->GetAnimInstance();
	if (AnimInstance && EquipMontage)
	{
		AnimInstance->Montage_Play(EquipMontage);
		AnimInstance->Montage_JumpToSection(SectionName, EquipMontage);
	}
}

void AuNclearCharacter::Die()
{
	Super::Die();

	ActionState = EActionState::EAS_Dead;
	DisableMeshCollision();
}

bool AuNclearCharacter::HasEnoughStamina(const float ActionCost)
{
	return Attributes && Attributes->GetStamina() > ActionCost;
}

bool AuNclearCharacter::CanDisarm()
{
	return ActionState == EActionState::EAS_Unoccupied && 
		CharacterState != ECharacterState::ECS_Unequipped;
}

bool AuNclearCharacter::CanArm()
{
	return ActionState == EActionState::EAS_Unoccupied &&
		CharacterState == ECharacterState::ECS_Unequipped && 
		EquippedWeapon;
}

void AuNclearCharacter::Disarm()
{
	PlayEquipMontage(FName("Unequip"));
	CharacterState = ECharacterState::ECS_Unequipped;
	ActionState = EActionState::EAS_EquippingWeapon;
}

void AuNclearCharacter::Arm()
{
	PlayEquipMontage(FName("Equip"));
	CharacterState = ECharacterState::ECS_EquippedOneHandedWeapon;
	ActionState = EActionState::EAS_EquippingWeapon;
}

bool AuNclearCharacter::IsOccupied()
{
	return ActionState != EActionState::EAS_Unoccupied;
}

void AuNclearCharacter::AttachWeaponToBack()
{
	if (EquippedWeapon)
	{
		EquippedWeapon->AttachMeshToSocket(GetMesh(), FName("SpineSocket"));
	}
}

void AuNclearCharacter::AttachWeaponToHand()
{
	if (EquippedWeapon)
	{
		EquippedWeapon->AttachMeshToSocket(GetMesh(), FName("RightHandSocket"));
	}
}

void AuNclearCharacter::FinishEquipping()
{
	ActionState = EActionState::EAS_Unoccupied;
}

void AuNclearCharacter::HitReactEnd()
{
	ActionState = EActionState::EAS_Unoccupied;
}

void AuNclearCharacter::InitializeuNclearOverlay()
{
	APlayerController* PlayerController = Cast<APlayerController>(GetController());
	if (PlayerController)
	{
		AuNclearHUD* uNclearHUD = Cast<AuNclearHUD>(PlayerController->GetHUD());
		if (uNclearHUD)
		{
			uNclearOverlay = uNclearHUD->GetuNclearOverlay();
			if (uNclearOverlay && Attributes)
			{
				uNclearOverlay->SetHealthBarPercent(Attributes->GetHealthPercent());
				uNclearOverlay->SetStaminaBarPercent(1.f);
				uNclearOverlay->SetGold(0);
				uNclearOverlay->SetSouls(0);
			}
		}
	}
}

bool AuNclearCharacter::IsUnoccupied()
{
	return ActionState == EActionState::EAS_Unoccupied;
}

void AuNclearCharacter::SetHUDHealth()
{
	if (uNclearOverlay && Attributes)
	{
		uNclearOverlay->SetHealthBarPercent(Attributes->GetHealthPercent());
	}
}

void AuNclearCharacter::SetupPlayerInputComponent(UInputComponent* PlayerInputComponent)
{
	Super::SetupPlayerInputComponent(PlayerInputComponent);

	if (UEnhancedInputComponent* EnhancedInputComponent = CastChecked<UEnhancedInputComponent>(PlayerInputComponent))
	{
		EnhancedInputComponent->BindAction(MovementAction, ETriggerEvent::Triggered, this, &AuNclearCharacter::Movement);
		EnhancedInputComponent->BindAction(LookingAction, ETriggerEvent::Triggered, this, &AuNclearCharacter::Looking);
		EnhancedInputComponent->BindAction(JumpAction, ETriggerEvent::Triggered, this, &AuNclearCharacter::Jump);
		EnhancedInputComponent->BindAction(EKeyAction, ETriggerEvent::Triggered, this, &AuNclearCharacter::EKeyPressed);
		EnhancedInputComponent->BindAction(AttackAction, ETriggerEvent::Triggered, this, &AuNclearCharacter::Attack);
		EnhancedInputComponent->BindAction(DodgeAction, ETriggerEvent::Triggered, this, &AuNclearCharacter::Dodge);
	}
}

void AuNclearCharacter::GetHit_Implementation(const FVector& ImpactPoint, AActor* Hitter)
{
	Super::GetHit_Implementation(ImpactPoint, Hitter);
	SetWeaponCollisionEnabled(ECollisionEnabled::NoCollision);
	if (Attributes && Attributes->GetHealthPercent() > 0.f)
	{
		ActionState = EActionState::EAS_HitReaction;
	}
}

float AuNclearCharacter::TakeDamage(float DamageAmount, FDamageEvent const& DamageEvent, AController* EventInstigator,
	AActor* DamageCauser)
{
	HandleDamage(DamageAmount);
	SetHUDHealth();
	return DamageAmount;
}

void AuNclearCharacter::SetOverlappingItem(AItem* Item)
{
	OverlappingItem = Item;
}

void AuNclearCharacter::AddSouls(ASoul* Soul)
{
	if (Attributes && uNclearOverlay)
	{
		Attributes->AddSouls(Soul->GetSouls());
		uNclearOverlay->SetSouls(Attributes->GetSouls());
	}
}

void AuNclearCharacter::AddGold(ATreasure* Treasure)
{
	if (Attributes && uNclearOverlay)
	{
		Attributes->AddGold(Treasure->GetGold());
		uNclearOverlay->SetGold(Attributes->GetGold());
	}
}
