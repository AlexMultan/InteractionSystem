// Copyright Automatic Games sp. z o.o. All Rights Reserved


#include "CharacterPlayer.h"
#include "EnhancedInputComponent.h"
#include "InputActionValue.h"
#include "GameFramework/CharacterMovementComponent.h"
#include "ProjectDynasty/Interfaces/InteractionIntf.h"
#include "ProjectDynasty/Interfaces/OpenableIntf.h"
#include "ProjectDynasty/Objects/ActorFlashlight.h"


// Sets default values
ACharacterPlayer::ACharacterPlayer()
{
	// Set this character to call Tick() every frame.  You can turn this off to improve performance if you don't need it.
	PrimaryActorTick.bCanEverTick = true;

	springArm = CreateDefaultSubobject<USpringArmComponent>("Spring Arm");
	springArm->SetupAttachment(GetMesh(), FName("head"));
	springArm->TargetArmLength = 0.f;
	
	firstPersonCamera = CreateDefaultSubobject<UCameraComponent>(TEXT("FirstPersonCamera"));
	firstPersonCamera->SetupAttachment(springArm);
	firstPersonCamera->bUsePawnControlRotation = true;
	firstPersonCamera->bEnableFirstPersonFieldOfView = true;
	firstPersonCamera->bEnableFirstPersonScale = true;
	firstPersonCamera->FirstPersonFieldOfView = 90.0f;

	armsSpringArm = CreateDefaultSubobject<USpringArmComponent>("Arms Spring Arm");
	armsSpringArm->SetupAttachment(firstPersonCamera);
	armsSpringArm->TargetArmLength = 0.f;
	armsSpringArm->bEnableCameraRotationLag = true;
	armsSpringArm->CameraRotationLagSpeed = 15.f;

	armsMesh = CreateDefaultSubobject<USkeletalMeshComponent>(TEXT("ArmsMesh"));
	armsMesh->SetupAttachment(armsSpringArm);
	//armsMesh->FirstPersonPrimitiveType = EFirstPersonPrimitiveType::FirstPerson;
	armsMesh->SetCollisionProfileName(FName("NoCollision"));
	armsMesh->HideBoneByName(FName("clavicle_l"), PBO_None);

	inventoryComp = CreateDefaultSubobject<UComponentInventory>(TEXT("Inventory Component"));
}


// Called when the game starts or when spawned
void ACharacterPlayer::BeginPlay()
{
	Super::BeginPlay();

	GetCharacterMovement()->MaxWalkSpeed = walkingSpeed;

	FOnTimelineFloat sprintProgressUpdate;
	sprintProgressUpdate.BindUFunction(this, FName("SprintUpdate"));

	FOnTimelineEvent sprintFinishedEvent;
	sprintFinishedEvent.BindUFunction(this, FName("SprintFinished"));

	sprintTimeline.AddInterpFloat(sprintCurve, sprintProgressUpdate);
	sprintTimeline.SetTimelineFinishedFunc(sprintFinishedEvent);

	armsMesh->SetVisibility(false);
	armsMesh->SetCastShadow(false);
	armsMesh->HideBoneByName(FName("clavicle_l"), PBO_None);

	SpawnFlashlight();
}

// Called every frame
void ACharacterPlayer::Tick(float DeltaTime)
{
	Super::Tick(DeltaTime);

	sprintTimeline.TickTimeline(DeltaTime);
	AutoDepthOfField();
	InteractionHover();
	UpdateStamina();
	LimitSpeedByDirection();
	IKLeftHandLocation = HandIK(FName("upperarm_lSocket"), 60.f).hitLocation;
	leftHandHitWall = HandIK(FName("upperarm_lSocket"), 60.f).hasHitWall;
	if (IsValid(flashlightActor))
	{
		flashlightActor->BatteryDrain(isFlashlightEquipped);
	}
	slots = inventoryComp->currentSlotIndex;
}

