// Fill out your copyright notice in the Description page of Project Settings.


#include "TPSGameInstance.h"
#include "SocketSubsystem.h"
#include "PlatformTrigger.h"
#include "Blueprint/UserWidget.h"
#include "Kismet/GameplayStatics.h"
#include "MenuSystem/MainMenu.h"
#include "MenuSystem/InGameMenu.h"
#include "GameFramework/GameModeBase.h"
#include "OnlineSubsystem.h"
#include "OnlineSessionSettings.h"
#include "Interfaces/OnlineSessionInterface.h"

const static FName SESSION_NAME = TEXT("GameSession");
const static FName SERVER_NAME_SETTING_KEY = TEXT("ServerName");

UTPSGameInstance::UTPSGameInstance()
{
	static ConstructorHelpers::FClassFinder<UUserWidget> WBP_MainMenu(TEXT("WidgetBlueprint'/Game/MenuSystem/WBP_MainMenu.WBP_MainMenu_C'"));
	if (WBP_MainMenu.Succeeded())
	{
		MenuClass = WBP_MainMenu.Class;
	}

	static ConstructorHelpers::FClassFinder<UUserWidget> WBP_InGameMenu(TEXT("WidgetBlueprint'/Game/MenuSystem/WBP_InGameMenu.WBP_InGameMenu_C'"));
	if (WBP_InGameMenu.Succeeded())
	{
		InGameMenuClass = WBP_InGameMenu.Class;
	}
}

void UTPSGameInstance::Init()
{
	Super::Init();

	OnlineSubsystem = IOnlineSubsystem::Get();
	if (OnlineSubsystem != nullptr)
	{
		UE_LOG(LogTemp, Error, TEXT("OnlineSubsystem Name is %s"), *OnlineSubsystem->GetSubsystemName().ToString());
		SessionInterface = OnlineSubsystem->GetSessionInterface();
		if (SessionInterface.IsValid())
		{
			SessionInterface->OnCreateSessionCompleteDelegates.AddUObject(this, &UTPSGameInstance::OnCreateSessionComplete);
			SessionInterface->OnDestroySessionCompleteDelegates.AddUObject(this, &UTPSGameInstance::OnDestroySessionComplete);
			SessionInterface->OnFindSessionsCompleteDelegates.AddUObject(this, &UTPSGameInstance::OnFindSessionsComplete);
			SessionInterface->OnJoinSessionCompleteDelegates.AddUObject(this, &UTPSGameInstance::OnJoinSessionComplete);
		}
	}
	else
	{
		UE_LOG(LogTemp, Error, TEXT("OnlineSubsystem is null"));
	}

	if (GEngine != nullptr)
	{
		GEngine->OnNetworkFailure().AddUObject(this, &UTPSGameInstance::OnNetworkFailure);
	}

}

void UTPSGameInstance::LoadMenuWidget()
{
	if (MenuClass)
	{
		Menu = CreateWidget<UMainMenu>(this, MenuClass);
		if (Menu == nullptr)
			return;

		Menu->Setup();

		Menu->SetMenuInterface(this);
	}

	if (SessionInterface.IsValid())
	{
		SessionInterface->DestroySession(SESSION_NAME);
	}
}

void UTPSGameInstance::LoadInGameMenu()
{
	if (InGameMenuClass)
	{
		InGameMenu = CreateWidget<UInGameMenu>(this, InGameMenuClass);
		if (InGameMenu == nullptr)
			return;

		InGameMenu->SetMenuInterface(this);
	}
}

void UTPSGameInstance::OnInGameMenu()
{
	if (InGameMenu && InGameMenu->GetIsActive())
	{
		InGameMenu->Inactive();
	}
	else
	{
		InGameMenu->OnActive();
	}
}

void UTPSGameInstance::Host(const FString& InputServerName)
{
	if (SessionInterface.IsValid())
	{
		ServerName = InputServerName;
		auto ExistingSession = SessionInterface->GetNamedSession(SESSION_NAME);
		if (ExistingSession != nullptr)
		{
			SessionInterface->DestroySession(SESSION_NAME);
		}
		else
		{
			CreateSession();
		}
	}
}

void UTPSGameInstance::Join(uint32 Index)
{
	if (!SessionInterface.IsValid()) return;
	if (!SessionSearch.IsValid()) return;

	SessionInterface->JoinSession(0, SESSION_NAME, SessionSearch->SearchResults[Index]);
}

void UTPSGameInstance::LoadMainMenu()
{
	APlayerController* PlayerController = GetFirstLocalPlayerController();
	if (PlayerController == nullptr)
		return;

	PlayerController->ClientTravel("/Game/MenuSystem/MainMenu", ETravelType::TRAVEL_Absolute);
}

void UTPSGameInstance::Quit()
{
	APlayerController* PlayerController = GetFirstLocalPlayerController();
	if (PlayerController == nullptr)
		return;

	PlayerController->ConsoleCommand("Quit");
}

void UTPSGameInstance::FindSessions()
{
	SessionSearch = MakeShareable(new FOnlineSessionSearch());
	if (SessionSearch.IsValid() && SessionInterface.IsValid())
	{
		//SessionSearch->bIsLanQuery = true;
		SessionSearch->MaxSearchResults = 100;
		SessionSearch->QuerySettings.Set(SEARCH_PRESENCE, true, EOnlineComparisonOp::Equals);
		UE_LOG(LogTemp, Warning, TEXT("Start Find Sessions"));
		SessionInterface->FindSessions(0, SessionSearch.ToSharedRef());
	}
}

