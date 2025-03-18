// Just a chill copyright notice

#pragma once

#include "CoreMinimal.h"
#include "Engine/DataAsset.h"
#include "PFItemData.generated.h"

UCLASS()
class PROJECTFRIENDS_API UPFItemData : public UPrimaryDataAsset
{
	GENERATED_BODY()
    
public:
    FPrimaryAssetId GetPrimaryAssetId() const override;

public:
    UPROPERTY(EditAnywhere, BlueprintReadOnly, Category = "Display")
    FText DisplayName;

    UPROPERTY(EditAnywhere, BlueprintReadOnly, Category = "Display")
    UTexture2D* Icon;
};
