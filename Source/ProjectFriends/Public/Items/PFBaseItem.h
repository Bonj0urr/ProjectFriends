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

    FPrimaryAssetId GetItemDataPrimaryAssetId();

    FORCEINLINE UPFItemData* GetItemData() const { return ItemData; };

    void SetIsItemEnabled(bool IsEnabled);

    FORCEINLINE bool GetIsItemEnabled() const { return bIsItemEnabled; };

protected:
	virtual void BeginPlay() override;

private:
    UFUNCTION()
    void OnRep_IsItemEnabledChanged();

    void DisableItem();

    void EnableItem();

protected:
    UPROPERTY(VisibleAnywhere, BlueprintReadOnly, Category = "Mesh", meta = (AllowPrivateAccess = "true"))
    UStaticMeshComponent* StaticMeshComponent;

    UPROPERTY(EditDefaultsOnly, BlueprintReadOnly, Category = "Collision", meta = (AllowPrivateAccess = "true"))
    USphereComponent* SphereInteractionComponent;

private:
    UPROPERTY(EditDefaultsOnly, BlueprintReadOnly, Category = "Item", meta = (AllowPrivateAccess = "true"))
    UPFItemData* ItemData;

    UPROPERTY(ReplicatedUsing = OnRep_IsItemEnabledChanged)
    bool bIsItemEnabled;
};