void UTPSGameInstance::OnCreateSessionComplete(FName SessionName, bool Success)
{
	if (Success == false)
	{
		UE_LOG(LogTemp, Error, TEXT("Could not create session"));
		return;
	}

	if (Menu)
	{
		Menu->Teardown();
	}

	UEngine* Engine = GetEngine();
	if (Engine == nullptr)
		return;

	Engine->AddOnScreenDebugMessage(0, 2.f, FColor::Green, TEXT("Hosting"));


	auto World = GetWorld();
	if (World == nullptr)
		return;

	World->ServerTravel("/Game/Platforms/Maps/Lobby?listen");
}

void UTPSGameInstance::OnDestroySessionComplete(FName SessionName, bool Success)
{
	UEngine* Engine = GetEngine();
	if (Engine == nullptr)
		return;

	if (Success)
	{
		Engine->AddOnScreenDebugMessage(0, 2.f, FColor::Green, TEXT("Success Destroy Session"));
		return;
	}
	else
	{
		Engine->AddOnScreenDebugMessage(0, 2.f, FColor::Red, TEXT("Fail Destroy Session"));
		return;
	}
}

void UTPSGameInstance::OnFindSessionsComplete(bool Success)
{
	if (Success && SessionSearch.IsValid() && Menu)
	{
		UE_LOG(LogTemp, Warning, TEXT("Complete Find Sessions"));

		TArray<FServerData> ServerNames;
		for (const FOnlineSessionSearchResult& SearchResult : SessionSearch->SearchResults)
		{
			UE_LOG(LogTemp, Error, TEXT("Found Session ID: %s"), *SearchResult.GetSessionIdStr());
			FServerData Data;
			FString GetServerName;
			if (SearchResult.Session.SessionSettings.Get(SERVER_NAME_SETTING_KEY, GetServerName))
			{
				Data.Name = GetServerName;
			}
			else
			{
				Data.Name = "Could not find ServerName";
			}
			Data.MaxPlayers = SearchResult.Session.SessionSettings.NumPublicConnections;
			Data.CurrentPlayers = Data.MaxPlayers - SearchResult.Session.NumOpenPublicConnections;
			Data.HostUsername = SearchResult.Session.OwningUserName;
			
			ServerNames.Emplace(Data);
		}

		Menu->SetServerList(ServerNames);
	}
}

void UTPSGameInstance::OnJoinSessionComplete(FName SessionName, EOnJoinSessionCompleteResult::Type Result)
{
	if (!SessionInterface.IsValid()) return;

	UEngine* Engine = GetEngine();
	if (Engine == nullptr)
		return;

	switch (Result)
	{
	case EOnJoinSessionCompleteResult::Success:
	{
		FString Address;
		if (!SessionInterface->GetResolvedConnectString(SessionName, Address))
		{
			UE_LOG(LogTemp, Warning, TEXT("Fail to get connect string"));
			return;
		}

		Engine->AddOnScreenDebugMessage(0, 5.f, FColor::Green, FString::Printf(TEXT("Joining %s"), *Address));

		APlayerController* PlayerController = GetFirstLocalPlayerController();
		if (PlayerController == nullptr)
			return;

		PlayerController->ClientTravel(Address, ETravelType::TRAVEL_Absolute);

		if (Menu)
		{
			Menu->Teardown();
		}

		break;
	}
	case EOnJoinSessionCompleteResult::SessionIsFull:
	{
		Engine->AddOnScreenDebugMessage(0, 5.f, FColor::Green, TEXT("Session is full"));

		break;
	}
	case EOnJoinSessionCompleteResult::SessionDoesNotExist:
	{
		Engine->AddOnScreenDebugMessage(0, 5.f, FColor::Green, TEXT("Session Does Not Exist"));

		break;
	}
	case EOnJoinSessionCompleteResult::UnknownError:
	{
		Engine->AddOnScreenDebugMessage(0, 5.f, FColor::Green, TEXT("Unknown Error"));

		break;
	}
	case EOnJoinSessionCompleteResult::CouldNotRetrieveAddress:
	{
		Engine->AddOnScreenDebugMessage(0, 5.f, FColor::Green, TEXT("Could Not Retrieve Address"));

		break;
	}
	case EOnJoinSessionCompleteResult::AlreadyInSession:
	{
		Engine->AddOnScreenDebugMessage(0, 5.f, FColor::Green, TEXT("AlreadyInSession"));

		break;
	}
	}
}

void UTPSGameInstance::OnNetworkFailure(UWorld* World, UNetDriver* NetDriver, ENetworkFailure::Type FailureType, const FString& ErrorString)
{
	if (GEngine == nullptr) return;

	GEngine->AddOnScreenDebugMessage(1, 3.f, FColor::Blue, TEXT("Network Error"));
}

void UTPSGameInstance::CreateSession()
{
	if (SessionInterface.IsValid())
	{
		FOnlineSessionSettings SessionSettings;

		SessionSettings.bIsLANMatch = (IOnlineSubsystem::Get()->GetSubsystemName() == "NULL") ? true : false;
		SessionSettings.NumPublicConnections = 3;
		SessionSettings.bShouldAdvertise = true;
		SessionSettings.bUsesPresence = true;
		SessionSettings.bUseLobbiesIfAvailable = true;
		SessionSettings.Set(SERVER_NAME_SETTING_KEY, ServerName, EOnlineDataAdvertisementType::ViaOnlineServiceAndPing);

		SessionInterface->CreateSession(0, SESSION_NAME, SessionSettings);
	}
}

void UTPSGameInstance::StartSession()
{
	if (SessionInterface.IsValid())
	{
		SessionInterface->StartSession(SESSION_NAME);
	}
}
