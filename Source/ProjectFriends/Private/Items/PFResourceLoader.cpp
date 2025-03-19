// Just a chill copyright notice

#include "Items/PFResourceLoader.h"
#include "Engine/AssetManager.h"

TSharedPtr<FStreamableHandle> PFResourceLoader::RequestAsyncLoad(const FSoftObjectPath& TargetToStream, FStreamableDelegate DelegateToCall)
{
    if (UAssetManager::IsInitialized())
    {
        return UAssetManager::GetStreamableManager().RequestAsyncLoad(TargetToStream, DelegateToCall);
    }
    return TSharedPtr<FStreamableHandle>();
}
