// Copyright Automatic Games sp. z o.o. All Rights Reserved


#include "ActorFlashlight.h"

#include "Components/SpotLightComponent.h"


// Sets default values
AActorFlashlight::AActorFlashlight()
{
	// Set this actor to call Tick() every frame.  You can turn this off to improve performance if you don't need it.
	PrimaryActorTick.bCanEverTick = true;

	root = CreateDefaultSubobject<USceneComponent>(TEXT("Root"));
	root->SetupAttachment(GetRootComponent());

	flashlightMesh = CreateDefaultSubobject<UStaticMeshComponent>(TEXT("Flashlight"));
	flashlightMesh->SetupAttachment(root);

	spotLight = CreateDefaultSubobject<USpotLightComponent>(TEXT("Spot Light"));
	spotLight->SetupAttachment(flashlightMesh);
	spotLight->Intensity = flashlightIntensity;
	spotLight->SetUseTemperature(true);
	spotLight->Temperature = 5500.f;
}

// Called when the game starts or when spawned
void AActorFlashlight::BeginPlay()
{
	Super::BeginPlay();
	
}

// Called every frame
void AActorFlashlight::Tick(float DeltaTime)
{
	Super::Tick(DeltaTime);
	
}

void AActorFlashlight::ToggleFlashlight(bool Toggle)
{
	spotLight->SetIntensity(Toggle ? flashlightIntensity : 0.f);
}

void AActorFlashlight::BatteryDrain(bool IsFlashlightEquipped)
{
	currentBatteryLevel = FMath::Clamp(currentBatteryLevel, 0.f, MAX_BATTERY_LEVEL);

	if (currentBatteryLevel > 0.f)
	{
		if (IsFlashlightEquipped)
		{
			float _batteryConspumption = FMath::RandRange(minBatteryConsumption, maxBatteryConsumption);
			currentBatteryLevel -= _batteryConspumption * GetWorld()->GetDeltaSeconds();
		}
	}

	if (IsFlashlightEquipped)
	{
		if (currentBatteryLevel <= 0.f)
		{
			spotLight->SetIntensity(1000.f);
		}
		else
		{
			spotLight->SetIntensity(flashlightIntensity);
		}
	}
}

