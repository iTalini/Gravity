// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "Components/TextBlock.h"
#include "Blueprint/UserWidget.h"
#include "MyUserWidget.generated.h"

/**
 * 
 */
UCLASS(Abstract)
class GRAVITY_API UMyUserWidget : public UUserWidget
{
	GENERATED_BODY()

public:
	UMyUserWidget(const FObjectInitializer& ObjectInitializer);

	virtual void NativeConstruct() override;

	virtual void NativeTick(const FGeometry& MyGeometry, float InDeltaTime) override;

	UFUNCTION()
	void UpdateScoreInWidget(int32 score);

	FTimerHandle MessageKillTimerHandle;

	UFUNCTION()
	void HideMessage();

	UFUNCTION()
	void ShowMessage(FString text, float time, FLinearColor textcolor);

	UPROPERTY(BlueprintReadWrite, meta = (BindWidget))
	UTextBlock* ScoreBlock = nullptr;
	
	UPROPERTY(BlueprintReadWrite, meta = (BindWidget))
	UTextBlock* Message = nullptr;
};
