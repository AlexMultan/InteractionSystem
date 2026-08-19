// Copyright Automatic Games sp. z o.o. All Rights Reserved

#pragma once

#include "CoreMinimal.h"
#include "Components/TimelineComponent.h"
#include "Components/BoxComponent.h"
#include "GameFramework/Actor.h"
#include "ProjectDynasty/Interfaces/OpenableIntf.h"
#include "ActorDoorBase.generated.h"

UCLASS()
class PROJECTDYNASTY_API AActorDoorBase : public AActor, public IOpenableIntf
{
	GENERATED_BODY()

protected:

	UPROPERTY(VisibleAnywhere, BlueprintReadOnly)
	USceneComponent* root;

	UPROPERTY(EditDefaultsOnly, BlueprintReadWrite)
	UStaticMeshComponent* staticMesh;

	UPROPERTY(EditDefaultsOnly, BlueprintReadWrite)
	UBoxComponent* boxCollison;

	UPROPERTY(EditDefaultsOnly, BlueprintReadWrite)
	UBoxComponent* boxCollison1;

	UPROPERTY(EditDefaultsOnly)
	UCurveFloat* openDoorCurve;

	UPROPERTY(EditDefaultsOnly)
	UCurveFloat* closeDoorCurve;

	UPROPERTY(EditDefaultsOnly)
	USoundBase* openDoorSound;

	UPROPERTY(EditDefaultsOnly)
	USoundBase* closeDoorSound;

public:
	// Sets default values for this actor's properties
	AActorDoorBase();

	UPROPERTY(EditAnywhere, BlueprintReadWrite)
	EDoorState doorState = EDoorState::EDS_Closed;

protected:
	// Called when the game starts or when spawned
	virtual void BeginPlay() override;

public:
	// Called every frame
	virtual void Tick(float DeltaTime) override;

	virtual void OnOpen(AActor* Player) override;
	virtual EDoorState GetDoorState() override;
	virtual bool CanInteract(AActor* Player) override;

	UPROPERTY(VisibleAnywhere, BlueprintReadWrite)
	bool enableInteraction = true;

	UPROPERTY(EditAnywhere, BlueprintReadWrite)
	USceneComponent* interactionTransform;

	UPROPERTY(EditAnywhere, BlueprintReadWrite)
	USceneComponent* interactionTransform1;

	UPROPERTY(EditDefaultsOnly)
	UCurveFloat* moveToComponentCurve;

private:

	FTimeline openDoorTimeline;
	UFUNCTION()
	void OpenDoorUpdate(float Alpha);
	
	UFUNCTION()
	void OpenDoorFinished();

	FTimeline closeDoorTimeline;
	UFUNCTION()
	void CloseDoorUpdate(float Alpha);

	UFUNCTION()
	void CloseDoorFinished();

	UFUNCTION()
	void DoorAnimation(float Alpha, float CurrentYawRotation, float TargetYawRotation) const;

	float openedDoorYawRotation = 105.f;
	float openedDoorYawRotationNegative = -105.f;
	bool bOpenFromFront = true;

	AActor* playerActor = nullptr;

	FTimeline moveToComponentTimeline;

	UFUNCTION()
	void MoveToComponentUpdate(float Alpha);

	UFUNCTION()
	void MoveToComponentFinished();
	
};
