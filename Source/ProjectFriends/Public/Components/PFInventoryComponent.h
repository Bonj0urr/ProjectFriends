// Just a chill copyright notice

#pragma once

#include "CoreMinimal.h"
#include "Components/ActorComponent.h"
#include "PFInventoryComponent.generated.h"

DECLARE_MULTICAST_DELEGATE_OneParam(FOnInventoryItemsChangedSignature, TArray<FPFInventoryItem>/*InventoryItems*/);

class APFBaseItem;

USTRUCT(BlueprintType)
struct FPFInventoryItem
{
    GENERATED_BODY()

    FPFInventoryItem() : ItemDataId(FPrimaryAssetId(NAME_None, NAME_None)) , CachedItem(nullptr) {}

    UPROPERTY()
    FPrimaryAssetId ItemDataId;

    UPROPERTY(NotReplicated)
    APFBaseItem* CachedItem;
};

UCLASS( ClassGroup=(Custom), meta=(BlueprintSpawnableComponent) )
class PROJECTFRIENDS_API UPFInventoryComponent : public UActorComponent
{
	GENERATED_BODY()

public:	
	UPFInventoryComponent();

    virtual void GetLifetimeReplicatedProps(TArray<FLifetimeProperty>& OutLifetimeProps) const override;

    void TryAddItem(APFBaseItem* const ItemToAdd);

    FORCEINLINE TArray<FPFInventoryItem> GetInventoryItems() const { return InventoryItems; }

    FORCEINLINE int32 GetInventorySize() const { return InventorySize; }

    void UseItem(int32 ItemSlotId);

protected:
	virtual void BeginPlay() override;

private:
    UFUNCTION()
    void OnRep_InventoryItemsChanged();

public:
    FOnInventoryItemsChangedSignature OnInventoryItemsChanged;

private:
    UPROPERTY(ReplicatedUsing = OnRep_InventoryItemsChanged)
    TArray<FPFInventoryItem> InventoryItems;

    UPROPERTY(EditDefaultsOnly, BlueprintReadOnly, Category = "Inventory", meta = (AllowPrivateAccess = "true"))
    int32 InventorySize;
		
};
