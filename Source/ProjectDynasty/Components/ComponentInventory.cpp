// Copyright Automatic Games sp. z o.o. All Rights Reserved


#include "ComponentInventory.h"


// Sets default values for this component's properties
UComponentInventory::UComponentInventory()
{
	// Set this component to be initialized when the game starts, and to be ticked every frame.  You can turn these features
	// off to improve performance if you don't need them.
	PrimaryComponentTick.bCanEverTick = true;
	
}


// Called when the game starts
void UComponentInventory::BeginPlay()
{
	Super::BeginPlay();
	
}


// Called every frame
void UComponentInventory::TickComponent(float DeltaTime, ELevelTick TickType,
                                        FActorComponentTickFunction* ThisTickFunction)
{
	Super::TickComponent(DeltaTime, TickType, ThisTickFunction);

	currentSlotIndex = FMath::Clamp(currentSlotIndex, 0, MAX_SLOTS);
}

void UComponentInventory::SelectItem(int32 ItemIndex)
{
	if (ItemIndex >= 0 && ItemIndex <= MAX_SLOTS)
	{
		currentSlotIndex = ItemIndex;
	}
}

void UComponentInventory::ChangeCurrentSlot(float InputValue)
{
	if (InputValue == 1.f)
	{
		if (currentSlotIndex == MAX_SLOTS)
		{
			currentSlotIndex = 0;
		}
		else
		{
			currentSlotIndex++;
		}
	}
	else if (InputValue == -1.f)
	{
		if (currentSlotIndex == 0)
		{
			currentSlotIndex = MAX_SLOTS;
		}
		else
		{
			currentSlotIndex--;
		}
	}
}

