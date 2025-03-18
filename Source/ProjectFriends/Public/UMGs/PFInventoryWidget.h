// Just a chill copyright notice

#pragma once

#include "CoreMinimal.h"
#include "Blueprint/UserWidget.h"
#include "PFInventoryWidget.generated.h"

class UImage;
struct FPFInventoryItem;

UCLASS()
class PROJECTFRIENDS_API UPFInventoryWidget : public UUserWidget
{
	GENERATED_BODY()
	
public:
    UFUNCTION()
    void UpdateInventoryItems(TArray<FPFInventoryItem> NewInventoryItems);

protected:
    virtual void NativeConstruct() override;

private:
    void UpdateInventoryItemImage(FPrimaryAssetId LoadedId, UImage* InventoryItemImage);

protected:
    UPROPERTY(BlueprintReadWrite, meta = (BindWidget))
    UImage* InventoryItem1;

    UPROPERTY(BlueprintReadWrite, meta = (BindWidget))
    UImage* InventoryItem2;

    UPROPERTY(BlueprintReadWrite, meta = (BindWidget))
    UImage* InventoryItem3;

private:
    TArray<UImage*> InventoryItemImages;
};
