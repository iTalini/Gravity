// Copyright Epic Games, Inc. All Rights Reserved.

#pragma once

#include "CoreMinimal.h"
#include "GameFramework/GameModeBase.h"
#include "ScoreSpheres.h"
#include "GravityGameMode.generated.h"

UCLASS(minimalapi)
class AGravityGameMode : public AGameModeBase
{
	GENERATED_BODY()

public:
	AGravityGameMode();

	UFUNCTION(BlueprintCallable)
	void OnSphereDestroyed(AScoreSpheres* A_Sphere);
	UFUNCTION(BlueprintCallable)
	int Get_Score();

	UFUNCTION(BlueprintCallable)
	void EndGame();

	UFUNCTION()
	void SpawnOdject();

	FTimerHandle MessageKillTimerHandle;

	UPROPERTY(EditAnywhere, Category = ScoreSpheres)
	int amount_spheres;

	UPROPERTY(EditDefaultsOnly, Category = "Spawning")
	TSubclassOf<AActor>ActorToSpawn;

protected:
	virtual void BeginPlay() override;

	int score;

	int last_spawn;
};



