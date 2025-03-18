// Just a chill copyright notice

#include "Items/PFBaseItem.h"
#include "Components/StaticMeshComponent.h"
#include "Components/SphereComponent.h"
#include "Characters/PFBaseCharacter.h"
#include "PFInventoryComponent.h"

APFBaseItem::APFBaseItem()
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

void APFBaseItem::DisableItem()
{
    UPrimitiveComponent* const RootPrimitive = Cast<UPrimitiveComponent>(GetRootComponent());
    if (RootPrimitive)
    {
        RootPrimitive->SetSimulatePhysics(false);
        RootPrimitive->SetCollisionEnabled(ECollisionEnabled::NoCollision);
    }

    SetActorHiddenInGame(true);
}

void APFBaseItem::EnableItem()
{
    UPrimitiveComponent* const RootPrimitive = Cast<UPrimitiveComponent>(GetRootComponent());
    if (RootPrimitive)
    {
        RootPrimitive->SetSimulatePhysics(true);
        RootPrimitive->SetCollisionEnabled(ECollisionEnabled::QueryAndPhysics);
    }

    SetActorHiddenInGame(false);
}

void APFBaseItem::BeginPlay()
{
    Super::BeginPlay();

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

