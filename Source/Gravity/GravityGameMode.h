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
		void Add_Score(int input);
	UFUNCTION(BlueprintCallable)
		void EndGame();

	UFUNCTION(BlueprintCallable)
		void GameStart();
	UFUNCTION(BlueprintCallable)
		void Three();
	UFUNCTION(BlueprintCallable)
		void Two();
	UFUNCTION(BlueprintCallable)
		void One();
	UFUNCTION(BlueprintCallable)
		void Go();

	FTimerHandle MessageKillTimerHandle;

	UPROPERTY(EditAnywhere, Category = ScoreSpheres)
		int amount_spheres;

	//Spawner
	UPROPERTY(EditDefaultsOnly, Category = "Spawning")
		TSubclassOf<AActor>ActorToSpawn;

protected:
	virtual void BeginPlay() override;

	UFUNCTION()
		void SpawnOdject();

	int score;

	int last_spawn;
};



