// Just a chill copyright notice

#include "Components/PFInventoryComponent.h"
#include "Net/UnrealNetwork.h"
#include "Items/PFItemData.h"
#include "Items/PFBaseItem.h"
#include "Characters/PFBaseCharacter.h"

UPFInventoryComponent::UPFInventoryComponent()
    : InventorySize(3), ReadyToUseId(-1)
{
	PrimaryComponentTick.bCanEverTick = false;
    SetIsReplicatedByDefault(true);
}

void UPFInventoryComponent::BeginPlay()
{
	Super::BeginPlay();

    InventoryItems.Reserve(InventorySize);
    for (int32 i = 0; i < InventorySize; i++)
    {
        InventoryItems.AddDefaulted();
    }
}

void UPFInventoryComponent::OnRep_InventoryItemsChanged()
{
    OnInventoryItemsChanged.Broadcast(InventoryItems);
}

void UPFInventoryComponent::GetLifetimeReplicatedProps(TArray<FLifetimeProperty>& OutLifetimeProps) const
{
    Super::GetLifetimeReplicatedProps(OutLifetimeProps);

    DOREPLIFETIME(UPFInventoryComponent, InventoryItems);

    /* TO DO add COND_*/
    DOREPLIFETIME(UPFInventoryComponent, ReadyToUseId);
}

void UPFInventoryComponent::TryAddItem(APFBaseItem* const ItemToAdd)
{
    APFBaseCharacter* const CharacterOwner = Cast<APFBaseCharacter>(GetOwner());
    if (!CharacterOwner) return;

    checkf(CharacterOwner->HasAuthority(), TEXT("Should only run on server!"))

    if (!ItemToAdd) return;

    /* Select Item slot id where we should try to add a new item */
    int32 SlotIdToAdd = -1;

    for (int32 i = 0; i < InventorySize; i++)
    {
        if (!InventoryItems[i].IsValid())
        {
            SlotIdToAdd = i;
            break;
        }
    }

    if (SlotIdToAdd == -1 ) return;

    /* Add an item to the inventory */
    FPFInventoryItem NewInventoryItem;
    NewInventoryItem.ItemDataId = ItemToAdd->GetItemDataPrimaryAssetId();
    NewInventoryItem.CachedItem = ItemToAdd;

    InventoryItems[SlotIdToAdd] = NewInventoryItem;

    /* Manual update for server copy */
    OnRep_InventoryItemsChanged();

    ItemToAdd->SetOwner(CharacterOwner);
    ItemToAdd->SetIsItemEnabled(false);
}

void UPFInventoryComponent::EquipItem(int32 ItemSlotId)
{
    APFBaseCharacter* const CharacterOwner = Cast<APFBaseCharacter>(GetOwner());
    if (!CharacterOwner) return;

    checkf(CharacterOwner->HasAuthority(), TEXT("Should only run on server!"))

    if (!InventoryItems.IsValidIndex(ItemSlotId)) return;

    if (!InventoryItems[ItemSlotId].ItemDataId.IsValid()) return;

    APFBaseItem* const SelectedInventoryItem = InventoryItems[ItemSlotId].CachedItem;
    if (!SelectedInventoryItem) return;

    /* If SelectedInventoryItem is already in hands, disable and detach it */
    if (SelectedInventoryItem->GetIsItemEnabled())
    {
        DisableAndDetachItem(SelectedInventoryItem);
        ReadyToUseId = -1;
        return;
    }

    /* Disable and detach any item that is currently in hands, then equip new item */
    for (const FPFInventoryItem& Item : InventoryItems)
    {
        if (!Item.ItemDataId.IsValid()) continue;

        if (Item.CachedItem == SelectedInventoryItem) continue;

        if (Item.CachedItem->GetIsItemEnabled())
        {
            DisableAndDetachItem(Item.CachedItem);
        }
    }

    USceneComponent* const HeldItemSceneComponent = CharacterOwner->GetHeldItemSceneComponent();
    if (!HeldItemSceneComponent) return;

    FAttachmentTransformRules AttachmentRules(EAttachmentRule::SnapToTarget, EAttachmentRule::SnapToTarget, EAttachmentRule::KeepWorld, true);
    SelectedInventoryItem->AttachToComponent(HeldItemSceneComponent, AttachmentRules);
    SelectedInventoryItem->SetIsItemEnabled(true);
    ReadyToUseId = ItemSlotId;
}

void UPFInventoryComponent::UseReadyToUseItem()
{
    APFBaseCharacter* const CharacterOwner = Cast<APFBaseCharacter>(GetOwner());
    if (!CharacterOwner) return;

    checkf(CharacterOwner->HasAuthority(), TEXT("Should only run on server!"))

    if (!IsReadyToUseIdValid()) return;

    if (!InventoryItems.IsValidIndex(ReadyToUseId)) return;

    if (!InventoryItems[ReadyToUseId].ItemDataId.IsValid()) return;

    APFBaseItem* const ReadyToUseItem = InventoryItems[ReadyToUseId].CachedItem;
    if (!ReadyToUseItem) return;

    ReadyToUseItem->UseItem();

    InvalidateInventorySlot(ReadyToUseId);
    /* Manual update for server copy */
    OnRep_InventoryItemsChanged();
    ReadyToUseId = -1;
}

void UPFInventoryComponent::DisableAndDetachItem(APFBaseItem* Item)
{
    if (!Item) return;

    FDetachmentTransformRules DetachmentRules(EDetachmentRule::KeepWorld, EDetachmentRule::KeepWorld, EDetachmentRule::KeepWorld, false);
    Item->DetachFromActor(DetachmentRules);
    Item->SetIsItemEnabled(false);
}

void UPFInventoryComponent::InvalidateInventorySlot(int32 SlotId)
{
    InventoryItems[SlotId] = FPFInventoryItem();
}

