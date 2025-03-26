// Just a chill copyright notice

#pragma once

#include "CoreMinimal.h"
#include "Subsystems/GameInstanceSubsystem.h"
#include "PFNetworkSubsystem.generated.h"

UCLASS()
class PROJECTFRIENDS_API UPFNetworkSubsystem : public UGameInstanceSubsystem
{
	GENERATED_BODY()
	
public:
    UFUNCTION(BlueprintCallable, Category = "PF|Networking")
    void SetNetUpdateFrequencyForItems(float NewFrequency);

    static void SetNetUpdateFrequencyForItemsCommand(const TArray<FString>& Args);
};
