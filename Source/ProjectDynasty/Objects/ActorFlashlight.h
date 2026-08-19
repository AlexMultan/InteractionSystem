// Copyright Automatic Games sp. z o.o. All Rights Reserved

#pragma once

#include "CoreMinimal.h"
#include "GameFramework/Actor.h"
#include "ActorFlashlight.generated.h"

class USpotLightComponent;

UCLASS()
class PROJECTDYNASTY_API AActorFlashlight : public AActor
{
	GENERATED_BODY()

	UPROPERTY(VisibleAnywhere, Category = "Default Settings")
	USceneComponent* root;

	UPROPERTY(VisibleAnywhere, Category = "Default Settings")
	UStaticMeshComponent* flashlightMesh;

	UPROPERTY(VisibleAnywhere, Category = "Default Settings")
	USpotLightComponent* spotLight;

	UPROPERTY(VisibleAnywhere, Category = "Default Settings")
	float flashlightIntensity = 4000.f;

	UPROPERTY(VisibleAnywhere, Category = "Default Settings")
	float currentBatteryLevel = 100.f;

	float MAX_BATTERY_LEVEL = 100.f;

	UPROPERTY(EditDefaultsOnly, Category = "Default Settings")
	TObjectPtr<UMaterialInterface> flashlightFlickeringMaterial;

public:
	AActorFlashlight();

protected:
	virtual void BeginPlay() override;

	UPROPERTY(EditDefaultsOnly, Category = "Default Settings")
	float minBatteryConsumption = 0.8f;

	UPROPERTY(EditDefaultsOnly, Category = "Default Settings")
	float maxBatteryConsumption = 1.f;

public:
	virtual void Tick(float DeltaTime) override;

	UFUNCTION(BlueprintCallable)
	void ToggleFlashlight(bool Toggle);
	void BatteryDrain(bool IsFlashlightEquipped);

	UFUNCTION(BlueprintCallable)
	float GetCurrentBatteryLevel() { return currentBatteryLevel; }

	UFUNCTION(BlueprintCallable, BlueprintPure)
	USpotLightComponent* GetSpotLight() { return spotLight; }
};
