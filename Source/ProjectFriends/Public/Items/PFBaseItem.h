// Just a chill copyright notice

#pragma once

#include "CoreMinimal.h"
#include "GameFramework/Actor.h"
#include "Items/PFItemData.h"
#include "PFBaseItem.generated.h"

class UStaticMeshComponent;
class USphereComponent;
class APFBaseCharacter;

UCLASS(Abstract)
class PROJECTFRIENDS_API APFBaseItem : public AActor
{
	GENERATED_BODY()
	
public:	
	APFBaseItem();

    virtual void GetLifetimeReplicatedProps(TArray<FLifetimeProperty>& OutLifetimeProps) const override;

    void HandleInteraction(APFBaseCharacter* const Character);

    FORCEINLINE FPrimaryAssetId GetItemDataPrimaryAssetId() const { return ItemDataPrimaryAssetId; }

    void SetIsItemEnabled(bool IsEnabled);

    void SetIsItemPhysicsSimulated(bool IsSimulated);

    FORCEINLINE bool GetIsItemEnabled() const { return bIsItemEnabled; };

    virtual void UseItem();

protected:
	virtual void BeginPlay() override;

private:
    UFUNCTION()
    void OnRep_IsItemEnabledChanged();

    UFUNCTION()
    void OnRep_IsItemPhysicsSimulatedChanged();

    void DisableItem();

    void EnableItem();

protected:
    UPROPERTY(VisibleAnywhere, BlueprintReadOnly, Category = "Mesh", meta = (AllowPrivateAccess = "true"))
    UStaticMeshComponent* StaticMeshComponent;

    UPROPERTY(EditDefaultsOnly, BlueprintReadOnly, Category = "Collision", meta = (AllowPrivateAccess = "true"))
    USphereComponent* SphereInteractionComponent;

private:
    UPROPERTY(EditDefaultsOnly, BlueprintReadOnly, Category = "Item", meta = (AllowPrivateAccess = "true"))
    FPrimaryAssetId ItemDataPrimaryAssetId;

    UPROPERTY(ReplicatedUsing = OnRep_IsItemEnabledChanged)
    bool bIsItemEnabled;

    UPROPERTY(ReplicatedUsing = OnRep_IsItemPhysicsSimulatedChanged)
    bool bIsItemPhysicsSimulated;
};
