// Just a chill copyright notice

#include "Items/PFItemData.h"

FPrimaryAssetId UPFItemData::GetPrimaryAssetId() const
{
    return FPrimaryAssetId("Item", GetFName());
}
