// Fill out your copyright notice in the Description page of Project Settings.


#include "ParentButton.h"
#include "GravityGameMode.h"
#include "Kismet/GameplayStatics.h"
#include "Components/TimelineComponent.h"
#include "Containers/UnrealString.h"
#include "Runtime/CoreUObject/Public/UObject/ConstructorHelpers.h"

// Sets default values
AParentButton::AParentButton()
{
 	// Set this actor to call Tick() every frame.  You can turn this off to improve performance if you don't need it.
	PrimaryActorTick.bCanEverTick = true;


	ButtonBody = CreateDefaultSubobject<UStaticMeshComponent>("RootButton");
	RootComponent = ButtonBody;

	MovePart = CreateDefaultSubobject<UStaticMeshComponent>("PressButton");
	MovePart->SetupAttachment(ButtonBody);

	CollisionPart = CreateDefaultSubobject<UStaticMeshComponent>("CollisCube");
	CollisionPart->SetupAttachment(ButtonBody);

	static ConstructorHelpers::FObjectFinder<UStaticMesh>MeshToUse(TEXT("/Game/Geometry/Meshes/1M_Cube.1M_Cube"));
	if (MeshToUse.Succeeded())
	{
		ButtonBody->SetStaticMesh(MeshToUse.Object);
		ButtonBody->SetWorldScale3D(FVector(1.0f, 0.2f, 1.0f));

		MovePart->SetStaticMesh(MeshToUse.Object);
		MovePart->SetWorldScale3D(FVector(0.6f, 1.2f, 0.6f));
		MovePart->SetRelativeLocation(FVector(0.0f, 75.0f, 0.0f));

		CollisionPart->SetStaticMesh(MeshToUse.Object);
		CollisionPart->SetWorldScale3D(FVector(1.0f, 5.0f, 1.0f));
		CollisionPart->SetRelativeLocation(FVector(0.0f, 0.0f, 0.0f));
		CollisionPart->SetHiddenInGame(true);
	}

	CF_PressButton = CreateDefaultSubobject<UCurveFloat>(TEXT("/Game/C_ChangeButton.C_ChangeButton"));
}

// Called when the game starts or when spawned
void AParentButton::BeginPlay()
{
	Super::BeginPlay();

	CanPress = false;

	Player = Cast<AMyCharacter>(UGameplayStatics::GetPlayerPawn(this, 0));

	CollisionPart->OnComponentBeginOverlap.AddDynamic(this, &AParentButton::OnOverlapBegin);
	CollisionPart->OnComponentEndOverlap.AddDynamic(this, &AParentButton::OnOverlapEnd);

	if (CF_PressButton)
	{
		FOnTimelineFloat TimelineProgress;
		TimelineProgress.BindUFunction(this, FName("TimelineProgress"));
		CurveTimeline.AddInterpFloat(CF_PressButton, TimelineProgress);
		CurveTimeline.SetLooping(true);

		FOnTimelineEventStatic FinishTimeline;
		FinishTimeline.BindUFunction(this, FName("TimelineFinish"));
		CurveTimeline.SetTimelineFinishedFunc(FinishTimeline);
	}
}

void AParentButton::OnOverlapBegin(UPrimitiveComponent* OverlappedComp, AActor* OtherActor, UPrimitiveComponent* OtherComp,
	int32 OtherBodyIndex, bool bFromSweep, const FHitResult& SweepResult)//Не меняется
{	
	if (Player)
	{
		CanPress = true;
		Player->Set_ReadyPress(true);
	}
}

void AParentButton::OnOverlapEnd(UPrimitiveComponent* OverlappedComp, AActor* OtherActor, UPrimitiveComponent* OtherComp, int32 OtherBodyIndex)
{	
	if (Player)
	{
		Player->Set_ReadyPress(false);
	}
}

void AParentButton::Shock()//Не меняется
{
	if (Player)
		Player->Set_WasPress(false);
	CurveTimeline.PlayFromStart();
	CanPress = false;
}

void AParentButton::TimelineFinish()//Не меняется
{	
	if (Player)
		Player->Set_WasPress(false);
}

// Called every frame
void AParentButton::Tick(float DeltaTime)
{
	Super::Tick(DeltaTime);
}

void AParentButton::TimelineProgress(float input)//Не меняется
{
	//UE_LOG(LogTemp, Warning, TEXT("TimelineProgress"));

	FVector output = FMath::Lerp(FVector(0.0f, 75.0f, 0.0f), FVector(0.0f, 25.0f, 0.0f), input);
	MovePart->SetRelativeLocation(output);
}

bool AParentButton::Get_CanPress()//Не меняется
{
	return(CanPress);
}

void AParentButton::Set_CanPress(bool input)//Не меняется
{
	CanPress = input;
}
