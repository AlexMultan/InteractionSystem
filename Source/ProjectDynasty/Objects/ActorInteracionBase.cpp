// Copyright Automatic Games sp. z o.o. All Rights Reserved


#include "ActorInteracionBase.h"
#include "Math/UnrealMathUtility.h"


// Sets default values
AActorInteracionBase::AActorInteracionBase()
{
	// Set this actor to call Tick() every frame.  You can turn this off to improve performance if you don't need it.
	PrimaryActorTick.bCanEverTick = true;

	root = CreateDefaultSubobject<USceneComponent>("Root");
	root->SetupAttachment(RootComponent);

	interactionTransform = CreateDefaultSubobject<USceneComponent>("Interaction Location");
	interactionTransform->SetupAttachment(root);

	staticMesh = CreateDefaultSubobject<UStaticMeshComponent>("Static Mesh");
	staticMesh->SetupAttachment(root);

	skeletalMesh = CreateDefaultSubobject<USkeletalMeshComponent>("Skeletal Mesh");
	skeletalMesh->SetupAttachment(root);

	boxCollison = CreateDefaultSubobject<UBoxComponent>("Box Collision");
	boxCollison->SetupAttachment(root);
}

// Called when the game starts or when spawned
void AActorInteracionBase::BeginPlay()
{
	Super::BeginPlay();

	FOnTimelineFloat moveToComponentProgressUpdate;
	moveToComponentProgressUpdate.BindUFunction(this, FName("MoveToComponentUpdate"));

	FOnTimelineEvent moveToComponentFinishedEvent;
	moveToComponentFinishedEvent.BindUFunction(this, FName("MoveToComponentFinished"));

	moveToComponentTimeline.AddInterpFloat(moveToComponentCurve, moveToComponentProgressUpdate);
	moveToComponentTimeline.SetTimelineFinishedFunc(moveToComponentFinishedEvent);
}

// Called every frame
void AActorInteracionBase::Tick(float DeltaTime)
{
	Super::Tick(DeltaTime);

	moveToComponentTimeline.TickTimeline(DeltaTime);
}

void AActorInteracionBase::OnInteract(AActor* Player)
{
	if (IsValid(Player))
	{
		playerActor = Player;
		moveToComponentTimeline.PlayFromStart();
		if (itemType == EItemType::EIT_Valve || itemType == EItemType::EIT_WallLever)
		{
			PlayObjectAnimation();
		}
		else if (itemType != EItemType::EIT_Pickable)
		{
			if (!schoolLockerAnimations[0]) { return; }
			PlayOpenableObjectAnimation(schoolLockerAnimations[0]);
		}
	}
}

EItemType AActorInteracionBase::GetItemType()
{
	return itemType;
}

EPickUpHeight AActorInteracionBase::GetPickUpHeight()
{
	return pickUpHeight;
}

UStaticMesh* AActorInteracionBase::GetStaticMesh()
{
	return staticMesh->GetStaticMesh();
}

void AActorInteracionBase::MoveToComponentUpdate(float Alpha)
{
	if (!IsValid(playerActor) || !IsValid(interactionTransform)) { return; }
	
	FVector CurrentLoc = playerActor->GetActorLocation();
	FVector TargetLoc  = interactionTransform->GetComponentLocation();
	FVector NewLoc(
		FMath::Lerp(CurrentLoc.X, TargetLoc.X, Alpha),
		FMath::Lerp(CurrentLoc.Y, TargetLoc.Y, Alpha),
		CurrentLoc.Z
	);
	playerActor->SetActorLocation(NewLoc);
	
	float CurrentYaw = GetWorld()->GetFirstPlayerController()->GetControlRotation().Yaw;
	float TargetYaw  = interactionTransform->GetComponentRotation().Yaw;
	
	float DeltaYaw = FMath::FindDeltaAngleDegrees(CurrentYaw, TargetYaw);
	
	float NewYaw = CurrentYaw + DeltaYaw * Alpha;
	NewYaw = FMath::Fmod(NewYaw + 360.0f, 360.0f);

	GetWorld()->GetFirstPlayerController()->SetControlRotation(FRotator(0.f, NewYaw, 0.f));
}

void AActorInteracionBase::MoveToComponentFinished()
{
	
}