// Called to bind functionality to input
void ACharacterPlayer::SetupPlayerInputComponent(UInputComponent* PlayerInputComponent)
{
	if (UEnhancedInputComponent* EnhancedInputComponent = Cast<UEnhancedInputComponent>(PlayerInputComponent))
	{
		EnhancedInputComponent->BindAction(moveAction, ETriggerEvent::Triggered, this, &ACharacterPlayer::Move);
		EnhancedInputComponent->BindAction(lookAction, ETriggerEvent::Triggered, this, &ACharacterPlayer::LookAround);
		EnhancedInputComponent->BindAction(jumpAction, ETriggerEvent::Started, this, &ACharacterPlayer::Jump);
		EnhancedInputComponent->BindAction(jumpAction, ETriggerEvent::Completed, this, &ACharacterPlayer::StopJumping);
		EnhancedInputComponent->BindAction(sprintAction, ETriggerEvent::Triggered, this, &ACharacterPlayer::StartSprinting);
		EnhancedInputComponent->BindAction(sprintAction, ETriggerEvent::Ongoing, this, &ACharacterPlayer::StartSprinting);
		EnhancedInputComponent->BindAction(sprintAction, ETriggerEvent::Completed, this, &ACharacterPlayer::StopSprinting);
		EnhancedInputComponent->BindAction(crouchAction, ETriggerEvent::Started, this, &ACharacterPlayer::Crouching);

		EnhancedInputComponent->BindAction(interactAction, ETriggerEvent::Started, this, &ACharacterPlayer::Interact);
		EnhancedInputComponent->BindAction(flashlightAction, ETriggerEvent::Started, this, &ACharacterPlayer::Flashlight);
	}
	else
	{
		UE_LOG(LogTemp, Error, TEXT("'%s' Failed to find an Enhanced Input Component!"), *GetNameSafe(this));
	}
}

void ACharacterPlayer::SpawnFlashlight()
{
	FActorSpawnParameters spawnParameters;
	flashlightActor = GetWorld()->SpawnActor<AActorFlashlight>(flashlightActorClass);
	if (IsValid(flashlightActor))
	{
		flashlightActor->AttachToComponent(armsMesh, FAttachmentTransformRules::SnapToTargetIncludingScale, FName("flashlight"));
		flashlightActor->SetActorHiddenInGame(true);
		flashlightActor->ToggleFlashlight(false);
	}
}

FHandIKParams ACharacterPlayer::HandIK(FName SocketName, float Distance)
{
	/*FHitResult OutHit;
	FVector Start = GetMesh()->GetSocketLocation(SocketName);
	FVector End = Start + GetActorForwardVector() * Distance;
	FCollisionQueryParams TraceParams;

	//DrawDebugLine(GetWorld(), Start, End, FColor::Red, false, 2.f);

	if (GetWorld()->LineTraceSingleByChannel(OutHit, Start, End, ECollisionChannel::ECC_Visibility, TraceParams))
	{
		return FHandIKParams(OutHit.Location, true);
	}*/
	
	return FHandIKParams(FVector::ZeroVector, false);
}

void ACharacterPlayer::Move(const FInputActionValue& Value)
{
	FVector2D _movementVector = Value.Get<FVector2D>();

	if (GetController() && !shouldDisableInput)
	{
		AddMovementInput(GetActorRightVector(), _movementVector.X);
		AddMovementInput(GetActorForwardVector(), _movementVector.Y);
	}
}

void ACharacterPlayer::LookAround(const FInputActionValue& Value)
{
	FVector2D _lookAxisVector = Value.Get<FVector2D>();

	if (GetController() && !shouldDisableInput)
	{
		AddControllerYawInput(_lookAxisVector.X);
		AddControllerPitchInput(_lookAxisVector.Y);
	}
}

void ACharacterPlayer::SprintUpdate(float Alpha)
{
	float _desiredSpeed = 0.f;
	if (currentStamina > 0.f)
	{
		if (isMovingForward)
		{
			if (!isStaminaLow)
			{
				_desiredSpeed = FMath::Lerp(walkingSpeed, sprintingSpeed, Alpha);
			}
			else
			{
				_desiredSpeed = FMath::Lerp(walkingSpeed, jogSpeed, Alpha);
			}
		}
		else
		{
			if (!isStaminaLow)
			{
				_desiredSpeed = FMath::Lerp(walkingSpeed, jogSpeed, Alpha);
			}
			else
			{
				constexpr float lowStaminaSubtractor = 75.f;
				_desiredSpeed = FMath::Lerp(walkingSpeed, jogSpeed - lowStaminaSubtractor, Alpha);
			}
		}
	}
	else
	{
		_desiredSpeed = walkingSpeed;
	}
	GetCharacterMovement()->MaxWalkSpeed = _desiredSpeed;
}

void ACharacterPlayer::LimitSpeedByDirection()
{
	constexpr float MinSprintDirectionAngle = -50.f;
	constexpr float MaxSprintDirectionAngle = 50.f;
	
	if(movementDirection > MinSprintDirectionAngle && movementDirection < MaxSprintDirectionAngle)
	{
		isMovingForward = true;
	}
	else
	{
		isMovingForward = false;
	}
}

