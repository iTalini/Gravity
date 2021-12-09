// Copyright Epic Games, Inc. All Rights Reserved.


#include "GravityGameMode.h"
#include "Kismet/GameplayStatics.h"
#include "GravityCharacter.h"
#include "ParentButton.h"
#include "SpawnActor.h"
#include "MyHUD.h"
#include "UObject/ConstructorHelpers.h"

AGravityGameMode::AGravityGameMode() : Super()
{
	HUDClass = AMyHUD::StaticClass();

	// set default pawn class to our Blueprinted character
	static ConstructorHelpers::FClassFinder<APawn> PlayerPawnBPClass(TEXT("/Game/ThirdPersonCPP/Blueprints/ThirdPersonCharacter"));
	score = 0;
	if (PlayerPawnBPClass.Class != NULL)
	{
		DefaultPawnClass = PlayerPawnBPClass.Class;
	}
	last_spawn = -1;
}

void AGravityGameMode::OnSphereDestroyed(AScoreSpheres* A_Sphere)
{
	amount_spheres--;
	if (amount_spheres == 0)
	{
		AMyHUD* InGameHUD = Cast<AMyHUD>(GetWorld()->GetFirstPlayerController()->GetHUD());
		if (InGameHUD)
			InGameHUD->NewMessage(FString("You end the game!!!"), 5.0f, FLinearColor::Yellow);
		GetWorld()->GetTimerManager().SetTimer(MessageKillTimerHandle, this, &AGravityGameMode::EndGame, 5.5f, true);
	}
	else
		SpawnOdject();
}

int AGravityGameMode::Get_Score()
{
	return score;
}

void AGravityGameMode::EndGame()
{
	//FGenericPlatformMisc::RequestExit(false);

	GetWorld()->GetFirstPlayerController()->ConsoleCommand("quit");
}

void AGravityGameMode::GameStart()
{
	AMyHUD* InGameHUD = Cast<AMyHUD>(GetWorld()->GetFirstPlayerController()->GetHUD());
	if (InGameHUD)
		InGameHUD->NewMessage(FString("Game start in..."), 1.0f, FLinearColor::Red);
	GetWorld()->GetTimerManager().SetTimer(MessageKillTimerHandle, this, &AGravityGameMode::Three, 1.0f, false);
}

void AGravityGameMode::Three()
{
	AMyHUD* InGameHUD = Cast<AMyHUD>(GetWorld()->GetFirstPlayerController()->GetHUD());
	if (InGameHUD)
		InGameHUD->NewMessage(FString("3"), 1.0f, FLinearColor::Red);
	GetWorld()->GetTimerManager().SetTimer(MessageKillTimerHandle, this, &AGravityGameMode::Two, 1.0f, false);
}

void AGravityGameMode::Two()
{
	AMyHUD* InGameHUD = Cast<AMyHUD>(GetWorld()->GetFirstPlayerController()->GetHUD());
	if (InGameHUD)
		InGameHUD->NewMessage(FString("2"), 1.0f, FLinearColor::Red);
	GetWorld()->GetTimerManager().SetTimer(MessageKillTimerHandle, this, &AGravityGameMode::One, 1.0f, false);
}

void AGravityGameMode::One()
{
	AMyHUD* InGameHUD = Cast<AMyHUD>(GetWorld()->GetFirstPlayerController()->GetHUD());
	if (InGameHUD)
		InGameHUD->NewMessage(FString("1"), 1.0f, FLinearColor::Red);
	GetWorld()->GetTimerManager().SetTimer(MessageKillTimerHandle, this, &AGravityGameMode::Go, 1.0f, false);
}

void AGravityGameMode::Go()
{
	AMyHUD* InGameHUD = Cast<AMyHUD>(GetWorld()->GetFirstPlayerController()->GetHUD());
	if (InGameHUD)
		InGameHUD->NewMessage(FString("GO!!!"), 1.0f, FLinearColor::Green);
	SpawnOdject();
}

void AGravityGameMode::SpawnOdject()
{
	if (amount_spheres != 0)
	{
		TArray <AActor*> TA_Spawners;

		UGameplayStatics::GetAllActorsOfClass(this, ASpawnActor::StaticClass(), TA_Spawners);

		int amount_spawner = TA_Spawners.Num() - 1;
		if (amount_spawner > 0)
		{
			amount_spawner = FMath::RandRange(0, amount_spawner);
			while (amount_spawner == last_spawn)
				amount_spawner = FMath::RandRange(0, amount_spawner);

			last_spawn = amount_spawner;
		}

		if (TA_Spawners.IsValidIndex(amount_spawner))
		{
			ASpawnActor* Spawner = Cast<ASpawnActor>(TA_Spawners[amount_spawner]);

			FActorSpawnParameters SpawnParam;
			if (Spawner)
			{
				AActor* SpawnedActorRef = GetWorld()->SpawnActor<AActor>(ActorToSpawn, Spawner->GetActorLocation(), Spawner->GetActorRotation(), SpawnParam);
				TArray <AActor*> TA_Spheres;

				UGameplayStatics::GetAllActorsOfClass(this, AScoreSpheres::StaticClass(), TA_Spheres);
				for (AActor* A_Sphere : TA_Spheres)
				{
					AScoreSpheres* ScoreSphere = Cast<AScoreSpheres>(A_Sphere);
					if (ScoreSphere)
					{
						ScoreSphere->SphereDisapear.AddUFunction(this, "OnSphereDestroyed", ScoreSphere);
					}
				}
			}
		}
	}
}

void AGravityGameMode::BeginPlay()
{
	Super::BeginPlay();

	//GameStart();
	Go();
}
