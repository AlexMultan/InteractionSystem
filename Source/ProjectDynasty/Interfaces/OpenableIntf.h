// Copyright Automatic Games sp. z o.o. All Rights Reserved

#pragma once

#include "CoreMinimal.h"
#include "UObject/Interface.h"
#include "OpenableIntf.generated.h"

UENUM()
enum class EDoorState : uint8
{
	EDS_Opened,
	EDS_Closed,
	EDS_Locked
};

// This class does not need to be modified.
UINTERFACE()
class UOpenableIntf : public UInterface
{
	GENERATED_BODY()
};


class PROJECTDYNASTY_API IOpenableIntf
{
	GENERATED_BODY()

	
public:

	virtual void OnOpen(AActor* Player) = 0;
	virtual EDoorState GetDoorState() = 0;
	virtual bool CanInteract(AActor* Player) = 0;
};
