// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "Blueprint/UserWidget.h"
#include "ServerSlot.generated.h"

/**
 * 
 */
UCLASS()
class TPS_API UServerSlot : public UUserWidget
{
	GENERATED_BODY()

private:

	UPROPERTY(meta = (BindWidget))
	class UTextBlock* Text_ServerName;

	UPROPERTY(meta = (BindWidget))
	class UButton* Btn_Server;

};