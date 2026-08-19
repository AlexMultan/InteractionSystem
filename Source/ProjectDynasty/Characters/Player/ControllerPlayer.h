// Copyright Automatic Games sp. z o.o. All Rights Reserved

#pragma once

#include "CoreMinimal.h"
#include "GameFramework/PlayerController.h"
#include "ControllerPlayer.generated.h"

class UInputMappingContext;

UCLASS()
class PROJECTDYNASTY_API AControllerPlayer : public APlayerController
{
	GENERATED_BODY()

	AControllerPlayer();

protected:
	
	UPROPERTY(EditAnywhere, Category="Input Mappings")
	TArray<UInputMappingContext*> mappingContexts;

	virtual void SetupInputComponent() override;
};
