// Just a chill copyright notice

#include "Core/Controllers/PFBasePlayerController.h"
#include "EnhancedInputComponent.h"
#include "EnhancedInputSubsystems.h"
#include "Engine/LocalPlayer.h"

APFBasePlayerController::APFBasePlayerController()
{
}

void APFBasePlayerController::BeginPlay()
{
    Super::BeginPlay();

    SetupInputComponent();
}

void APFBasePlayerController::SetupInputComponent()
{
    Super::SetupInputComponent();

    UEnhancedInputLocalPlayerSubsystem* const Subsystem = ULocalPlayer::GetSubsystem<UEnhancedInputLocalPlayerSubsystem>(GetLocalPlayer());
    if (Subsystem) Subsystem->AddMappingContext(DefaultMappingContext, 0);
}