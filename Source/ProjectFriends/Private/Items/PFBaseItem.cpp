// Just a chill copyright notice

#include "Items/PFBaseItem.h"
#include "Net/UnrealNetwork.h"
#include "Components/StaticMeshComponent.h"
#include "Components/SphereComponent.h"
#include "Characters/PFBaseCharacter.h"
#include "PFInventoryComponent.h"

APFBaseItem::APFBaseItem()
    : bIsItemEnabled(true), bIsItemPhysicsSimulated(false)
{
    bReplicates = true;
	PrimaryActorTick.bCanEverTick = false;
    SetReplicateMovement(false);
    SetPhysicsReplicationMode(EPhysicsReplicationMode::PredictiveInterpolation);

    StaticMeshComponent = CreateDefaultSubobject<UStaticMeshComponent>(TEXT("StaticMeshComponent"));
    StaticMeshComponent->SetupAttachment(RootComponent);
    StaticMeshComponent->SetSimulatePhysics(false);

    float SphereInteractionRadius = 50.0f;
    SphereInteractionComponent = CreateDefaultSubobject<USphereComponent>(TEXT("SphereInteractionComponent"));
    SphereInteractionComponent->SetupAttachment(StaticMeshComponent);
    SphereInteractionComponent->SetCollisionEnabled(ECollisionEnabled::QueryOnly);
    SphereInteractionComponent->SetCollisionResponseToAllChannels(ECR_Ignore);
    SphereInteractionComponent->SetCollisionResponseToChannel(
        ECollisionChannel::ECC_GameTraceChannel1, ECollisionResponse::ECR_Block); /* InteractionChannel */
    SphereInteractionComponent->SetSphereRadius(SphereInteractionRadius);
}

void APFBaseItem::UseItem()
{
    checkf(HasAuthority(), TEXT("Should only run on server!"))

    AActor* const OwnerActor = GetOwner();
    if (!OwnerActor) return;

    FDetachmentTransformRules DetachmentRules(EDetachmentRule::KeepWorld, EDetachmentRule::KeepWorld, EDetachmentRule::KeepWorld, false);
    DetachFromActor(DetachmentRules);

    SetReplicateMovement(true);
    SetIsItemPhysicsSimulated(true);

    FVector Direction = OwnerActor->GetActorForwardVector();
    float ImpulseStrength = 1000.0f;
    StaticMeshComponent->AddImpulse(Direction * ImpulseStrength, NAME_None, true);

    SetOwner(nullptr);
}

void APFBaseItem::BeginPlay()
{
    Super::BeginPlay();
}

void APFBaseItem::GetLifetimeReplicatedProps(TArray<FLifetimeProperty>& OutLifetimeProps) const
{
    Super::GetLifetimeReplicatedProps(OutLifetimeProps);

    DOREPLIFETIME(APFBaseItem, bIsItemEnabled);
    DOREPLIFETIME(APFBaseItem, bIsItemPhysicsSimulated);
}

void APFBaseItem::SetIsItemEnabled(bool IsEnabled)
{
    checkf(HasAuthority(), TEXT("Should only run on server!"))

    bIsItemEnabled = IsEnabled;
    /* Manual update for server copy */
    OnRep_IsItemEnabledChanged();

    if(!IsEnabled) SetReplicateMovement(false);
}

void APFBaseItem::SetIsItemPhysicsSimulated(bool IsSimulated)
{
    checkf(HasAuthority(), TEXT("Should only run on server!"))

    bIsItemPhysicsSimulated = IsSimulated;
    /* Manual update for server copy */
    OnRep_IsItemPhysicsSimulatedChanged();
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

void APFBaseItem::OnRep_IsItemPhysicsSimulatedChanged()
{
    StaticMeshComponent->SetSimulatePhysics(bIsItemPhysicsSimulated);
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
