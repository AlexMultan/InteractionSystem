// Copyright Automatic Games sp. z o.o. All Rights Reserved


#include "ActorDoorBase.h"
#include "Kismet/GameplayStatics.h"


// Sets default values
AActorDoorBase::AActorDoorBase()
{
	// Set this actor to call Tick() every frame.  You can turn this off to improve performance if you don't need it.
	PrimaryActorTick.bCanEverTick = true;

	root = CreateDefaultSubobject<USceneComponent>("Root");
	root->SetupAttachment(RootComponent);

	staticMesh = CreateDefaultSubobject<UStaticMeshComponent>("Static Mesh");
	staticMesh->SetupAttachment(root);

	boxCollison = CreateDefaultSubobject<UBoxComponent>("Box Collision");
	boxCollison->SetupAttachment(root);

	boxCollison1 = CreateDefaultSubobject<UBoxComponent>("Box Collision 1");
	boxCollison1->SetupAttachment(root);

	interactionTransform = CreateDefaultSubobject<USceneComponent>("Interaction Location");
	interactionTransform->SetupAttachment(root);

	interactionTransform1 = CreateDefaultSubobject<USceneComponent>("Interaction Location 1");
	interactionTransform1->SetupAttachment(root);
}

// Called when the game starts or when spawned
void AActorDoorBase::BeginPlay()
{
	Super::BeginPlay();

	FOnTimelineFloat openDoorProgressUpdate;
	openDoorProgressUpdate.BindUFunction(this, FName("OpenDoorUpdate"));

	FOnTimelineEvent openDoorFinishedEvent;
	openDoorFinishedEvent.BindUFunction(this, FName("OpenDoorFinished"));

	openDoorTimeline.AddInterpFloat(openDoorCurve, openDoorProgressUpdate);
	openDoorTimeline.SetTimelineFinishedFunc(openDoorFinishedEvent);

	FOnTimelineFloat closeDoorProgressUpdate;
	closeDoorProgressUpdate.BindUFunction(this, FName("CloseDoorUpdate"));

	FOnTimelineEvent closeDoorFinishedEvent;
	closeDoorFinishedEvent.BindUFunction(this, FName("CloseDoorFinished"));

	closeDoorTimeline.AddInterpFloat(closeDoorCurve, closeDoorProgressUpdate);
	closeDoorTimeline.SetTimelineFinishedFunc(closeDoorFinishedEvent);

	FOnTimelineFloat moveToComponentProgressUpdate;
	moveToComponentProgressUpdate.BindUFunction(this, FName("MoveToComponentUpdate"));

	FOnTimelineEvent moveToComponentFinishedEvent;
	moveToComponentFinishedEvent.BindUFunction(this, FName("MoveToComponentFinished"));

	moveToComponentTimeline.AddInterpFloat(moveToComponentCurve, moveToComponentProgressUpdate);
	moveToComponentTimeline.SetTimelineFinishedFunc(moveToComponentFinishedEvent);
}

// Called every frame
void AActorDoorBase::Tick(float DeltaTime)
{
	Super::Tick(DeltaTime);

	openDoorTimeline.TickTimeline(DeltaTime);
	closeDoorTimeline.TickTimeline(DeltaTime);
	moveToComponentTimeline.TickTimeline(DeltaTime);
}

void AActorDoorBase::OnOpen(AActor* Player)
{
	if (IsValid(Player))
	{
		playerActor = Player;
		if (enableInteraction)
		{
			if (!openDoorSound && !closeDoorSound) { return; }
			switch (doorState)
			{
			case EDoorState::EDS_Closed:
				openDoorTimeline.PlayFromStart();
				moveToComponentTimeline.PlayFromStart();
				UGameplayStatics::PlaySoundAtLocation(GetWorld(), openDoorSound, GetActorLocation());
				break;
			case EDoorState::EDS_Opened:
				openDoorTimeline.ReverseFromEnd();
				moveToComponentTimeline.PlayFromStart();
				UGameplayStatics::PlaySoundAtLocation(GetWorld(), closeDoorSound, GetActorLocation());
				break;
			default:
				break;
			}
		}
	}
}

EDoorState AActorDoorBase::GetDoorState()
{
	return doorState;
}

