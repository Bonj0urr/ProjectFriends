// Just a chill copyright notice

#pragma once

#include "CoreMinimal.h"
#include "GameFramework/PlayerController.h"
#include "PFBasePlayerController.generated.h"

class UInputMappingContext;

UCLASS()
class PROJECTFRIENDS_API APFBasePlayerController : public APlayerController
{
	GENERATED_BODY()

public:
    APFBasePlayerController();

protected:
    virtual void SetupInputComponent() override;

    virtual void BeginPlay() override;

protected:
    UPROPERTY(EditDefaultsOnly, BlueprintReadOnly, Category = "PF|Input", meta = (AllowPrivateAccess = "true"))
    UInputMappingContext* DefaultMappingContext;
};
