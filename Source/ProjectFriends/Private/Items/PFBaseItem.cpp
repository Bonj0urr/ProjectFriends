// Just a chill copyright notice

#include "Items/PFBaseItem.h"
#include "Net/UnrealNetwork.h"
#include "Components/StaticMeshComponent.h"
#include "Components/SphereComponent.h"
#include "Characters/PFBaseCharacter.h"
#include "PFInventoryComponent.h"

APFBaseItem::APFBaseItem()
    : ItemData(nullptr), bIsItemEnabled(true)
{
    bReplicates = true;
	PrimaryActorTick.bCanEverTick = false;

    StaticMeshComponent = CreateDefaultSubobject<UStaticMeshComponent>(TEXT("StaticMeshComponent"));
    StaticMeshComponent->SetupAttachment(RootComponent);

    float SphereInteractionRadius = 50.0f;
    SphereInteractionComponent = CreateDefaultSubobject<USphereComponent>(TEXT("SphereInteractionComponent"));
    SphereInteractionComponent->SetupAttachment(StaticMeshComponent);
    SphereInteractionComponent->SetCollisionEnabled(ECollisionEnabled::QueryOnly);
    SphereInteractionComponent->SetCollisionResponseToAllChannels(ECR_Ignore);
    SphereInteractionComponent->SetCollisionResponseToChannel(
        ECollisionChannel::ECC_GameTraceChannel1, ECollisionResponse::ECR_Block); /* InteractionChannel */
    SphereInteractionComponent->SetSphereRadius(SphereInteractionRadius);
}

void APFBaseItem::BeginPlay()
{
    Super::BeginPlay();
}

void APFBaseItem::GetLifetimeReplicatedProps(TArray<FLifetimeProperty>& OutLifetimeProps) const
{
    Super::GetLifetimeReplicatedProps(OutLifetimeProps);

    DOREPLIFETIME(APFBaseItem, bIsItemEnabled);
}

void APFBaseItem::SetIsItemEnabled(bool IsEnabled)
{
    checkf(HasAuthority(), TEXT("Should only run on server!"))

    bIsItemEnabled = IsEnabled;
    /* Manual update for server copy */
    OnRep_IsItemEnabledChanged();
}

void APFBaseItem::OnRep_IsItemEnabledChanged()
{
    if (bIsItemEnabled)
    {
        EnableItem();
    }
    else
    {
        DisableItem();
    }
}

void APFBaseItem::DisableItem()
{
    SetActorEnableCollision(false);
    SetActorHiddenInGame(true);
}

void APFBaseItem::EnableItem()
{
    SetActorEnableCollision(true);
    SetActorHiddenInGame(false);
}

void APFBaseItem::HandleInteraction(APFBaseCharacter* const Character)
{
    checkf(HasAuthority(), TEXT("Should only run on server!"))

    if (!Character) return;

    UPFInventoryComponent* const InventoryComponent = Character->GetInventoryComponent();
    if (!InventoryComponent) return;

    InventoryComponent->TryAddItem(this);
}

FPrimaryAssetId APFBaseItem::GetItemDataPrimaryAssetId()
{
    if (!IsValid(ItemData)) return FPrimaryAssetId();

    return ItemData->GetPrimaryAssetId();
}

