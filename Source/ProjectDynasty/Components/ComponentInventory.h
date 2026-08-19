// Copyright Automatic Games sp. z o.o. All Rights Reserved

#pragma once

#include "CoreMinimal.h"
#include "Components/ActorComponent.h"
#include "ProjectDynasty/Structs/StructItemInfo.h"
#include "ComponentInventory.generated.h"


UCLASS(ClassGroup=(Custom), meta=(BlueprintSpawnableComponent))
class PROJECTDYNASTY_API UComponentInventory : public UActorComponent
{
	GENERATED_BODY()

	UPROPERTY(EditDefaultsOnly)
	int32 MAX_SLOTS = 4;

public:
	// Sets default values for this component's properties
	UComponentInventory();

	UPROPERTY(EditAnywhere, BlueprintReadWrite)
	int32 currentSlotIndex = 0;

	UPROPERTY(EditAnywhere)
	TArray<FItemData> inventory;

protected:
	// Called when the game starts
	virtual void BeginPlay() override;

public:
	// Called every frame
	virtual void TickComponent(float DeltaTime, ELevelTick TickType,
	                           FActorComponentTickFunction* ThisTickFunction) override;

	UFUNCTION()
	void SelectItem(int32 ItemIndex);

	UFUNCTION()
	void ChangeCurrentSlot(float InputValue);
};
