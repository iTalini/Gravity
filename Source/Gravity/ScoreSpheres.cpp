// Fill out your copyright notice in the Description page of Project Settings.


#include "ScoreSpheres.h"
#include "Materials/MaterialInstanceDynamic.h"
#include "TimerManager.h"
#include "GravityGameMode.h"
#include "MyCharacter.h"
#include "Runtime/CoreUObject/Public/UObject/ConstructorHelpers.h"

// Sets default values
AScoreSpheres::AScoreSpheres()
{
 	// Set this actor to call Tick() every frame.  You can turn this off to improve performance if you don't need it.
	PrimaryActorTick.bCanEverTick = true;

	Body = CreateDefaultSubobject<UStaticMeshComponent>("SphereBody");
	RootComponent = Body;

	static ConstructorHelpers::FObjectFinder<UStaticMesh>MeshToUse(TEXT("/Game/StarterContent/Shapes/Shape_Sphere.Shape_Sphere"));

	if (MeshToUse.Succeeded())
		Body->SetStaticMesh(MeshToUse.Object);

	StatMaterial = CreateDefaultSubobject<UMaterial>(TEXT("StaticMaterial"));
	DynMat = CreateDefaultSubobject<UMaterialInstanceDynamic>(TEXT("DynamicMaterial"));
	static ConstructorHelpers::FObjectFinder<UMaterial>MatToUse(TEXT("/Game/MyMaterial/M_ScoreSphere.M_ScoreSphere"));
	if (MatToUse.Succeeded())
	{
		StatMaterial = MatToUse.Object;
	}

	if (GetLocalRole() == ROLE_Authority)
		Body->OnComponentHit.AddDynamic(this, &AScoreSpheres::OnComponentHit);
	
	changeMat = false;
	for_changeMat = 0.0f;
	amount_sphere = 1000;

	time_for_maxscore = 20.0f;

	//Replication
	SetRemoteRoleForBackwardsCompat(ROLE_SimulatedProxy);
	bReplicates = true;
	SetReplicatingMovement(true);

	hit = false;
	//bReplicateMovement = true;
}

void AScoreSpheres::BeginPlay()
{
	Super::BeginPlay();

	if (StatMaterial)
		DynMat = UMaterialInstanceDynamic::Create(StatMaterial, nullptr);
	Body->SetMaterial(0, DynMat);

	FTimerDelegate TimerDel;
	FTimerHandle TimerHandle;

	TimerDel.BindUFunction(this, FName("ChangeMaterial"));
	GetWorldTimerManager().SetTimer(TimerHandle, TimerDel, time_for_maxscore, false);
}

void AScoreSpheres::OnComponentHit(UPrimitiveComponent* HitComponent, AActor* OtherActor, UPrimitiveComponent* OtherComp, FVector NormalImpulse, const FHitResult& Hit)
{
	if (hit == false)
	{
		hit = true;
		if (SphereDisapear.IsBound())
			SphereDisapear.Broadcast();

		AMyCharacter* HitPerson = Cast<AMyCharacter>(OtherActor);

		HitPerson->AddToScore(amount_sphere);
		//AGravityGameMode* const MyMode = GetWorld()->GetAuthGameMode<AGravityGameMode>();
		//MyMode->OnSphereDestroyed(this);
		Destroy();

	}
}

void AScoreSpheres::ChangeMaterial()
{
	changeMat = true;
}

void AScoreSpheres::Tick(float DeltaTime)
{
	Super::Tick(DeltaTime);
	if (changeMat)
	{
		DynMat->SetVectorParameterValue("EmissiveColor", FLinearColor::LerpUsingHSV(FColor::FromHex("BDA800FF"), FColor::FromHex("FF0002FF"), for_changeMat));
		for_changeMat += 0.001f;
		amount_sphere = (int)(amount_sphere - 1);
		if (for_changeMat >= 0.998f)
			changeMat = false;
	}
}

int AScoreSpheres::Get_amountSphere()
{
	return amount_sphere;
}

