#include "SteamGameInstanceSubsystem.h"

#include "OnlineSubsystem.h"
#include "Online.h"
#include "Kismet/GameplayStatics.h"
#include "Online/OnlineSessionNames.h"

USteamGameInstanceSubsystem::USteamGameInstanceSubsystem() : _currentSession(nullptr), _currentSessionName(""), _currentSessionLevel(""), _currentMultiplayerState(EMultiplayerState::Idle)
{
}

bool USteamGameInstanceSubsystem::ShouldCreateSubsystem(UObject* Outer) const
{
	return true;
}

void USteamGameInstanceSubsystem::Initialize(FSubsystemCollectionBase& Collection)
{
	Super::Initialize(Collection);

	if (IOnlineSubsystem* SubSystem = IOnlineSubsystem::Get())
	{
		SessionInterface = SubSystem->GetSessionInterface();
		if (SessionInterface.IsValid())
		{
			SessionInterface->OnCreateSessionCompleteDelegates.AddUObject(this, &USteamGameInstanceSubsystem::OnCreateSessionComplete);
			SessionInterface->OnFindSessionsCompleteDelegates.AddUObject(this, &USteamGameInstanceSubsystem::OnFindSessionComplete);
			SessionInterface->OnJoinSessionCompleteDelegates.AddUObject(this, &USteamGameInstanceSubsystem::OnJoinSessionComplete);
			SessionInterface->OnDestroySessionCompleteDelegates.AddUObject(this, &USteamGameInstanceSubsystem::OnDestroySessionComplete);
			SessionInterface->OnSessionUserInviteAcceptedDelegates.AddUObject(this, &USteamGameInstanceSubsystem::OnSessionUserInviteAccepted);
			OnMultiplayerStateChangedDelegate.AddUniqueDynamic(this, &USteamGameInstanceSubsystem::OnMultiplayerStateChanged);
		}
	}
}

void USteamGameInstanceSubsystem::Deinitialize()
{
	Super::Deinitialize();
}

void USteamGameInstanceSubsystem::OpenFriendInviteList()
{
	IOnlineExternalUI* externalUI = IOnlineSubsystem::Get()->GetExternalUIInterface().Get();
	if (externalUI != nullptr)
	{
		externalUI->ShowInviteUI(0, _currentSessionName);
	}
}

void USteamGameInstanceSubsystem::StartSession()
{
	if (IOnlineSubsystem* SubSystem = IOnlineSubsystem::Get())
	{
		SessionInterface = SubSystem->GetSessionInterface();
		if (SessionInterface.IsValid() && _currentSession != nullptr)
		{
			UE_LOG(LogTemp, Warning, TEXT("SESSION STARTED."));
			
			FOnlineSessionSettings SessionSettings;
			SessionSettings.bAllowJoinInProgress = false;
			SessionSettings.bShouldAdvertise = false;
			SessionSettings.bAllowJoinViaPresence = false;
			SessionSettings.bIsDedicated = false;
			SessionSettings.bIsLANMatch = false;
			SessionSettings.bUsesPresence = false;
			SessionSettings.NumPublicConnections = SessionInterface->GetSessionSettings(_currentSessionName)->NumPublicConnections;
			SessionSettings.NumPrivateConnections = SessionInterface->GetSessionSettings(_currentSessionName)->NumPrivateConnections;
			SessionSettings.bUseLobbiesIfAvailable = false;
			SessionSettings.bAllowJoinViaPresenceFriendsOnly = false;
			
			SessionInterface->UpdateSession(FName(_currentSessionName),SessionSettings , true);
		}
	}
}

void USteamGameInstanceSubsystem::GetSessionSearchResults(TArray<FSessionData>& found_sessions)
{
	for (auto session : _searchResults)
	{
		FSessionData foundSession;
		foundSession._sessionId = session.Session.GetSessionIdStr();
		foundSession._owningPlayerName = session.Session.OwningUserName;
		foundSession._playerCount = session.Session.SessionSettings.NumPublicConnections - session.Session.NumOpenPublicConnections;
		found_sessions.Add(foundSession);
	}
}

