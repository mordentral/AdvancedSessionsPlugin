#pragma once

#include "BlueprintDataDefinitions.h"
#include "Net/OnlineBlueprintCallProxyBase.h"
#include "StartMatchmakingCallbackProxy.generated.h"

UCLASS(MinimalAPI)
class UStartMatchmakingCallbackProxy : public UOnlineBlueprintCallProxyBase
{
	GENERATED_BODY()

	TWeakObjectPtr<UObject> WorldContextObject;

	TWeakObjectPtr<APlayerController> PlayerControllerWeakPtr;

	TArray<FSessionsSearchSetting> Filters;

	FDelegateHandle DelegateHandle;

	UFUNCTION()
	void OnMatchmakingComplete(FName SessionName, bool bWasSuccessful);

public:
	UPROPERTY(BlueprintAssignable)
	FEmptyOnlineDelegate OnSuccess;

	UPROPERTY(BlueprintAssignable)
	FEmptyOnlineDelegate OnFailure;

	UFUNCTION(BlueprintCallable, meta = (BlueprintInternalUseOnly = "true", WorldContext = "WorldContextObject", AutoCreateRefTerm="Filters"), Category = "Online|AdvancedSessions")
	static UStartMatchmakingCallbackProxy* StartMatchmaking(UObject* WorldContextObject, APlayerController* PlayerController, const TArray<FSessionsSearchSetting>& Filters);

	virtual void Activate() override;
};
