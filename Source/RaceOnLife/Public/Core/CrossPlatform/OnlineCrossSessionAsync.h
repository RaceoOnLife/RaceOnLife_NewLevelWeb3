#pragma once

#include "CoreMinimal.h"
#include "Kismet/BlueprintAsyncActionBase.h"
#include "Interfaces/OnlineSessionInterface.h"
#include "OnlineCrossSessionAsync.generated.h"

USTRUCT(BlueprintType)
struct FOnlinePlayerInfo
{
    GENERATED_BODY()

    UPROPERTY(BlueprintReadOnly, Category = "Player Info")
    FString Nickname;

    UPROPERTY(BlueprintReadOnly, Category = "Player Info")
    FString UniqueId;

    UPROPERTY(BlueprintReadOnly, Category = "Player Info")
    FString AvatarUrl; // Optional
};

USTRUCT(BlueprintType)
struct FSessionData
{
    GENERATED_BODY()

    FOnlineSessionSearchResult NativeResult;

    UPROPERTY(BlueprintReadOnly, Category = "Session")
    FString SessionId;

    UPROPERTY(BlueprintReadOnly, Category = "Session")
    FString HostName;

    UPROPERTY(BlueprintReadOnly, Category = "Session")
    int32 MaxPlayers;

    UPROPERTY(BlueprintReadOnly, Category = "Session")
    int32 OpenSlots;
};

UCLASS()
class USubsystemInfoAsync : public UBlueprintAsyncActionBase
{
    GENERATED_BODY()

public:
    UPROPERTY(BlueprintAssignable)
    FSimpleMulticastDelegate OnFound;

    UPROPERTY(BlueprintAssignable)
    FSimpleMulticastDelegate OnNotFound;

    UFUNCTION(BlueprintCallable, Category = "Online|Subsystem", meta = (BlueprintInternalUseOnly = "true", DisplayName = "Get Online Subsystem"))
    static USubsystemInfoAsync* GetOnlineSubsystem();

    virtual void Activate() override;

private:
    IOnlineSubsystem* Subsystem;
};

UCLASS()
class UGetLocalPlayerInfoAsync : public UBlueprintAsyncActionBase
{
    GENERATED_BODY()

public:
    UPROPERTY(BlueprintAssignable)
    FSimpleMulticastDelegate OnFailure;

    UPROPERTY(BlueprintAssignable)
    FSimpleMulticastDelegate OnSuccess;

    UFUNCTION(BlueprintCallable, Category = "Online|Player", meta = (BlueprintInternalUseOnly = "true", DisplayName = "Get Local Player Info"))
    static UGetLocalPlayerInfoAsync* GetLocalPlayerInfo();

    virtual void Activate() override;

    UPROPERTY(BlueprintReadOnly, Category = "Player Info")
    FOnlinePlayerInfo PlayerInfo;
};

DECLARE_DYNAMIC_MULTICAST_DELEGATE_OneParam(FOnSessionsFound, const TArray<FSessionData>&, Results);

UCLASS()
class UFindSessionsAsync : public UBlueprintAsyncActionBase
{
    GENERATED_BODY()

public:
    UPROPERTY(BlueprintAssignable)
    FOnSessionsFound OnSuccess;

    UPROPERTY(BlueprintAssignable)
    FSimpleMulticastDelegate OnFailure;

    UFUNCTION(BlueprintCallable, Category = "Online|Sessions", meta = (BlueprintInternalUseOnly = "true", DisplayName = "Find Online Sessions"))
    static UFindSessionsAsync* FindSessions(int32 MaxResults = 50);

    virtual void Activate() override;

private:
    TSharedPtr<FOnlineSessionSearch> SearchSettings;
    void HandleSearchComplete(bool bSuccess);
};

UCLASS()
class UManageSessionAsync : public UBlueprintAsyncActionBase
{
    GENERATED_BODY()

public:
    UPROPERTY(BlueprintAssignable)
    FSimpleMulticastDelegate OnSuccess;

    UPROPERTY(BlueprintAssignable)
    FSimpleMulticastDelegate OnFailure;

    UFUNCTION(BlueprintCallable, Category = "Online|Sessions", meta = (BlueprintInternalUseOnly = "true", DisplayName = "Create Session"))
    static UManageSessionAsync* CreateSession(int32 MaxPlayers);

    UFUNCTION(BlueprintCallable, Category = "Online|Sessions", meta = (BlueprintInternalUseOnly = "true", DisplayName = "End Session"))
    static UManageSessionAsync* EndSession();

    UFUNCTION(BlueprintCallable, Category = "Online|Sessions", meta = (BlueprintInternalUseOnly = "true", DisplayName = "Destroy Session"))
    static UManageSessionAsync* DestroySession();

    virtual void Activate() override;

private:
    enum class ESessionAction { Create, End, Destroy } Action;
    int32 MaxPlayers;
    void HandleCallback(FName SessionName, bool bWasSuccessful);
};

UCLASS()
class UMatchmakingAsync : public UBlueprintAsyncActionBase
{
    GENERATED_BODY()

public:
    UPROPERTY(BlueprintAssignable)
    FSimpleMulticastDelegate OnMatchFound;

    UPROPERTY(BlueprintAssignable)
    FSimpleMulticastDelegate OnMatchNotFound;

    UFUNCTION(BlueprintCallable, Category = "Online|Matchmaking", meta = (BlueprintInternalUseOnly = "true", DisplayName = "Start Matchmaking"))
    static UMatchmakingAsync* StartMatchmaking(int32 MaxPlayers);

    virtual void Activate() override;
};

UCLASS()
class UHostTransferAsync : public UBlueprintAsyncActionBase
{
    GENERATED_BODY()

public:
    UPROPERTY(BlueprintAssignable)
    FSimpleMulticastDelegate OnHostTransferred;

    UPROPERTY(BlueprintAssignable)
    FSimpleMulticastDelegate OnNoPlayersLeft;

    UFUNCTION(BlueprintCallable, Category = "Online|Sessions", meta = (BlueprintInternalUseOnly = "true", DisplayName = "Handle Host Migration"))
    static UHostTransferAsync* HandleHostExit();

    virtual void Activate() override;
};