void USteamGameInstanceSubsystem::JoinServer(FString session_str_id)
{
	UE_LOG(LogTemp, Warning, TEXT("Joining session %s"), *session_str_id);

	FOnlineSessionSearchResult sessionToJoin;
	for (auto session : _searchResults)
	{
		if (session.GetSessionIdStr() == session_str_id)
		{
			sessionToJoin = session;
			break;
		}
	}

	SessionInterface->JoinSession(0, FName(session_str_id), sessionToJoin);
	OnMultiplayerStateChangedDelegate.Broadcast(EMultiplayerState::Joining);
}

void USteamGameInstanceSubsystem::LeaveSession()
{
	SessionInterface->DestroySession(_currentSessionName);
}

void USteamGameInstanceSubsystem::FindServers()
{
	SessionSearch = MakeShareable(new FOnlineSessionSearch());
	SessionSearch->bIsLanQuery = false;
	SessionSearch->MaxSearchResults = 100;

	// ONLY WORKS WITH STEAM SUBSYSTEM
	SessionSearch->QuerySettings.Set(SEARCH_PRESENCE, true, EOnlineComparisonOp::Equals);
	SessionInterface->FindSessions(0, SessionSearch.ToSharedRef());
	
	OnMultiplayerStateChangedDelegate.Broadcast(EMultiplayerState::Searching);
}

void USteamGameInstanceSubsystem::CreateServer(FString session_name, bool friends_only, FString session_level)
{
	FName sessionName = FName(session_name);
	auto ExistingSession = SessionInterface->GetNamedSession(sessionName);
	if (ExistingSession != nullptr)
	{
		SessionInterface->DestroySession(sessionName);
		return;
	}
	
	FOnlineSessionSettings SessionSettings;
	SessionSettings.bAllowJoinInProgress = true;
	SessionSettings.bShouldAdvertise = true;
	SessionSettings.bAllowJoinViaPresence = true;
	SessionSettings.bIsDedicated = false;
	SessionSettings.bIsLANMatch = false;
	SessionSettings.bUsesPresence = true;
	SessionSettings.NumPublicConnections = friends_only ? 0 : 4;
	SessionSettings.NumPrivateConnections = friends_only ? 4 : 0;
	SessionSettings.bUseLobbiesIfAvailable = true;
	// This will make it friends only.
	SessionSettings.bAllowJoinViaPresenceFriendsOnly = friends_only;
	// This may break things
	SessionSettings.BuildUniqueId = GetBuildUniqueId();
	
	UE_LOG(LogTemp, Warning, TEXT("Creating session with OnlineSystem: %s"), *IOnlineSubsystem::Get()->GetSubsystemName().ToString());
	
	_currentSessionLevel = session_level;
	
	SessionInterface->CreateSession(0, FName(sessionName), SessionSettings);
	OnMultiplayerStateChangedDelegate.Broadcast(EMultiplayerState::Hosting);
}

void USteamGameInstanceSubsystem::OnSessionUserInviteAccepted(const bool was_successful, const int32 controller_id,
	TSharedPtr<const FUniqueNetId> user_id, const FOnlineSessionSearchResult& invite_result)
{
	// UserSettings save check, to see if we should perform first time calibration
	//if (UGameplayStatics::DoesSaveGameExist("UserSettings", 0))
	{
		UE_LOG(LogTemp, Warning, TEXT("Joining session %s"), *invite_result.GetSessionIdStr());
		SessionInterface->JoinSession(0, FName(invite_result.GetSessionIdStr()), invite_result);
		OnMultiplayerStateChangedDelegate.Broadcast(EMultiplayerState::Joining);
	}
}

