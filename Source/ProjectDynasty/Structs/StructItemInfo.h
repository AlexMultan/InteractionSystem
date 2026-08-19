// Copyright Automatic Games sp. z o.o. All Rights Reserved

#pragma once

#include "CoreMinimal.h"
#include "UObject/Object.h"
#include "StructItemInfo.generated.h"

USTRUCT()
struct FItemData
{
	GENERATED_BODY()
	
	UPROPERTY(EditAnywhere)
	int32 itemSlot = 0;

	UPROPERTY(EditAnywhere)
	bool stackable = false;

	UPROPERTY(EditAnywhere)
	int32 maxStackSize = 1;

	UPROPERTY(EditAnywhere)
	bool removable = true;

	UPROPERTY(EditAnywhere)
	UClass* itemClass = nullptr;
	
};

UCLASS()
class PROJECTDYNASTY_API UStructItemInfo : public UObject
{
	GENERATED_BODY()
};
