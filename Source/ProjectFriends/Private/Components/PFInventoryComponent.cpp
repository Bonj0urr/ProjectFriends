// Just a chill copyright notice

#include "Components/PFInventoryComponent.h"
#include "Net/UnrealNetwork.h"
#include "Items/PFItemData.h"
#include "Items/PFBaseItem.h"
#include "Characters/PFBaseCharacter.h"

UPFInventoryComponent::UPFInventoryComponent()
    : InventorySize(3)
{
	PrimaryComponentTick.bCanEverTick = false;
    SetIsReplicatedByDefault(true);
}

void UPFInventoryComponent::BeginPlay()
{
	Super::BeginPlay();

    InventoryItems.Reserve(InventorySize);
}

void UPFInventoryComponent::OnRep_InventoryItemsChanged()
{
    OnInventoryItemsChanged.Broadcast(InventoryItems);
}

void UPFInventoryComponent::GetLifetimeReplicatedProps(TArray<FLifetimeProperty>& OutLifetimeProps) const
{
    Super::GetLifetimeReplicatedProps(OutLifetimeProps);

    DOREPLIFETIME(UPFInventoryComponent, InventoryItems);
}

void UPFInventoryComponent::TryAddItem(APFBaseItem* const ItemToAdd)
{
    APFBaseCharacter* const CharacterOwner = Cast<APFBaseCharacter>(GetOwner());
    if (!CharacterOwner) return;

    checkf(CharacterOwner->HasAuthority(), TEXT("Should only run on server!"))

    if (!ItemToAdd) return;

    if (InventoryItems.Num() >= InventorySize) return;

    FPFInventoryItem NewInventoryItem;
    NewInventoryItem.ItemDataId = ItemToAdd->GetItemDataPrimaryAssetId();
    NewInventoryItem.CachedItem = ItemToAdd;

    InventoryItems.Add(NewInventoryItem);
    /* Manual update for server copy */
    OnRep_InventoryItemsChanged();

    ItemToAdd->SetOwner(CharacterOwner);
    ItemToAdd->SetIsItemEnabled(false);
}

void UPFInventoryComponent::UseItem(int32 ItemSlotId)
{

}

