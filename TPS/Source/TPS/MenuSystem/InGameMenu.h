// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "MenuWidget.h"
#include <stack>
#include "MenuInterface.h"

#include "InGameMenu.generated.h"

/**
 * 
 */
UCLASS()
class TPS_API UInGameMenu : public UMenuWidget
{
	GENERATED_BODY()
	
public:

	virtual bool Initialize() override;
	virtual void NativeConstruct() override;

	void OnActive();
	void OnInactive();

	UFUNCTION()
	void OnClick_InGameMenu_MainMenu();

	UFUNCTION()
	void OnClick_InGameMenu_Cancel();

	UFUNCTION()
	void OnClick_ConfirmMenu_Confirm();

	UFUNCTION()
	void OnClick_ConfirmMenu_Cancel();

	bool GetIsActive();

	void Inactive();

private:

	UPROPERTY(Transient, meta = (BindWidget))
	class UWidget* InGameMenu;

	UPROPERTY(Transient, meta = (BindWidget))
	class UWidget* ConfirmMenu;


	UPROPERTY(meta = (BindWidget))
	class UButton* Btn_InGameMenu_MainMenu;

	UPROPERTY(meta = (BindWidget))
	class UButton* Btn_InGameMenu_Cancel;


	UPROPERTY(meta = (BindWidget))
	class UWidgetSwitcher* WidgetSwitcher_MenuSwitcher;


	UPROPERTY(meta = (BindWidget))
	class UButton* Btn_ConfirmMenu_Confirm;

	UPROPERTY(meta = (BindWidget))
	class UButton* Btn_ConfirmMenu_Cancel;

	bool IsActive;

	std::stack<class UWidget*> MenuWidgets;
};