bool AActorDoorBase::CanInteract(AActor* Player)
{
	if (IsValid(Player) && (boxCollison->IsOverlappingActor(Player) || boxCollison1->IsOverlappingActor(Player)))
	{
		return true;
	}
	return false;
}

void AActorDoorBase::OpenDoorUpdate(float Alpha)
{
	if (!IsValid(playerActor))
	{
		return;
	}

	enableInteraction = false;

	if (doorState == EDoorState::EDS_Closed)
	{
		if (boxCollison->IsOverlappingActor(playerActor))
		{
			DoorAnimation(Alpha, 0.f, openedDoorYawRotation);
		}
		else if (boxCollison1->IsOverlappingActor(playerActor))
		{
			DoorAnimation(Alpha, 0.f, openedDoorYawRotationNegative);
		}
	}
	else if (doorState == EDoorState::EDS_Opened)
	{
		if (staticMesh->GetRelativeRotation().Yaw > 0.f)
		{
			DoorAnimation(Alpha, 0.f, openedDoorYawRotation);
		}
		else if (staticMesh->GetRelativeRotation().Yaw < 0.f)
		{
			DoorAnimation(Alpha, 0.f, openedDoorYawRotationNegative);
		}
	}
}

void AActorDoorBase::OpenDoorFinished()
{
	enableInteraction = true;
	if (doorState == EDoorState::EDS_Closed)
	{
		doorState = EDoorState::EDS_Opened;
	}
	else if (doorState == EDoorState::EDS_Opened)
	{
		doorState = EDoorState::EDS_Closed;
	}
	
}

void AActorDoorBase::CloseDoorUpdate(float Alpha)
{
	enableInteraction = false;
	if (staticMesh->GetComponentRotation().Yaw > 0.f)
	{
		DoorAnimation(Alpha, openedDoorYawRotation, 0.f);
	}
	else if (staticMesh->GetComponentRotation().Yaw < 0.f)
	{
		DoorAnimation(Alpha, openedDoorYawRotationNegative, 0.f);
	}
}

void AActorDoorBase::CloseDoorFinished()
{
	enableInteraction = true;
	doorState = EDoorState::EDS_Closed;
}

void AActorDoorBase::DoorAnimation(float Alpha, float CurrentYawRotation, float TargetYawRotation) const
{
	staticMesh->SetRelativeRotation(FRotator(0.f, FMath::Lerp(CurrentYawRotation, TargetYawRotation,
		Alpha), 0.f));
}

void AActorDoorBase::MoveToComponentUpdate(float Alpha)
{
	if (!IsValid(playerActor) || !IsValid(interactionTransform) || !IsValid(interactionTransform1)) { return; }

	FVector targetLocation;
	FRotator targetRotation;
	if (boxCollison->IsOverlappingActor(playerActor))
	{
		targetLocation = interactionTransform->GetComponentLocation();
		targetRotation = interactionTransform->GetComponentRotation();
	}
	else if (boxCollison1->IsOverlappingActor(playerActor))
	{
		targetLocation = interactionTransform1->GetComponentLocation();
		targetRotation = interactionTransform1->GetComponentRotation();
	}
	
	FVector CurrentLoc = playerActor->GetActorLocation();
	FVector TargetLoc  = targetLocation;
	FVector NewLoc(
		FMath::Lerp(CurrentLoc.X, TargetLoc.X, Alpha),
		FMath::Lerp(CurrentLoc.Y, TargetLoc.Y, Alpha),
		CurrentLoc.Z
	);
	playerActor->SetActorLocation(NewLoc);
	
	float CurrentYaw = GetWorld()->GetFirstPlayerController()->GetControlRotation().Yaw;
	float TargetYaw  = targetRotation.Yaw;
	
	float DeltaYaw = FMath::FindDeltaAngleDegrees(CurrentYaw, TargetYaw);
	
	float NewYaw = CurrentYaw + DeltaYaw * Alpha;
	NewYaw = FMath::Fmod(NewYaw + 360.0f, 360.0f);

	GetWorld()->GetFirstPlayerController()->SetControlRotation(FRotator(0.f, NewYaw, 0.f));
}

void AActorDoorBase::MoveToComponentFinished()
{
	
}

