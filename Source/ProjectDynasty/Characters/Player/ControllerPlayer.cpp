// Copyright Automatic Games sp. z o.o. All Rights Reserved


#include "ControllerPlayer.h"
#include "Engine/LocalPlayer.h"
#include "EnhancedInputSubsystems.h"
#include "Kismet/GameplayStatics.h"
#include "InputMappingContext.h"

AControllerPlayer::AControllerPlayer()
{

}

void AControllerPlayer::SetupInputComponent()
{
	Super::SetupInputComponent();

	if (IsLocalPlayerController())
	{
		if (UEnhancedInputLocalPlayerSubsystem* _subsystem = ULocalPlayer::GetSubsystem<UEnhancedInputLocalPlayerSubsystem>(GetLocalPlayer()))
		{
			for (UInputMappingContext* _currentContext : mappingContexts)
			{
				_subsystem->AddMappingContext(_currentContext, 0);
			}
		}
	}
}
