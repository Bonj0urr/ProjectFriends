// Just a chill copyright notice

#pragma once

#include "CoreMinimal.h"
#include "Engine/StreamableManager.h"

class PROJECTFRIENDS_API PFResourceLoader
{
public:
    static TSharedPtr<FStreamableHandle> RequestAsyncLoad(const FSoftObjectPath& TargetToStream, FStreamableDelegate DelegateToCall = FStreamableDelegate());
};
