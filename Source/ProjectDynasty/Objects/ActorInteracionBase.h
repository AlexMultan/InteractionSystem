// Copyright Automatic Games sp. z o.o. All Rights Reserved

#pragma once

#include "CoreMinimal.h"
#include "Components/BoxComponent.h"
#include "GameFramework/Actor.h"
#include "Components/TimelineComponent.h"
#include "ProjectDynasty/Interfaces/InteractionIntf.h"
#include "ActorInteracionBase.generated.h"

UCLASS()
class PROJECTDYNASTY_API AActorInteracionBase : public AActor, public IInteractionIntf
{
	GENERATED_BODY()

public:
	// Sets default values for this actor's properties
	AActorInteracionBase();

protected:
	// Called when the game starts or when spawned
	virtual void BeginPlay() override;

	UPROPERTY(VisibleAnywhere, BlueprintReadOnly)
	USceneComponent* root;

	UPROPERTY(EditAnywhere, BlueprintReadWrite)
	USceneComponent* interactionTransform;

	UPROPERTY(EditDefaultsOnly, BlueprintReadWrite)
	UStaticMeshComponent* staticMesh;

	UPROPERTY(EditDefaultsOnly, BlueprintReadWrite)
	USkeletalMeshComponent* skeletalMesh;

	UPROPERTY(EditDefaultsOnly, BlueprintReadWrite)
	UBoxComponent* boxCollison;

	UPROPERTY(EditDefaultsOnly)
	UCurveFloat* moveToComponentCurve;

	UPROPERTY(EditDefaultsOnly)
	TArray<UAnimationAsset*> schoolLockerAnimations;

public:
	// Called every frame
	virtual void Tick(float DeltaTime) override;

	virtual auto OnInteract(AActor* Player) -> void override;
	virtual EItemType GetItemType() override;
	virtual EPickUpHeight GetPickUpHeight() override;
	virtual UStaticMesh* GetStaticMesh() override;

	UFUNCTION(BlueprintImplementableEvent)
	void PlayObjectAnimation();

	UFUNCTION(BlueprintImplementableEvent)
	void PlayOpenableObjectAnimation(UAnimationAsset* Animation);

	UPROPERTY(EditAnywhere, BlueprintReadWrite)
	EItemType itemType;

	UPROPERTY(EditAnywhere, BlueprintReadWrite)
	EPickUpHeight pickUpHeight;

private:

	FTimeline moveToComponentTimeline;

	UFUNCTION()
	void MoveToComponentUpdate(float Alpha);

	UFUNCTION()
	void MoveToComponentFinished();

	AActor* playerActor = nullptr;
};
