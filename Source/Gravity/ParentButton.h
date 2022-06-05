// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "MyCharacter.h"
#include "Engine/EngineTypes.h"
#include "Components/StaticMeshComponent.h"
#include "Components/PrimitiveComponent.h"
#include "Curves/CurveFloat.h"
#include "ParentButton.generated.h"

class AMyCharacter;

UCLASS()
class GRAVITY_API AParentButton : public AActor
{
	GENERATED_BODY()
	
public:	
	// Sets default values for this actor's properties
	AParentButton();

	virtual void GetLifetimeReplicatedProps(TArray< FLifetimeProperty >& OutLifetimeProps) const;

	UPROPERTY(VisibleDefaultsOnly, Category = ButtonPart)
	UStaticMeshComponent* ButtonBody;

	UPROPERTY(VisibleDefaultsOnly, Category = ButtonPart)
	UStaticMeshComponent* MovePart;

	UPROPERTY(VisibleDefaultsOnly, Category = ButtonPart)
	UStaticMeshComponent* CollisionPart;

	UFUNCTION(BlueprintCallable)
	void InitializeCF_PressButton();

	UFUNCTION()
	void OnRep_ButtonReaction();

	UFUNCTION()
	void TryPressButton();
	

protected:
	FTimeline CurveTimeline;

	UPROPERTY(VisibleDefaultsOnly, Category = Timeline)
	UCurveFloat* CF_PressButton;

	float PressValue;

	float CurveFloatValue;

	float TimelineValue;


	UPROPERTY(ReplicatedUsing = OnRep_ButtonReaction)
	bool bReadyState;

	UFUNCTION(Server, Reliable, WithValidation)
	void Server_TryPressButton();

public:
	// Called when the game starts or when spawned
	virtual void BeginPlay() override;

	UFUNCTION()
	virtual void OnOverlapBegin(UPrimitiveComponent* OverlappedComp, AActor* OtherActor, UPrimitiveComponent* OtherComp,
			int32 OtherBodyIndex, bool bFromSweep, const FHitResult& SweepResult);

	UFUNCTION()
	virtual void OnOverlapEnd(UPrimitiveComponent* OverlappedComp, AActor* OtherActor, UPrimitiveComponent* OtherComp,
			int32 OtherBodyIndex);

	// Called every frame
	virtual void Tick(float DeltaTime) override;

	UFUNCTION(BlueprintCallable)
	void TimelineProgress();
	UFUNCTION(BlueprintCallable)
	void TimelineFinish();

	UFUNCTION(BlueprintCallable)
	bool Get_ReadyStat();
	UFUNCTION(BlueprintCallable)
	void Set_ReadyStat(bool input);
};
