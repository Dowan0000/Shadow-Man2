// Fill out your copyright notice in the Description page of Project Settings.


#include "T1GameInstance.h"

#include "OnlineSubsystem.h"
#include "OnlineSessionSettings.h"

#include "Kismet/GameplayStatics.h"

UT1GameInstance::UT1GameInstance()
{
}

void UT1GameInstance::Init()
{
	if (IOnlineSubsystem* Subsystem = IOnlineSubsystem::Get())
	{
		SessionInterface = Subsystem->GetSessionInterface();
		if (SessionInterface.IsValid())
		{
			SessionInterface->OnCreateSessionCompleteDelegates.AddUObject(this, &UT1GameInstance::OnCreateSessionComplete);
			SessionInterface->OnFindSessionsCompleteDelegates.AddUObject(this, &UT1GameInstance::OnFindSessionsComplete);
			SessionInterface->OnJoinSessionCompleteDelegates.AddUObject(this, &UT1GameInstance::OnJoinSessionComplete);
			
			//SessionInterface->OnDestroySessionCompleteDelegates.AddUObject(this, &UT1GameInstance::OnDestroySessionComplete);
		}
	}
}

void UT1GameInstance::OnCreateSessionComplete(FName SessionName, bool Success)
{
	if(Success)
	{
		UE_LOG(LogTemp, Warning, TEXT("Session created"));

		GetWorld()->ServerTravel("/Game/Maps/GameLevel?listen");

	}
	else
	{
		UE_LOG(LogTemp, Warning, TEXT("Session not created"));
	}
}

void UT1GameInstance::OnFindSessionsComplete(bool Success)
{
	if(Success)
	{
		TArray<FOnlineSessionSearchResult> SearchResults = SessionSearch->SearchResults;
		UE_LOG(LogTemp, Warning, TEXT("Session found, Server Count : %d"), SearchResults.Num());

		if(SearchResults.Num())
		{
			UE_LOG(LogTemp, Warning, TEXT("Join Server"));
			SessionInterface->JoinSession(0, FName("My Session"), SearchResults[0]);
		}
		else
		{
			CreateServer();
		}
	}
}

void UT1GameInstance::OnJoinSessionComplete(FName SessionName, EOnJoinSessionCompleteResult::Type Result)
{
	UE_LOG(LogTemp, Warning, TEXT("OnJoinSessionComplete"));

	if (APlayerController* PlayerController = UGameplayStatics::GetPlayerController(GetWorld(), 0))
	{
		FString JoinAddress = "";
		SessionInterface->GetResolvedConnectString(SessionName, JoinAddress);;
		if (JoinAddress != "")
		{
			UE_LOG(LogTemp, Warning, TEXT("Join Address : %s"), *JoinAddress);
			PlayerController->ClientTravel(JoinAddress, ETravelType::TRAVEL_Absolute);
		}
	}
}

void UT1GameInstance::CreateServer()
{
	UE_LOG(LogTemp, Warning, TEXT("create Server"));

	FOnlineSessionSettings SessionSettings;
	SessionSettings.bIsDedicated = false;
	SessionSettings.bIsLANMatch = false;
	SessionSettings.NumPublicConnections = 5;
	SessionSettings.bShouldAdvertise = true;
	SessionSettings.bUsesPresence = true;
	SessionSettings.bUseLobbiesIfAvailable = true;

	SessionInterface->CreateSession(0, FName("My Session"), SessionSettings);
}

void UT1GameInstance::JoinServer()
{
	UE_LOG(LogTemp, Warning, TEXT("Find Server"));

	SessionSearch = MakeShareable(new FOnlineSessionSearch());
	SessionSearch->bIsLanQuery = false; // Is LAN
	SessionSearch->MaxSearchResults = 10000;
	SessionSearch->QuerySettings.Set(SEARCH_PRESENCE, true, EOnlineComparisonOp::Equals);

	SessionInterface->FindSessions(0, SessionSearch.ToSharedRef());
}
