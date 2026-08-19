// Copyright Automatic Games sp. z o.o. All Rights Reserved

#pragma once

#include "CoreMinimal.h"
#include "UObject/Interface.h"
#include "InteractionIntf.generated.h"

UENUM()
enum class EItemType : uint8
{
	EIT_Valve UMETA(DisplayName = "Valve Item"),
	EIT_WallLever UMETA(DisplayName = "Wall Lever Item"),
	EIT_SchoolLocker UMETA(DisplayName = "School Locker Item"),
	EIT_SmallUpperSchoolLocker UMETA(DisplayName = "Small Upper School Locker Item"),
	EIT_Drawer100cm UMETA(DisplayName = "Drawer 100 cm"),
	EIT_Pickable UMETA(DisplayName = "Pickable Item")
};

UENUM()
enum class EPickUpHeight : uint8
{
	EPH_Floor,
	EPH_Table,
	EPH_Top
};

UINTERFACE()
class UInteractionIntf : public UInterface
{
	GENERATED_BODY()
};

class PROJECTDYNASTY_API IInteractionIntf
{
	GENERATED_BODY()

public:

	virtual void OnInteract(AActor* Player) = 0;
	virtual EItemType GetItemType() = 0;
	virtual EPickUpHeight GetPickUpHeight() = 0;
	virtual UStaticMesh* GetStaticMesh() = 0;
};