bool USteamGameInstanceSubsystem::IsCurrentSessionPendingToStart()
{
	if (_currentSession != nullptr)
	{
		return _currentSession->SessionSettings.bAllowJoinInProgress;
	}

	return false;
}

void USteamGameInstanceSubsystem::OnMultiplayerStateChanged(EMultiplayerState new_result_state)
{
	_currentMultiplayerState = new_result_state;
}

void USteamGameInstanceSubsystem::OnCreateSessionComplete(FName SessionName, bool Succeeded)
{
	if (Succeeded)
	{
		UE_LOG(LogTemp, Warning, TEXT("Session successfully created."));
		GetWorld()->ServerTravel("/Game/Levels/" + _currentSessionLevel + "?listen");
		_currentSession = SessionInterface->GetNamedSession(SessionName);
		_currentSessionName = SessionName;
	}
	else
	{
		UE_LOG(LogTemp, Warning, TEXT("Session failed to create."));
		OnMultiplayerStateChangedDelegate.Broadcast(EMultiplayerState::Idle);
	}
}

void USteamGameInstanceSubsystem::OnFindSessionComplete(bool Succeeded)
{
	if (SessionSearch == nullptr)
	{
		return;
	}
	
	if (Succeeded)
	{
		_searchResults = SessionSearch->SearchResults;
		UE_LOG(LogTemp, Warning, TEXT("We found %d sessions."), _searchResults.Num());
	}
	else
	{
		UE_LOG(LogTemp, Warning, TEXT("Failed to the find sessions request."));
	}
	
	if (_currentMultiplayerState == EMultiplayerState::Searching)
	{
		OnMultiplayerStateChangedDelegate.Broadcast(EMultiplayerState::Idle);
	}
}

void USteamGameInstanceSubsystem::OnJoinSessionComplete(FName SessionName, EOnJoinSessionCompleteResult::Type Result)
{
	UE_LOG(LogTemp, Warning, TEXT("SessionName Received: %s"), *SessionName.ToString());

	switch (Result)
	{
	case EOnJoinSessionCompleteResult::Success:
		UE_LOG(LogTemp, Warning, TEXT("Success"));
		
		_currentSession = SessionInterface->GetNamedSession(SessionName);
		_currentSessionName = SessionName;
		
		if (APlayerController* PlayerController = UGameplayStatics::GetPlayerController(GetWorld(), 0))
		{
			FString JoinAddress = "";
			SessionInterface->GetResolvedConnectString(FName(SessionName), JoinAddress);
			if (JoinAddress != "")
			{
				PlayerController->ClientTravel(JoinAddress, TRAVEL_Absolute);
			}
		}
		break;
	case EOnJoinSessionCompleteResult::UnknownError:
		UE_LOG(LogTemp, Warning, TEXT("UnknownError"));
		break;
	case EOnJoinSessionCompleteResult::AlreadyInSession:
		UE_LOG(LogTemp, Warning, TEXT("AlreadyInSession"));
		break;
	case EOnJoinSessionCompleteResult::SessionIsFull:
		UE_LOG(LogTemp, Warning, TEXT("SessionIsFull"));
		break;
	case EOnJoinSessionCompleteResult::SessionDoesNotExist:
		UE_LOG(LogTemp, Warning, TEXT("SessionDoesNotExist"));
		break;
	case EOnJoinSessionCompleteResult::CouldNotRetrieveAddress:
		UE_LOG(LogTemp, Warning, TEXT("CouldNotRetrieveAddress"));
		break;
	}

	if (Result != EOnJoinSessionCompleteResult::Success)
	{
		OnMultiplayerStateChangedDelegate.Broadcast(EMultiplayerState::Idle);
	}
}

void USteamGameInstanceSubsystem::OnDestroySessionComplete(FName SessionName, bool Succeeded)
{
	if (Succeeded)
	{
		_currentSessionName = "";
		_currentSession = nullptr;
		OnMultiplayerStateChangedDelegate.Broadcast(EMultiplayerState::Idle);
	}
}
