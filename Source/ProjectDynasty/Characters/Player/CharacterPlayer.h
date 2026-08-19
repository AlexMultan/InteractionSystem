// Copyright Automatic Games sp. z o.o. All Rights Reserved

#pragma once

#include "CoreMinimal.h"
#include "InputAction.h"
#include "Camera/CameraComponent.h"
#include "Components/TimelineComponent.h"
#include "GameFramework/Character.h"
#include "GameFramework/SpringArmComponent.h"
#include "ProjectDynasty/Components/ComponentInventory.h"
#include "ProjectDynasty/Interfaces/InteractionIntf.h"
#include "ProjectDynasty/Objects/ActorFlashlight.h"
#include "CharacterPlayer.generated.h"

class UInputMappingContext;

USTRUCT(BlueprintType)
struct FHandIKParams
{
	GENERATED_BODY()

	UPROPERTY(EditAnywhere, BlueprintReadOnly)
	FVector hitLocation = FVector::ZeroVector;

	UPROPERTY(EditAnywhere, BlueprintReadOnly)
	bool hasHitWall = false;
};

UENUM(BlueprintType)
enum class EMovementState : uint8
{
	EMS_Walking,
	EMS_Running,
	EMS_Crouching
};

UCLASS()
class PROJECTDYNASTY_API ACharacterPlayer : public ACharacter
{
	GENERATED_BODY()

	UPROPERTY(VisibleAnywhere, Category="Default Settings")
	USpringArmComponent* springArm;

	UPROPERTY(VisibleAnywhere, Category="Default Settings")
	USpringArmComponent* armsSpringArm;

public:
	// Sets default values for this character's properties
	ACharacterPlayer();

	UPROPERTY(EditAnywhere, BlueprintReadWrite)
	EMovementState MovementState = EMovementState::EMS_Walking;

	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Player Components")
	UComponentInventory* inventoryComp;

private:

	UPROPERTY(EditDefaultsOnly, Category="Movement|Speed", meta=(AllowPrivateAccess = "true"))
	float walkingSpeed = 150.f;

	UPROPERTY(EditDefaultsOnly, Category="Movement|Speed", meta=(AllowPrivateAccess = "true"))
	float sprintingSpeed = 400.f;

	UPROPERTY(EditDefaultsOnly, Category="Movement|Speed", meta=(AllowPrivateAccess = "true"))
	float jogSpeed = 250.f;

	UPROPERTY(EditDefaultsOnly, Category="Movement|Speed", meta=(AllowPrivateAccess = "true"))
	float crouchingSpeed = 100.f;

	UPROPERTY(EditDefaultsOnly, Category="Default Settings", meta=(AllowPrivateAccess = "true"))
	int32 interactionTraceLength = 200.f;

	UPROPERTY(EditDefaultsOnly, Category="Default Settings", meta=(AllowPrivateAccess = "true"))
	UClass* flashlightActorClass;

	UPROPERTY(EditDefaultsOnly, Category ="Default Settings", meta=(AllowPrivateAccess = "true"))
	float lowStaminaLevel = 0.2f;

	UPROPERTY(EditDefaultsOnly, Category ="Default Settings", meta=(AllowPrivateAccess = "true"))
	bool isStaminaLow = false;

	UPROPERTY(VisibleAnywhere, BlueprintReadOnly, Category = "DO NOT FUCKING TOUCH IT", meta=(AllowPrivateAccess = "true"))
	float MAX_STAMINA = 1.0f;

	UPROPERTY(VisibleAnywhere, BlueprintReadOnly, Category = "Default Settings", meta=(AllowPrivateAccess = "true"))
	bool isMovingForward = false;

	UPROPERTY(VisibleAnywhere, BlueprintReadOnly, Category = "Default Settings", meta=(AllowPrivateAccess = "true"))
	float movementDirection;

	void SpawnFlashlight();
	FHandIKParams HandIK(FName SocketName, float Distance);


#pragma region Animations

	UPROPERTY(EditDefaultsOnly, Category="Animations")
	UAnimMontage* valveInteractionAnim = nullptr;

	UPROPERTY(EditDefaultsOnly, Category="Animations")
	UAnimMontage* wallLeverInteractionAnim = nullptr;

	UPROPERTY(EditDefaultsOnly, Category="Animations")
	UAnimMontage* schoolLockerInteractionAnim = nullptr;

	UPROPERTY(EditDefaultsOnly, Category="Animations")
	UAnimMontage* smallUpperSchoolLockerInteractionAnim = nullptr;

	UPROPERTY(EditDefaultsOnly, Category="Animations")
	UAnimMontage* drawer100cmInteractionAnim = nullptr;

	UPROPERTY(EditDefaultsOnly, Category="Animations")
	UAnimMontage* equipFlashlightAnim = nullptr;

