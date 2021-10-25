// Fill out your copyright notice in the Description page of Project Settings.


#include "MyUserWidget.h"
#include "Styling/SlateColor.h"
#include "Kismet/GameplayStatics.h"

UMyUserWidget::UMyUserWidget(const FObjectInitializer& ObjectInitializer)
	: Super(ObjectInitializer)
{

}

void UMyUserWidget::NativeConstruct()
{
	Super::NativeConstruct();

	/*FCompositeFont MyCompFont = FCompositeFont(FName(TEXT("MyDownloadFont")), "TTF/Kids_Note_Font.ttf'", EFontHinting::Default, EFontLoadingPolicy::LazyLoad);

	TSharedPtr<const FCompositeFont> MyInCompositeFont(new FCompositeFont(MyCompFont));

	FSlateFontInfo MyScoreFont = FSlateFontInfo(MyInCompositeFont, 40);
	FSlateFontInfo MyMesFont = FSlateFontInfo(MyInCompositeFont, 60);*/
	if (ScoreBlock)
	{
		UpdateScoreInWidget(0);
		//ScoreBlock->SetFont(MyScoreFont);
	}

}

void UMyUserWidget::NativeTick(const FGeometry& MyGeometry, float InDeltaTime)
{
	Super::NativeTick(MyGeometry, InDeltaTime);
}

void UMyUserWidget::UpdateScoreInWidget(int32 score)
{
	FString text;

	text = "Score: " + FString::FromInt(score);
	if (ScoreBlock)
	{
		ScoreBlock->SetText(FText::FromString(text));
		ScoreBlock->SetJustification(ETextJustify::Center);
		//ScoreBlock->SetFont(FSlateFontInfo(, 60, FName("Roboto")));
		//ScoreBlock->Font.Size = 60;
	}
}

void UMyUserWidget::HideMessage()
{
	if (Message)
		Message->SetText(FText::FromString(" "));
}

void UMyUserWidget::ShowMessage(FString text, float time, FLinearColor textcolor)
{
	FSlateColor NewColor = FSlateColor(textcolor);

	if (Message)
	{
		Message->SetText(FText::FromString(text));
		Message->SetJustification(ETextJustify::Center);
		//Message->Font.Size = 60;
		if (NewColor.IsColorSpecified())
			Message->SetColorAndOpacity(NewColor);
	}

	GetWorld()->GetTimerManager().SetTimer(MessageKillTimerHandle, this, &UMyUserWidget::HideMessage, time, true);
}
