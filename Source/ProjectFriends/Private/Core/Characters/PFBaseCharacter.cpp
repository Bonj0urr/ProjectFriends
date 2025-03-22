// Just a chill copyright notice

#include "Core/Characters/PFBaseCharacter.h"
#include "EnhancedInputComponent.h"
#include "EnhancedInputSubsystems.h"
#include "InputActionValue.h"
#include "Engine/LocalPlayer.h"
#include "Camera/CameraComponent.h"
#include "Components/SceneComponent.h"
#include "Components/CapsuleComponent.h"
#include "GameFramework/CharacterMovementComponent.h"
#include "GameFramework/SpringArmComponent.h"
#include "GameFramework/Controller.h"
#include "PFBaseItem.h"
#include "PFInventoryComponent.h"
#include "Components/WidgetComponent.h"
#include "UMGs/PFInventoryWidget.h"
#include "Items/PFResourceLoader.h"

APFBaseCharacter::APFBaseCharacter()
    : InteractionTraceLength(900.0f)
{
	PrimaryActorTick.bCanEverTick = true;
    bReplicates = true;

    GetCapsuleComponent()->InitCapsuleSize(42.f, 96.0f);

    bUseControllerRotationPitch = false;
    bUseControllerRotationYaw = false;
    bUseControllerRotationRoll = false;

    GetCharacterMovement()->bOrientRotationToMovement = true;
    GetCharacterMovement()->RotationRate = FRotator(0.0f, 500.0f, 0.0f);

    GetCharacterMovement()->JumpZVelocity = 700.f;
    GetCharacterMovement()->AirControl = 0.35f;
    GetCharacterMovement()->MaxWalkSpeed = 500.f;
    GetCharacterMovement()->MinAnalogWalkSpeed = 20.f;
    GetCharacterMovement()->BrakingDecelerationWalking = 2000.f;
    GetCharacterMovement()->BrakingDecelerationFalling = 1500.0f;

    CameraBoom = CreateDefaultSubobject<USpringArmComponent>(TEXT("CameraBoom"));
    CameraBoom->SetupAttachment(RootComponent);
    CameraBoom->TargetArmLength = 400.0f;
    CameraBoom->bUsePawnControlRotation = true;

    FollowCamera = CreateDefaultSubobject<UCameraComponent>(TEXT("FollowCamera"));
    FollowCamera->SetupAttachment(CameraBoom, USpringArmComponent::SocketName);
    FollowCamera->bUsePawnControlRotation = false;

    HeldItemSceneComponent = CreateDefaultSubobject<USceneComponent>("HeldItemSceneComponent");
    HeldItemSceneComponent->SetupAttachment(RootComponent);

    InventoryWidgetComponent = CreateDefaultSubobject<UWidgetComponent>(TEXT("InventoryWidgetComponent"));
    InventoryWidgetComponent->SetupAttachment(RootComponent);

    InventoryComponent = CreateDefaultSubobject<UPFInventoryComponent>(TEXT("InventoryComponent"));
}

void APFBaseCharacter::BeginPlay()
{
	Super::BeginPlay();

    if (!IsValid(InventoryComponent)) return;

    if (!IsValid(InventoryWidgetComponent)) return;

    UPFInventoryWidget* const InventoryWidget = Cast<UPFInventoryWidget>(InventoryWidgetComponent->GetWidget());
    if (!IsValid(InventoryWidget)) return;

    InventoryComponent->OnInventoryItemsChanged.AddUObject(InventoryWidget, &UPFInventoryWidget::UpdateInventoryItems);
}

void APFBaseCharacter::Tick(float DeltaTime)
{
	Super::Tick(DeltaTime);
}

void APFBaseCharacter::Move(const FInputActionValue& Value)
{
    FVector2D MovementVector = Value.Get<FVector2D>();

    if (!Controller) return;

    const FRotator Rotation = Controller->GetControlRotation();
    const FRotator YawRotation(0, Rotation.Yaw, 0);

    const FVector ForwardDirection = FRotationMatrix(YawRotation).GetUnitAxis(EAxis::X);

    const FVector RightDirection = FRotationMatrix(YawRotation).GetUnitAxis(EAxis::Y);

    AddMovementInput(ForwardDirection, MovementVector.Y);
    AddMovementInput(RightDirection, MovementVector.X);
}

void APFBaseCharacter::Look(const FInputActionValue& Value)
{
    FVector2D LookAxisVector = Value.Get<FVector2D>();

    if (!Controller) return;

    AddControllerYawInput(LookAxisVector.X);
    AddControllerPitchInput(LookAxisVector.Y);
}

void APFBaseCharacter::CreateItem(const FInputActionValue& Value)
{
    if (!Value.Get<bool>()) return;

    Server_CreateItem();
}

void APFBaseCharacter::Interact(const FInputActionValue& Value)
{
    if (!Value.Get<bool>()) return;

    AActor* const HitActor = CreateInteractionTrace();
    if (!HitActor) return;

    Server_Interact();
}

void APFBaseCharacter::EquipItem(int32 ItemSlotNumber)
{
    if (!IsValid(InventoryComponent)) return;

    TArray<FPFInventoryItem> InventoryItems = InventoryComponent->GetInventoryItems();

    if (!InventoryItems.IsValidIndex(ItemSlotNumber)) return;

    if (!InventoryItems[ItemSlotNumber].ItemDataId.IsValid()) return;

    Server_EquipItem(ItemSlotNumber);
}

