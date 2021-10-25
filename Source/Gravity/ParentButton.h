// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "MyCharacter.h"
#include "Engine/EngineTypes.h"
#include "Components/StaticMeshComponent.h"
#include "Components/PrimitiveComponent.h"
#include "Curves/CurveFloat.h"
#include "ParentButton.generated.h"

UCLASS()
class GRAVITY_API AParentButton : public AActor
{
	GENERATED_BODY()
	
public:	
	// Sets default values for this actor's properties
	AParentButton();

	UPROPERTY(VisibleDefaultsOnly, Category = ButtonPart)
	UStaticMeshComponent* ButtonBody;

	UPROPERTY(VisibleDefaultsOnly, Category = ButtonPart)
	UStaticMeshComponent* MovePart;

	UPROPERTY(VisibleDefaultsOnly, Category = ButtonPart)
	UStaticMeshComponent* CollisionPart;

	//UPROPERTY(VisibleDefaultsOnly, Category = Curvel)
	//UCurveFloat* CF_PressButton;
	

protected:
	// Called when the game starts or when spawned
	virtual void BeginPlay() override;

	UFUNCTION()
	void OnOverlapBegin(UPrimitiveComponent* OverlappedComp, AActor* OtherActor, UPrimitiveComponent* OtherComp,
		int32 OtherBodyIndex, bool bFromSweep, const FHitResult& SweepResult);

	UFUNCTION()
		void OnOverlapEnd(UPrimitiveComponent* OverlappedComp, AActor* OtherActor, UPrimitiveComponent* OtherComp,
			int32 OtherBodyIndex);

	UFUNCTION(BlueprintCallable)//for_jock
		void Shock();

	FTimeline CurveTimeline;
	UPROPERTY(VisibleDefaultsOnly, Category = Timeline)
		UCurveFloat* CF_PressButton;

	UPROPERTY()
		FVector StartLoc;
	UPROPERTY()
		FVector EndLoc;
	//UPROPERTY(VisibleDefaultsOnly, Category = Timeline)
	//	float ZOffset;
	UPROPERTY()
		bool CanPress;

	UPROPERTY()
		AMyCharacter* Player;

public:
	// Called every frame
	virtual void Tick(float DeltaTime) override;

	UFUNCTION(BlueprintCallable)
		void TimelineProgress(float input);
	UFUNCTION(BlueprintCallable)
		void TimelineFinish();

	UFUNCTION(BlueprintCallable)
		bool Get_CanPress();
	UFUNCTION(BlueprintCallable)
		void Set_CanPress(bool input);
};
