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

	void CallupInGameMenu();
	void CancelInGameMenu();
	void CompleteInGameMenu();

	UFUNCTION()
	void OnClick_InGameMenu_MainMenu();

	UFUNCTION()
	void LoadMainMenu();

	bool GetIsActive();

	UFUNCTION()
	void Cancel();

private:

	UPROPERTY(Transient, meta = (BindWidget))
	class UWidget* InGameMenu;

	UPROPERTY(Transient, meta = (BindWidget))
	class UWidget* ConfirmMenu;

	UPROPERTY(Transient, meta = (BindWidget))
	class UWidget* CompleteMenu;


	UPROPERTY(meta = (BindWidget))
	class UWidgetSwitcher* WidgetSwitcher_MenuSwitcher;

	// InGameMenu
	UPROPERTY(meta = (BindWidget))
	class UButton* Btn_InGameMenu_MainMenu;

	UPROPERTY(meta = (BindWidget))
	class UButton* Btn_InGameMenu_Cancel;

	// ConfirmMenu
	UPROPERTY(meta = (BindWidget))
	class UButton* Btn_ConfirmMenu_Confirm;

	UPROPERTY(meta = (BindWidget))
	class UButton* Btn_ConfirmMenu_Cancel;

	// CompleteMenu
	UPROPERTY(meta = (BindWidget))
	class UButton* Btn_CompleteMenu_Confirm;

	UPROPERTY(meta = (BindWidget))
	class UButton* Btn_CompleteMenu_Cancel;

	bool IsActive;

	std::stack<class UWidget*> MenuWidgets;
};
