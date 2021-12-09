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
	// Called when the game starts or when spawned
	virtual void BeginPlay() override;

	UFUNCTION()
		virtual void OnOverlapBegin(UPrimitiveComponent* OverlappedComp, AActor* OtherActor, UPrimitiveComponent* OtherComp,
			int32 OtherBodyIndex, bool bFromSweep, const FHitResult& SweepResult);

	UFUNCTION()
		virtual void OnOverlapEnd(UPrimitiveComponent* OverlappedComp, AActor* OtherActor, UPrimitiveComponent* OtherComp,
			int32 OtherBodyIndex);

	UFUNCTION(BlueprintCallable)//for_jock
		virtual void Shock();
	// Called every frame
	virtual void Tick(float DeltaTime) override;

	UFUNCTION(BlueprintCallable)
		virtual void TimelineProgress(float input);
	UFUNCTION(BlueprintCallable)
		virtual void TimelineFinish();

	UFUNCTION(BlueprintCallable)
		virtual bool Get_CanPress();
	UFUNCTION(BlueprintCallable)
		virtual void Set_CanPress(bool input);
};
