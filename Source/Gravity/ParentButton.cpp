// Fill out your copyright notice in the Description page of Project Settings.


#include "ParentButton.h"
#include "GravityGameMode.h"
#include "MyCharacter.h"
#include "Components/TimelineComponent.h"
#include "Containers/UnrealString.h"
#include "Net/UnrealNetwork.h"
#include "Kismet/KismetMathLibrary.h"
#include "Kismet/GameplayStatics.h"
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

	if (HasAuthority())
	{
		CollisionPart->OnComponentBeginOverlap.AddDynamic(this, &AParentButton::OnOverlapBegin);
		CollisionPart->OnComponentEndOverlap.AddDynamic(this, &AParentButton::OnOverlapEnd);
	}

	bReadyState = true;
}

void AParentButton::GetLifetimeReplicatedProps(TArray<FLifetimeProperty>& OutLifetimeProps) const
{
	Super::GetLifetimeReplicatedProps(OutLifetimeProps);

	DOREPLIFETIME(AParentButton, bReadyState);
}

void AParentButton::InitializeCF_PressButton()
{
	if (CF_PressButton)
	{
		FOnTimelineFloat TimelineProgress;
		TimelineProgress.BindUFunction(this, FName("TimelineProgress"));
		CurveTimeline.AddInterpFloat(CF_PressButton, TimelineProgress);

		FOnTimelineEventStatic FinishTimeline;
		FinishTimeline.BindUFunction(this, FName("TimelineFinish"));
		CurveTimeline.SetTimelineFinishedFunc(FinishTimeline);
	}
}

void AParentButton::OnRep_ButtonReaction()
{
	bReadyState = false;
	CurveTimeline.PlayFromStart();
}

// Called when the game starts or when spawned
void AParentButton::BeginPlay()
{
	Super::BeginPlay();

	auto richCurve = new FRichCurve();
	auto key = richCurve->AddKey(0.0f, 75.0f);
	richCurve->AddKey(0.2f, 25.0f);
	richCurve->AddKey(1.0f, 75.f);
	richCurve->SetKeyInterpMode(key, RCIM_Linear);

	CF_PressButton = NewObject<UCurveFloat>();

	CF_PressButton->FloatCurve = *richCurve;

	InitializeCF_PressButton();
}

void AParentButton::OnOverlapBegin(UPrimitiveComponent* OverlappedComp, AActor* OtherActor, UPrimitiveComponent* OtherComp,
	int32 OtherBodyIndex, bool bFromSweep, const FHitResult& SweepResult)
{	
	AMyCharacter* Player = Cast<AMyCharacter>(OtherActor);
	if ((OtherActor && (OtherActor != this->GetOwner()) && OtherComp && Player))
	{
		Player->SetCurrentButton(this);
	}
}

void AParentButton::OnOverlapEnd(UPrimitiveComponent* OverlappedComp, AActor* OtherActor, UPrimitiveComponent* OtherComp, int32 OtherBodyIndex)
{	
	AMyCharacter* Player = Cast<AMyCharacter>(OtherActor);
	if ((OtherActor && (OtherActor != this->GetOwner()) && OtherComp && Player))
	{
		Player->SetCurrentButton(nullptr);
	}
}

void AParentButton::TryPressButton()
{
	if (HasAuthority())
	{
		if (bReadyState)
		{
			OnRep_ButtonReaction();
		}
	}
	else
	{
		Server_TryPressButton();
	}
}

bool AParentButton::Server_TryPressButton_Validate()
{
	return true;
}

void AParentButton::Server_TryPressButton_Implementation()
{
	TryPressButton();
}

void AParentButton::TimelineFinish()
{	
	bReadyState = true;
}

// Called every frame
void AParentButton::Tick(float DeltaTime)
{
	Super::Tick(DeltaTime);

	CurveTimeline.TickTimeline(DeltaTime);
}

void AParentButton::TimelineProgress()
{
	TimelineValue = CurveTimeline.GetPlaybackPosition();

	MovePart->SetRelativeLocation(FVector(0.0f, CF_PressButton->GetFloatValue(TimelineValue), 0.0f));
}

bool AParentButton::Get_ReadyStat()
{
	return(bReadyState);
}

void AParentButton::Set_ReadyStat(bool input)
{
	bReadyState = input;
}
