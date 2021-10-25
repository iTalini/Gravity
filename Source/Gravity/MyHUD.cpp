// Fill out your copyright notice in the Description page of Project Settings.


#include "MyHUD.h"

void AMyHUD::DrawHUD()
{
	Super::DrawHUD();
}

void AMyHUD::BeginPlay()
{
	if (WidgetTemplate)
	{
		WidgetInstance = CreateWidget<UMyUserWidget>(GetWorld(), WidgetTemplate);
		if (WidgetInstance)
		{
			WidgetInstance->AddToViewport();
			WidgetInstance->UpdateScoreInWidget(0);
		}
	}
}

void AMyHUD::UpdateScore(int score)
{
	if (WidgetInstance)
	{
		WidgetInstance->UpdateScoreInWidget(score);
	}
}

void AMyHUD::NewMessage(FString text, float time, FLinearColor textcolor)
{
	if (WidgetInstance)
	{
		WidgetInstance->ShowMessage(text, time, textcolor);
	}
}