void ACharacterPlayer::QuickItemSwap(const FInputActionValue& Value)
{
	inventoryComp->ChangeCurrentSlot(Value.Get<float>());
	input = Value.Get<float>();
}

void ACharacterPlayer::SprintFinished()
{
	
}

void ACharacterPlayer::StartSprinting()
{
	if (isCrouching) { return; }
	sprintTimeline.Play();
	isSprinting = true;
}

void ACharacterPlayer::StopSprinting()
{
	sprintTimeline.Reverse();
	isSprinting = false;
}

void ACharacterPlayer::Crouching()
{
	/*if (MovementState == EMovementState::EMS_Walking)
	{
		MovementState = EMovementState::EMS_Crouching;
		GetCharacterMovement()->MaxWalkSpeed = crouchingSpeed;
	}
	else
	{
		MovementState = EMovementState::EMS_Walking;
		GetCharacterMovement()->MaxWalkSpeed = walkingSpeed;
	}*/

	if (isSprinting) { return; }

	if (!isCrouching)
	{
		isCrouching = true;
		GetCharacterMovement()->MaxWalkSpeed = crouchingSpeed;
		MovementState = EMovementState::EMS_Crouching;
	}
	else
	{
		isCrouching = false;
		GetCharacterMovement()->MaxWalkSpeed = walkingSpeed;
		MovementState = EMovementState::EMS_Walking;
	}
}

void ACharacterPlayer::Interact()
{
	if (shouldDisableInput) { return; }
	
	FHitResult OutHit;
	FVector Start = firstPersonCamera->GetComponentLocation();
	FVector End = firstPersonCamera->GetForwardVector() * interactionTraceLength + Start;
	FCollisionQueryParams TraceParams;

	//DrawDebugLine(GetWorld(), Start, End, FColor::Red, false, 2.f);

	if (GetWorld()->LineTraceSingleByChannel(OutHit, Start, End, ECollisionChannel::ECC_Visibility, TraceParams))
	{
		//DrawDebugPoint(GetWorld(), OutHit.Location, 20.f, FColor::Cyan, false, 3.f);

		if (IInteractionIntf* interactionInterface = Cast<IInteractionIntf>(OutHit.GetActor()))
		{
			interactionInterface->OnInteract(this);
			switch (interactionInterface->GetItemType())
			{
			case EItemType::EIT_Valve:
				if (!valveInteractionAnim) { return; }
				PlayInteractionAnimation(valveInteractionAnim);
				break;
			case EItemType::EIT_WallLever:
				if (!wallLeverInteractionAnim) { return; }
				PlayInteractionAnimation(wallLeverInteractionAnim);
				break;
			case EItemType::EIT_SchoolLocker:
				if (!schoolLockerInteractionAnim) { return; }
				PlayInteractionAnimation(schoolLockerInteractionAnim);
				break;
			case EItemType::EIT_SmallUpperSchoolLocker:
				if (!smallUpperSchoolLockerInteractionAnim) { return; }
				PlayInteractionAnimation(smallUpperSchoolLockerInteractionAnim);
				break;
			case EItemType::EIT_Drawer100cm:
				if (!drawer100cmInteractionAnim) { return; }
				PlayInteractionAnimation(drawer100cmInteractionAnim);
				break;
			case EItemType::EIT_Pickable:
				switch (interactionInterface->GetPickUpHeight())
				{
				case EPickUpHeight::EPH_Floor:
					if (!pickUpAnimations[0]) { return; }
					PlayPickUpAnimation(pickUpAnimations[0], interactionInterface->GetStaticMesh(), OutHit.GetActor(),
						interactionInterface->GetPickUpHeight());
					break;
				case EPickUpHeight::EPH_Table:
					if (!pickUpAnimations[1]) { return; }
					PlayPickUpAnimation(pickUpAnimations[1], interactionInterface->GetStaticMesh(), OutHit.GetActor(),
						interactionInterface->GetPickUpHeight());
					break;
				case EPickUpHeight::EPH_Top:
					if (!pickUpAnimations[2]) { return; }
					PlayPickUpAnimation(pickUpAnimations[2], interactionInterface->GetStaticMesh(), OutHit.GetActor(),
						interactionInterface->GetPickUpHeight());
					break;
				default:
					break;
				}
			default:
				break;
			}
		}

		if (IOpenableIntf* openableInterface = Cast<IOpenableIntf>(OutHit.GetActor()))
		{
			if (!openableInterface->CanInteract(this)) { return; }
			if (!openDoorAnim) { return; }
			openableInterface->OnOpen(this);
			PlayInteractionAnimation(openDoorAnim);
		}
	}
}

