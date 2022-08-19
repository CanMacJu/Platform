// Fill out your copyright notice in the Description page of Project Settings.


#include "ServerRow.h"
#include "OnlineSessionSettings.h"
#include "Components/TextBlock.h"
#include "MainMenu.h"
#include "Components/Button.h"

void UServerRow::SetLoadingText()
{
	Text_ServerName->SetText(FText::FromString(FString::Printf(TEXT("Loading Server List..."))));
	Text_OwnerName->SetText(FText::FromString(FString::Printf(TEXT(""))));
	Text_Players->SetText(FText::FromString(FString::Printf(TEXT(""))));
}

void UServerRow::Setup(UMainMenu* _Parent, uint32 _Index)
{
	Btn_Index->OnClicked.AddDynamic(this, &UServerRow::OnClicked);
	Btn_Index->OnHovered.AddDynamic(this, &UServerRow::OnHovered);
	Btn_Index->OnUnhovered.AddDynamic(this, &UServerRow::OnUnhovered);
	ResetColor();
	Parent = _Parent;
	Index = _Index;
}

void UServerRow::OnClicked()
{
	if (Parent)
	{
		Parent->RowsResetColor();
		Parent->SelectIndex(Index);
		FSlateColor Color = FSlateColor(FLinearColor(FVector4(0.f, 1.f, 0.f)));
		Text_ServerName->SetColorAndOpacity(Color);
		Text_OwnerName->SetColorAndOpacity(Color);
		Text_Players->SetColorAndOpacity(Color);
		Selected = true;
	}
}

void UServerRow::OnHovered()
{
	if (!Selected && Text_ServerName && Text_OwnerName && Text_Players)
	{
		FSlateColor Color = FSlateColor(FLinearColor(FVector4(1.f, 1.f, 0.f)));
		Text_ServerName->SetColorAndOpacity(Color);
		Text_OwnerName->SetColorAndOpacity(Color);
		Text_Players->SetColorAndOpacity(Color);
	}
}

void UServerRow::OnUnhovered()
{
	if (!Selected && Text_ServerName && Text_OwnerName && Text_Players)
	{
		ResetColor();
	}
}

void UServerRow::ResetColor()
{
	FSlateColor Color = FSlateColor(FLinearColor(FVector4(1.f, 1.f, 1.f)));
	Text_ServerName->SetColorAndOpacity(Color);
	Text_OwnerName->SetColorAndOpacity(Color);
	Text_Players->SetColorAndOpacity(Color);

	Selected = false;
}
