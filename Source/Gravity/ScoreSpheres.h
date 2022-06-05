// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "GameFramework/Actor.h"
#include "Components/StaticMeshComponent.h"
#include "ScoreSpheres.generated.h"

DECLARE_EVENT(AScoreSpheres, FSphereDisapear)

UCLASS()
class GRAVITY_API AScoreSpheres : public AActor
{
	GENERATED_BODY()
	
public:	
	// Sets default values for this actor's properties
	AScoreSpheres();

	UPROPERTY(VisibleAnywhere, BlueprintReadWrite, Category = Components)
	UStaticMeshComponent* Body;

	UPROPERTY(VisibleAnywhere, BlueprintReadWrite, Category = Components)
	UMaterial* StatMaterial;

	UPROPERTY(VisibleAnywhere, BlueprintReadWrite, Category = Components)
	UMaterialInstanceDynamic* DynMat;


	bool bChangeMat;

	//by what value the material will change in tick
	float for_changeMat;

	//during this time wouldn't end the character could take max score
	UPROPERTY(EditAnywhere, Category = Time)
	float time_for_maxscore;

protected:
	int amount_sphere;

	bool bHit;

	// Called when the game starts or when spawned
	virtual void BeginPlay() override;

	UFUNCTION()
	void OnComponentHit(UPrimitiveComponent* HitComponent, AActor* OtherActor, UPrimitiveComponent* OtherComp, FVector NormalImpulse, const FHitResult& Hit);

	//function set changeMat = true
	UFUNCTION(BlueprintCallable)
	void ChangeMaterial();

public:	
	// Called every frame
	virtual void Tick(float DeltaTime) override;

	//return amount_sphere
	UFUNCTION(BlueprintCallable)
	int Get_amountSphere();

	//event that we call in the function OnComponentHit
	FSphereDisapear SphereDisapear;

};
