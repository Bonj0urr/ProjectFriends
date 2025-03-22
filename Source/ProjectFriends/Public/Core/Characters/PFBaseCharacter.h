// Just a chill copyright notice

#pragma once

#include "CoreMinimal.h"
#include "GameFramework/Character.h"
#include <EnhancedInputLibrary.h>
#include "Engine/StreamableManager.h" /* TO DO Remove it later after decoupling */
#include "PFBaseCharacter.generated.h"

class USpringArmComponent;
class UCameraComponent;
class UInputAction;
class APFBaseItem;
class UPFInventoryComponent;
class UWidgetComponent;
class USceneComponent;

UCLASS()
class PROJECTFRIENDS_API APFBaseCharacter : public ACharacter
{
	GENERATED_BODY()

public:
	APFBaseCharacter();

	virtual void Tick(float DeltaTime) override;

	virtual void SetupPlayerInputComponent(class UInputComponent* PlayerInputComponent) override;

    FORCEINLINE UPFInventoryComponent* GetInventoryComponent() const { return InventoryComponent; };

    FORCEINLINE USceneComponent* GetHeldItemSceneComponent() const { return HeldItemSceneComponent; }

protected:
	virtual void BeginPlay() override;

    void Move(const FInputActionValue& Value);

    void Look(const FInputActionValue& Value);

    void CreateItem(const FInputActionValue& Value);

    void Interact(const FInputActionValue& Value);

    void EquipItem(int32 ItemSlotNumber);

    UFUNCTION(Server, Reliable)
    void Server_CreateItem();

    UFUNCTION(Server, Reliable)
    void Server_Interact();

    UFUNCTION(Server, Reliable)
    void Server_EquipItem(int32 ItemSlotNumber);

private:
    AActor* CreateInteractionTrace();

    /* Helper method to spawn items from the class that already exists in memory */
    void SpawnItem(UClass* ItemClass);

    /* Helper method to spawn items from class that has been loaded by resource loader for the first time */
    UFUNCTION()
    void SpawnItemAfterLoad(int32 ItemClassIndex);

protected:
    UPROPERTY(VisibleAnywhere, BlueprintReadOnly, Category = "Components", meta = (AllowPrivateAccess = "true"))
    UPFInventoryComponent* InventoryComponent;

    UPROPERTY(EditDefaultsOnly, BlueprintReadOnly, Category = "Item", meta = (AllowPrivateAccess = "true"))
    TArray<TSoftClassPtr<APFBaseItem>> SpawnItemClasses;

    UPROPERTY(VisibleAnywhere, BlueprintReadOnly, Category = "Camera", meta = (AllowPrivateAccess = "true"))
    USpringArmComponent* CameraBoom;

    UPROPERTY(VisibleAnywhere, BlueprintReadOnly, Category = "Camera", meta = (AllowPrivateAccess = "true"))
    UCameraComponent* FollowCamera;

    UPROPERTY(VisibleAnywhere, BlueprintReadOnly, Category = "Components", meta = (AllowPrivateAccess = "true"))
    UWidgetComponent* InventoryWidgetComponent;

    UPROPERTY(VisibleAnywhere, BlueprintReadOnly, Category = "Components", meta = (AllowPrivateAccess = "true"))
    USceneComponent* HeldItemSceneComponent;

    UPROPERTY(EditDefaultsOnly, BlueprintReadOnly, Category = "Input", meta = (AllowPrivateAccess = "true"))
    UInputAction* JumpAction;

    UPROPERTY(EditDefaultsOnly, BlueprintReadOnly, Category = "Input", meta = (AllowPrivateAccess = "true"))
    UInputAction* MoveAction;

    UPROPERTY(EditDefaultsOnly, BlueprintReadOnly, Category = "Input", meta = (AllowPrivateAccess = "true"))
    UInputAction* LookAction;

    UPROPERTY(EditDefaultsOnly, BlueprintReadOnly, Category = "Input", meta = (AllowPrivateAccess = "true"))
    UInputAction* CreateItemAction;

    UPROPERTY(EditDefaultsOnly, BlueprintReadOnly, Category = "Input", meta = (AllowPrivateAccess = "true"))
    UInputAction* InteractAction;

    /** Insert EquipItemActions in the correct order (EquipItem1->EquipItem2->EquipItem3 etc.) */
    UPROPERTY(EditDefaultsOnly, BlueprintReadOnly, Category = "Input", meta = (AllowPrivateAccess = "true"))
    TArray<UInputAction*> EquipItemActions;

private:
    UPROPERTY(EditDefaultsOnly, BlueprintReadOnly, Category = "Interaction", meta = (AllowPrivateAccess = "true"))
    float InteractionTraceLength;

    /* TO DO decouple the storage of this handles from the character */
    TArray<TSharedPtr<FStreamableHandle>> LoadedItemClassHandles;
};