void ACharacterPlayer::InteractionHover()
{
	if (shouldDisableInput) { ToggleInteractionPrompt(FName("false")); return; }
	
	FHitResult OutHit;
	FVector Start = firstPersonCamera->GetComponentLocation();
	FVector End = firstPersonCamera->GetForwardVector() * interactionTraceLength + Start;
	FCollisionQueryParams TraceParams;

	//DrawDebugLine(GetWorld(), Start, End, FColor::Red, false, 2.f);

	if (GetWorld()->LineTraceSingleByChannel(OutHit, Start, End, ECollisionChannel::ECC_Visibility, TraceParams))
	{
		//DrawDebugPoint(GetWorld(), OutHit.Location, 20.f, FColor::Cyan, false, 3.f);
		if (IOpenableIntf* openableInterface = Cast<IOpenableIntf>(OutHit.GetActor()))
		{
			if (!openableInterface->CanInteract(this)) { return; }
			switch (openableInterface->GetDoorState())
			{
			case EDoorState::EDS_Closed:
				ToggleInteractionPrompt(FName("OPEN"));
				break;
			case EDoorState::EDS_Opened:
				ToggleInteractionPrompt(FName("CLOSE"));
				break;
			default:
				break;
			}
		}
		else if (IInteractionIntf* interactionInterface = Cast<IInteractionIntf>(OutHit.GetActor()))
		{
			switch (interactionInterface->GetItemType())
			{
			case EItemType::EIT_Valve:
				ToggleInteractionPrompt(FName("USE VALVE"));
				break;
			case EItemType::EIT_WallLever:
				ToggleInteractionPrompt(FName("USE LEVER"));
				break;
			case EItemType::EIT_SchoolLocker:
				ToggleInteractionPrompt(FName("OPEN"));
				break;
			case EItemType::EIT_SmallUpperSchoolLocker:
				ToggleInteractionPrompt(FName("OPEN"));
				break;
			default:
				break;
			}
		}
		else
		{
			ToggleInteractionPrompt(FName("false"));
		}
	}
	else
	{
		ToggleInteractionPrompt(FName("false"));
	}
}

void ACharacterPlayer::Flashlight()
{
	if (!equipFlashlightAnim && !unequipFlashlightAnim) { return; }
	if (!isFlashlightEquipped)
	{
		armsMesh->SetVisibility(true);
		PlayFlashlightAnimation(equipFlashlightAnim);
		GetMesh()->HideBoneByName(FName("upperarm_r"), PBO_None);
		isFlashlightEquipped = true;
		armsMesh->SetCastShadow(true);
		flashlightActor->SetActorHiddenInGame(false);
	}
	else
	{
		PlayFlashlightAnimation(unequipFlashlightAnim);
		isFlashlightEquipped = false;
	}
}

float ACharacterPlayer::AutoDepthOfField()
{
	if (isInteracting) { return 0.f; }
	FHitResult _outHit;
	FVector start = firstPersonCamera->GetComponentLocation();
	FVector end = firstPersonCamera->GetForwardVector() * 100.f + start;
	FCollisionQueryParams traceParams;

	//DrawDebugLine(GetWorld(), start, end, FColor::Black, false, 2.f);

	if (GetWorld()->LineTraceSingleByChannel(_outHit, start, end, ECollisionChannel::ECC_Visibility, traceParams))
	{
		//DrawDebugPoint(GetWorld(), _outHit.Location, 30.f, FColor::Red, false, 3.f);
		float _focusDistance = (_outHit.Location - firstPersonCamera->GetComponentLocation()).Length();
		return _focusDistance;
	}
	return 0.f;
}

void ACharacterPlayer::UpdateStamina()
{
	currentStamina = FMath::Clamp(currentStamina, 0.f, MAX_STAMINA);
	if (isSprinting)
	{
		float staminaConsumption = 0.09f;
		// Decrease stamina over time while sprinting
		currentStamina -= staminaConsumption * GetWorld()->GetDeltaSeconds();
	}
	else
	{
		float staminaRegainer = 0.13f;
		// Increase stamina over time when not sprinting
		currentStamina += staminaRegainer * GetWorld()->GetDeltaSeconds();
	}

	// Determine if player has low stamina based on threshold
	if(currentStamina >= lowStaminaLevel)
	{
		isStaminaLow = false;
	}
	else
	{
		isStaminaLow = true;
	}
}

