// Just a chill copyright notice

#include "Subsystems/PFNetworkSubsystem.h"
#include "Engine/World.h"
#include "Kismet/GameplayStatics.h"
#include "Items/PFBaseItem.h"

void UPFNetworkSubsystem::SetNetUpdateFrequencyForItems(float NewFrequency)
{
    UWorld* const World = GetWorld();
    if (!World) return;

    if (!World->GetAuthGameMode())
    {
        UE_LOG(LogTemp, Warning, TEXT("Net Update Frequency can only be set on the server"));
        return;
    }

    float ClampedFrequency = FMath::Clamp(NewFrequency, 1.0f, 100.0f);

    TArray<AActor*> FoundActors;
    UGameplayStatics::GetAllActorsOfClass(World, APFBaseItem::StaticClass(), FoundActors);

    for (AActor* const Actor : FoundActors)
    {
        if (!Actor) continue;

        Actor->NetUpdateFrequency = ClampedFrequency;

        UE_LOG(LogTemp, Warning, TEXT("Set Net Update Frequency to %f for Actor: %s"),
            ClampedFrequency, *Actor->GetName());
    }
}

static void SetNetUpdateFrequencyForItemsCommand(const TArray<FString>& Args, UWorld* World)
{
    if (Args.Num() < 1)
    {
        UE_LOG(LogTemp, Warning, TEXT("Usage: SetNetUpdateFrequencyForClass <frequency>"));
        return;
    }

    float NewFrequency = FCString::Atof(*Args[0]);

    if (!World) return;

    UGameInstance* const GameInstance = UGameplayStatics::GetGameInstance(World);
    if (!GameInstance) return;

    UPFNetworkSubsystem* const NetworkSubsystem = GameInstance->GetSubsystem<UPFNetworkSubsystem>();
    if (!NetworkSubsystem) return;

    NetworkSubsystem->SetNetUpdateFrequencyForItems(NewFrequency);
}

static FAutoConsoleCommandWithWorldAndArgs SetNetUpdateFrequencyForItemsCmd(
    TEXT("cmd.SetNetUpdateFrequencyForItems"),
    TEXT("Change the net update frequency for all items on the server"),
    FConsoleCommandWithWorldAndArgsDelegate::CreateStatic(&SetNetUpdateFrequencyForItemsCommand)
);