void APFBaseCharacter::Server_Interact_Implementation()
{
    AActor* const HitActor = CreateInteractionTrace();
    if (!HitActor) return;

    APFBaseItem* const HitItem = Cast<APFBaseItem>(HitActor);
    if (!HitItem) return;

    HitItem->HandleInteraction(this);
}

void APFBaseCharacter::Server_EquipItem_Implementation(int32 ItemSlotNumber)
{
    if (!IsValid(InventoryComponent)) return;

    InventoryComponent->EquipItem(ItemSlotNumber);
}

AActor* APFBaseCharacter::CreateInteractionTrace()
{
    UWorld* const World = GetWorld();
    if (!World) return nullptr;

    AController* const CharacterController = GetController();
    if (!CharacterController) return nullptr;

    FVector ViewLocation;
    FRotator ViewRotation;
    CharacterController->GetPlayerViewPoint(ViewLocation, ViewRotation);

    FVector InteractionTraceStart = ViewLocation;
    FVector InteractionTraceEnd = InteractionTraceStart + (ViewRotation.Vector() * InteractionTraceLength);

    FCollisionQueryParams TraceInteractionParams(FName(TEXT("InteractionTrace")), false, this);

    FHitResult HitResult;

    bool bHit = World->LineTraceSingleByChannel(HitResult, InteractionTraceStart, InteractionTraceEnd, ECC_GameTraceChannel1, 
        TraceInteractionParams, FCollisionResponseParams::DefaultResponseParam); /* InteractionChannel */

#ifdef UE_EDITOR
    DrawDebugLine(World, InteractionTraceStart, InteractionTraceEnd, FColor::Red, false, 2.0f);
#endif // UE_EDITOR

    return HitResult.GetActor();
}

void APFBaseCharacter::Server_CreateItem_Implementation()
{
    if (SpawnItemClasses.IsEmpty()) return;
    
    int32 SelectedItemClassIndex = FMath::RandRange(0, SpawnItemClasses.Num() - 1);

    UClass* const SelectedItemClass = SpawnItemClasses[SelectedItemClassIndex].Get();
    if (SelectedItemClass)
    {
        SpawnItem(SelectedItemClass);
    }
    else
    {
        /* Load selected item class async and spawn item from the loaded class */
        const FSoftObjectPath& SoftItemClassPath = SpawnItemClasses[SelectedItemClassIndex].ToSoftObjectPath();

        TSharedPtr<FStreamableHandle> LoadedItemClassHandle =
            PFResourceLoader::RequestAsyncLoad(SoftItemClassPath, FStreamableDelegate::CreateUObject(this, &ThisClass::SpawnItemAfterLoad, SelectedItemClassIndex));

        LoadedItemClassHandles.Add(LoadedItemClassHandle);
    }
}

void APFBaseCharacter::SpawnItem(UClass* ItemClass)
{
    checkf(HasAuthority(), TEXT("Should only run on server!"))

    if (!IsValid(ItemClass)) return;

    UWorld* const World = GetWorld();
    if (!World) return;

    FTransform SpawnedItemTransform = GetActorTransform();
    float SpawnDistance = 150.0f;
    SpawnedItemTransform.SetLocation(GetActorLocation() + GetActorForwardVector() * SpawnDistance);

    FActorSpawnParameters SpawnParameters;
    SpawnParameters.Owner = this;
    SpawnParameters.Instigator = this;
    SpawnParameters.SpawnCollisionHandlingOverride = ESpawnActorCollisionHandlingMethod::AdjustIfPossibleButAlwaysSpawn;

    APFBaseItem* const SpawnedItem = World->SpawnActor<APFBaseItem>(ItemClass, SpawnedItemTransform, SpawnParameters);
}

void APFBaseCharacter::SpawnItemAfterLoad(int32 ItemClassIndex)
{
    checkf(HasAuthority(), TEXT("Should only run on server!"))

    UClass* const SelectedItemClass = SpawnItemClasses[ItemClassIndex].Get();
    if (SelectedItemClass) SpawnItem(SelectedItemClass);
}

void APFBaseCharacter::SetupPlayerInputComponent(UInputComponent* PlayerInputComponent)
{
    Super::SetupPlayerInputComponent(PlayerInputComponent);
    
    UEnhancedInputComponent* const EnhancedInputComponent = Cast<UEnhancedInputComponent>(PlayerInputComponent);
    if (!(EnhancedInputComponent)) return;

    EnhancedInputComponent->BindAction(JumpAction, ETriggerEvent::Started, this, &ACharacter::Jump);
    EnhancedInputComponent->BindAction(JumpAction, ETriggerEvent::Completed, this, &ACharacter::StopJumping);

    EnhancedInputComponent->BindAction(MoveAction, ETriggerEvent::Triggered, this, &APFBaseCharacter::Move);

    EnhancedInputComponent->BindAction(LookAction, ETriggerEvent::Triggered, this, &APFBaseCharacter::Look);

    EnhancedInputComponent->BindAction(CreateItemAction, ETriggerEvent::Started, this, &APFBaseCharacter::CreateItem);

    EnhancedInputComponent->BindAction(InteractAction, ETriggerEvent::Started, this, &APFBaseCharacter::Interact);

    if (!IsValid(InventoryComponent)) return;
    
    checkf(InventoryComponent->GetInventorySize() == EquipItemActions.Num(), TEXT("InventorySize should be equal to EquipItemActions amount!"))

    for (int32 i = 0; i < EquipItemActions.Num(); i++)
    {
        EnhancedInputComponent->BindAction(EquipItemActions[i], ETriggerEvent::Started, this, &APFBaseCharacter::EquipItem, i);
    }
}

