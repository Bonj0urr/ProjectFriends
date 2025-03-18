// Just a chill copyright notice

#include "UMGs/PFInventoryWidget.h"
#include "Engine/AssetManager.h"
#include "Components/Image.h"
#include "PFInventoryComponent.h"
#include "Items/PFItemData.h"

void UPFInventoryWidget::NativeConstruct()
{
    Super::NativeConstruct();

    InventoryItemImages.Add(InventoryItem1);
    InventoryItemImages.Add(InventoryItem2);
    InventoryItemImages.Add(InventoryItem3);
}

void UPFInventoryWidget::UpdateInventoryItems(TArray<FPFInventoryItem> NewInventoryItems)
{
    UAssetManager* const AssetManager = UAssetManager::GetIfInitialized();
    if (!IsValid(AssetManager)) return;

    TArray<FName> Bundles;

    for (int32 i = 0; i < NewInventoryItems.Num(); i++)
    {
        FStreamableDelegate Delegate = FStreamableDelegate::CreateUObject(this, &UPFInventoryWidget::UpdateInventoryItemImage, 
            NewInventoryItems[i].ItemDataId, InventoryItemImages[i]);

        AssetManager->LoadPrimaryAsset(NewInventoryItems[i].ItemDataId, Bundles, Delegate);
    }
}

void UPFInventoryWidget::UpdateInventoryItemImage(FPrimaryAssetId LoadedId, UImage* InventoryItemImage)
{
    UAssetManager* const AssetManager = UAssetManager::GetIfInitialized();
    if (!IsValid(AssetManager)) return;

    UPFItemData* const ItemData = Cast<UPFItemData>(AssetManager->GetPrimaryAssetObject(LoadedId));
    if (!IsValid(ItemData)) return;

    if (!IsValid(InventoryItemImage)) return;

    InventoryItemImage->SetBrushFromTexture(ItemData->Icon, false);

    FVector2D NewBrushSize;
    NewBrushSize.X = ItemData->Icon->GetSizeX();
    NewBrushSize.Y = ItemData->Icon->GetSizeY();

    InventoryItemImage->SetDesiredSizeOverride(NewBrushSize);
}

