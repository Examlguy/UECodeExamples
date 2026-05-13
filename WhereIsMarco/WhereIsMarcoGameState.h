#pragma once

#include "CoreMinimal.h"
#include "StageConfigurationDataAsset.h"
#include "WhereIsMarcoGameMode.h"
#include "GameFramework/GameState.h"
#include "Kismet/KismetMathLibrary.h"
#include "WhereIsMarcoGameState.generated.h"

#define START_MARCO_LOCATION 0

DECLARE_DYNAMIC_MULTICAST_DELEGATE_OneParam(FOnGameStageChanged, uint8, new_gamestage);
DECLARE_DYNAMIC_MULTICAST_DELEGATE_OneParam(FOnRoundStateChanged, ERoundState, new_roundstate);
DECLARE_DYNAMIC_MULTICAST_DELEGATE_OneParam(FOnPlayerWon, APawn*, winning_player);
DECLARE_DYNAMIC_MULTICAST_DELEGATE_OneParam(FOnShowingMarcoScreenshot, int, key);
DECLARE_DYNAMIC_MULTICAST_DELEGATE(FOnAllPlayersReady);

USTRUCT(BlueprintType)
struct FMarcoLocation
{
	GENERATED_BODY()

	FMarcoLocation() : _locationPicture(nullptr) {}
	FMarcoLocation(UTexture2D* texture, FVector location) : _locationPicture(texture), _locationVector(location) {}

	UPROPERTY(BlueprintReadOnly)
	TObjectPtr<UTexture2D> _locationPicture;
	
	UPROPERTY(BlueprintReadOnly)
	FVector _locationVector;
};

UCLASS()
class WHEREISMARCO_API AWhereIsMarcoGameState : public AGameState
{
	GENERATED_BODY()
public:
	virtual void BeginPlay() override;
	virtual void GetLifetimeReplicatedProps(TArray<FLifetimeProperty>& OutLifetimeProps) const override;
	
	UFUNCTION(NetMulticast, Reliable)
	void Multicast_GameStageUpdated(uint8 new_gamestage);

	UFUNCTION(NetMulticast, Reliable)
	void Multicast_RoundStateUpdated(ERoundState new_roundstate);

	UFUNCTION(NetMulticast, Reliable, BlueprintCallable)
	void Multicast_KillPlayer(AMonsterBaseCharacter* killer_character = nullptr, APlayerCharacter* playerCharacter = nullptr, float animation_duration = 0.0f);

	UFUNCTION(NetMulticast, Reliable, BlueprintCallable)
	void Multicast_ToggleLights(bool toggle_on);
	
	UFUNCTION(BlueprintImplementableEvent)
	void ToggleAllLights(bool activated);
	
	UFUNCTION(BlueprintCallable)
	void FoundMarcoLocation(APawn* discovering_player);
	
	UFUNCTION(NetMulticast, Reliable, BlueprintCallable)
	void Multicast_ShowMarcoLocationScreenshot(int key);
	
	UFUNCTION(BlueprintNativeEvent, BlueprintCallable)
	void AddMarcoLocationScreenshot(int key, UTexture2D* texture, FVector location);
	
	UFUNCTION(NetMulticast, Reliable, BlueprintCallable)
	void Multicast_RestartMarcoLocationTextures(int num_screenshots_to_expect);

	UFUNCTION(NetMulticast, Reliable, BlueprintCallable)
	void Multicast_AllPlayersReady();

	UFUNCTION(Netmulticast, Reliable, BlueprintCallable)
	void Multicast_SetPlayersAllowedToMove(bool can_move);

	UFUNCTION(BlueprintCallable)
	void SetPlayersAllowedToMove(bool can_move);
	
	UFUNCTION(BlueprintCallable)
	int GetMarcoLocationsNum() const { return _marcoLocationsReceived.Num(); }
	
	UFUNCTION(BlueprintCallable)
	bool HasAllScreenshots() { return _numberOfMarcoLocationsExpected == _marcoLocationsReceived.Num(); }
	
	UFUNCTION(BlueprintCallable)
	uint8 GetGameStage() const { return _gameStage; }
	
	UFUNCTION(BlueprintPure)
	FMarcoLocation GetMarcoLocation(int index) const
	{
		if (_marcoLocationsReceived.Num() == 0)
		{
			return FMarcoLocation();
		}
		
		return UKismetMathLibrary::InRange_IntInt(index, 0, _marcoLocationsReceived.Num() - 1)
		? _marcoLocationsReceived[index] : FMarcoLocation();
	}

	UFUNCTION(BlueprintCallable)
	void SetCurrentMarcoLocationClue(int clue) { _currentMarcoLocationClue = clue; }
	
	UFUNCTION(BlueprintCallable, BlueprintPure)
	int GetCurrentMarcoLocationClue() const { return _currentMarcoLocationClue; }

	UFUNCTION(BlueprintCallable, BlueprintPure)
	TArray<AActor*> GetToggleableLights() const { return _toggleableLights; } 

	UFUNCTION(BlueprintCallable, BlueprintPure)
	UStageConfigurationDataAsset* GetStageConfigurationDataAsset() const { return _stageConfigurationData; }
	
	UPROPERTY(BlueprintAssignable)
	FOnGameStageChanged OnGameStageChanged;
	
	UPROPERTY(BlueprintAssignable)
	FOnRoundStateChanged OnRoundStateChanged;

	UPROPERTY(BlueprintAssignable)
	FOnPlayerWon OnPlayerWon;

	UPROPERTY(BlueprintAssignable)
	FOnShowingMarcoScreenshot OnShowingMarcoScreenshot;

	UPROPERTY(Replicated, BlueprintReadOnly)
	int _numberOfMarcoLocationsExpected = 0;

	UPROPERTY(Replicated, BlueprintReadOnly)
	int _currentMarcoLocationClue = START_MARCO_LOCATION;
	
private:
	UPROPERTY()
	uint8 _gameStage = 0;

	UPROPERTY()
	ERoundState _gameRoundState = ERoundState::BEGINNING;

	UPROPERTY()
	TMap<int, FMarcoLocation> _marcoLocationsReceived;

	UPROPERTY()
	TArray<TObjectPtr<AActor>> _toggleableLights;

	UPROPERTY(EditAnywhere)
	TObjectPtr<UStageConfigurationDataAsset> _stageConfigurationData;
	
	UPROPERTY(BlueprintAssignable)
	FOnAllPlayersReady OnAllPlayersReadyDelegate;
};
