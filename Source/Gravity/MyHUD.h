// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "MyUserWidget.h"
#include "GameFramework/HUD.h"
#include "Components/WidgetComponent.h"
#include "MyHUD.generated.h"

/**
 * 
 */
UCLASS()
class GRAVITY_API AMyHUD : public AHUD
{
	GENERATED_BODY()

public:
	virtual void DrawHUD() override;

	void BeginPlay();

	UFUNCTION()
	void UpdateScore(int score);

	UFUNCTION()
	void NewMessage(FString text, float time, FLinearColor textcolor);

	UPROPERTY(EditDefaultsOnly, Category = "Widget")
	TSubclassOf<class UUserWidget> WidgetTemplate;

	UPROPERTY(EditDefaultsOnly, Category = "Widget")
	UMyUserWidget* WidgetInstance;
};
