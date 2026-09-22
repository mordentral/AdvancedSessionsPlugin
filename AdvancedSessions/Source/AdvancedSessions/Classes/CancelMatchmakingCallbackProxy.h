#pragma once

#include "BlueprintDataDefinitions.h"
#include "CancelMatchmakingCallbackProxy.generated.h"

UCLASS(MinimalAPI)
class UCancelMatchmakingCallbackProxy : public UOnlineBlueprintCallProxyBase
{
	GENERATED_BODY()

	virtual void Activate() override;

	void OnCancelMatchmakingComplete(FName SessionName, bool bWasSuccessful);

	TWeakObjectPtr<APlayerController> PlayerControllerWeakPtr;

	FOnCancelMatchmakingCompleteDelegate Delegate;

	FDelegateHandle DelegateHandle;

	TWeakObjectPtr<UObject> WorldContextObject;

public:
	UPROPERTY(BlueprintAssignable)
	FEmptyOnlineDelegate OnSuccess;

	UPROPERTY(BlueprintAssignable)
	FEmptyOnlineDelegate OnFailure;

	UFUNCTION(BlueprintCallable, meta=(BlueprintInternalUseOnly = "true", WorldContext="WorldContextObject"), Category = "Online|AdvancedSessions")
	static UCancelMatchmakingCallbackProxy* CancelMatchmaking(UObject* WorldContextObject, APlayerController* PlayerController);
};
