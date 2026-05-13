// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "OnlineSubsystem.h"
#include "OnlineSessionSettings.h"
#include "Interfaces/OnlineSessionInterface.h"
#include "Subsystems/GameInstanceSubsystem.h"

#include "SteamGameInstanceSubsystem.generated.h"

UENUM(BlueprintType)
enum class EMultiplayerState : uint8
{
	Idle = 0 UMETA(DisplayName = "Idle"),
	Searching = 1 UMETA(DisplayName = "Searching"),
	Hosting = 2 UMETA(DisplayName = "Hosting"),
	Joining = 3 UMETA(DisplayName = "Joining"),
	Playing = 4 UMETA(DisplayName = "Playing")
};

USTRUCT(BlueprintType)
struct FSessionData
{
	GENERATED_BODY()

	bool operator==(const FSessionData& Other) const
	{
		return _sessionId == Other._sessionId &&
			_owningPlayerName == Other._owningPlayerName &&
				_isFriendsOnly == Other._isFriendsOnly &&
					_playerCount == Other._playerCount;
	}

	UPROPERTY(BlueprintReadWrite)
	FString _sessionId;
	
	UPROPERTY(BlueprintReadWrite)
	FString _owningPlayerName;

	UPROPERTY(BlueprintReadWrite)
	FString _gameVersion;
	
	UPROPERTY(BlueprintReadWrite)
	bool _isFriendsOnly;
	
	UPROPERTY(BlueprintReadWrite)
	int _playerCount;
};

DECLARE_DYNAMIC_MULTICAST_DELEGATE_OneParam(FOnMultiplayerStateChanged, EMultiplayerState, new_result_state);

UCLASS()
class WHEREISMARCO_API USteamGameInstanceSubsystem : public UGameInstanceSubsystem
{
	GENERATED_BODY()
public:
	USteamGameInstanceSubsystem();
	
	virtual bool ShouldCreateSubsystem(UObject* Outer) const override;

	virtual void Initialize(FSubsystemCollectionBase& Collection) override;

	virtual void Deinitialize() override;

	UFUNCTION(BlueprintCallable)
	void OpenFriendInviteList();

	UFUNCTION(BlueprintCallable)
	void StartSession();
	
	UFUNCTION(BlueprintCallable)
	void GetSessionSearchResults(TArray<FSessionData>& found_sessions);
	
	UFUNCTION(BlueprintCallable)
	void JoinServer(FString session_str_id);

	UFUNCTION(BlueprintCallable)
	void LeaveSession();
	
	UFUNCTION(BlueprintCallable)
	void FindServers();
	
	UFUNCTION(BlueprintCallable)
	void CreateServer(FString session_name, bool friends_only, FString session_level);

	void OnSessionUserInviteAccepted(const bool was_successful, const int32 controller_id, TSharedPtr<const FUniqueNetId> user_id, const FOnlineSessionSearchResult& invite_result);
	
	UFUNCTION(BlueprintCallable, BlueprintPure)
	bool IsCurrentSessionPendingToStart();

	UFUNCTION()
	void OnMultiplayerStateChanged(EMultiplayerState new_result_state);

	UFUNCTION(BlueprintCallable)
	bool IsCurrentlyInSession() const { return _currentSession != nullptr; }
	
	UPROPERTY(BlueprintAssignable)
	FOnMultiplayerStateChanged OnMultiplayerStateChangedDelegate;
	
protected:
	TSharedPtr<FOnlineSessionSearch> SessionSearch;
	IOnlineSessionPtr SessionInterface;

	void OnCreateSessionComplete(FName SessionName, bool Succeeded);
	void OnFindSessionComplete(bool Succeeded);
	void OnJoinSessionComplete(FName SessionName, EOnJoinSessionCompleteResult::Type Result);
	void OnDestroySessionComplete(FName SessionName, bool Succeeded);
	
private:
	TArray<FOnlineSessionSearchResult> _searchResults;
	FNamedOnlineSession* _currentSession;
	FName _currentSessionName;
	FString _currentSessionLevel;
	EMultiplayerState _currentMultiplayerState;
};