	UPROPERTY(EditDefaultsOnly, Category="Animations")
	UAnimMontage* unequipFlashlightAnim = nullptr;

	UPROPERTY(EditDefaultsOnly, Category="Animations")
	TArray<UAnimMontage*> pickUpAnimations;

	UPROPERTY(EditDefaultsOnly, Category="Animations")
	UAnimMontage* openDoorAnim = nullptr;

#pragma endregion

#pragma region Timelines
	
	FTimeline sprintTimeline;

	UFUNCTION()
	void SprintUpdate(float Alpha);

	UFUNCTION()
	void SprintFinished();

#pragma endregion
	
protected:
	// Called when the game starts or when spawned
	virtual void BeginPlay() override;

#pragma region Inputs
	
	UPROPERTY(EditDefaultsOnly, Category="Input|Actions")
	UInputAction* moveAction;

	UPROPERTY(EditDefaultsOnly, Category="Input|Actions")
	UInputAction* lookAction;

	UPROPERTY(EditDefaultsOnly, Category="Input|Actions")
	UInputAction* jumpAction;

	UPROPERTY(EditDefaultsOnly, Category="Input|Actions")
	UInputAction* sprintAction;

	UPROPERTY(EditDefaultsOnly, Category="Input|Actions")
	UInputAction* crouchAction;

	UPROPERTY(EditDefaultsOnly, Category="Input|Actions")
	UInputAction* interactAction;

	UPROPERTY(EditDefaultsOnly, Category="Input|Actions")
	UInputAction* flashlightAction;

	UPROPERTY(EditDefaultsOnly, Category="Input|Actions")
	UInputAction* quickItemSwapAction;

#pragma endregion
	
	UPROPERTY(EditDefaultsOnly, Category="Movement|Curves")
	UCurveFloat* sprintCurve;

	UPROPERTY(VisibleAnywhere, BlueprintReadWrite)
	bool shouldDisableInput = false;

	UPROPERTY(VisibleAnywhere, BlueprintReadWrite, Category="Default Settings")
	UCameraComponent* firstPersonCamera;

	UPROPERTY(VisibleAnywhere, BlueprintReadWrite, Category="Default Settings")
	USkeletalMeshComponent* armsMesh;

	UPROPERTY(VisibleAnywhere, BlueprintReadWrite)
	bool isInteracting = false;

	UPROPERTY(VisibleAnywhere, BlueprintReadWrite)
	AActorFlashlight* flashlightActor = nullptr;

	UPROPERTY(EditDefaultsOnly, BlueprintReadOnly, Category ="Default Settings", meta=(AllowPrivateAccess = "true"))
	float currentStamina = 1.0f;

public:
	
	virtual void Tick(float DeltaTime) override;
	virtual void SetupPlayerInputComponent(class UInputComponent* PlayerInputComponent) override;

	UPROPERTY(EditAnywhere, BlueprintReadWrite)
	bool isCrouching;

	UPROPERTY(EditAnywhere, BlueprintReadWrite)
	bool isSprinting;

	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category="Default Settings")
	bool isFlashlightEquipped = false;

	UPROPERTY(EditAnywhere, BlueprintReadWrite)
	FVector IKLeftHandLocation;

	UPROPERTY(EditAnywhere, BlueprintReadWrite)
	bool leftHandHitWall;

	UPROPERTY(EditAnywhere, BlueprintReadWrite)
	int32 slots = 0;

	UPROPERTY(EditAnywhere, BlueprintReadWrite)
	float input;

protected:

	void Move(const FInputActionValue& Value);
	void LookAround(const FInputActionValue& Value);
	void StartSprinting();
	void StopSprinting();
	void Crouching();
	void Interact();
	void InteractionHover();
	void Flashlight();
	void UpdateStamina();
	void LimitSpeedByDirection();
	
	UFUNCTION(BlueprintCallable)
	void QuickItemSwap(const FInputActionValue& Value);

	UFUNCTION(BlueprintCallable)
	float AutoDepthOfField();

	UFUNCTION(BlueprintImplementableEvent)
	void PlayInteractionAnimation(UAnimMontage* InteractionAnimation);

	UFUNCTION(BlueprintImplementableEvent)
	void PlayPickUpAnimation(UAnimMontage* InteractionAnimation, UStaticMesh* ItemMesh, AActor* ItemActor, EPickUpHeight PickUpHeight);

	UFUNCTION(BlueprintImplementableEvent)
	void ToggleInteractionPrompt(FName InteractionType);

	UFUNCTION(BlueprintImplementableEvent)
	void PlayFlashlightAnimation(UAnimMontage* Animation);

	UFUNCTION(BlueprintPure, BlueprintCallable)
	FORCEINLINE AActorFlashlight* GetFlashlightActor() const
	{
		return IsValid(flashlightActor) ? flashlightActor : nullptr;
	}
};
