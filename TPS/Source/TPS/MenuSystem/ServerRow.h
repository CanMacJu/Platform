// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "Blueprint/UserWidget.h"
#include "ServerRow.generated.h"

/**
 * 
 */
UCLASS()
class TPS_API UServerRow : public UUserWidget
{
	GENERATED_BODY()
	
public:

	void SetLoadingText();

	void Setup(class UMainMenu* _Parent, uint32 _Index);

	UFUNCTION()
	void OnClicked();

	UFUNCTION()
	void OnHovered();

	UFUNCTION()
	void OnUnhovered();

	void ResetColor();

public:

	UPROPERTY(BlueprintReadOnly, meta = (BindWidget))
	class UTextBlock* Text_ServerName;

	UPROPERTY(BlueprintReadOnly, meta = (BindWidget))
	class UTextBlock* Text_OwnerName;

	UPROPERTY(BlueprintReadOnly, meta = (BindWidget))
	class UTextBlock* Text_Players;

private:

	UPROPERTY(meta = (BindWidget))
	class UButton* Btn_Index;

	UPROPERTY()
	class UMainMenu* Parent;

	uint32 Index;

	UPROPERTY(BlueprintReadOnly, meta = (AllowPrivateAccess = "true"))
	bool Selected = false;
};
