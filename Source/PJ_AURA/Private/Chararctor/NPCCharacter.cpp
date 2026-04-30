// ALL CODE FOR  learning GAS


#include "Chararctor/NPCCharacter.h"
#include "Talk//NPCInteractionComponent.h"
#include "Components/SphereComponent.h"
#include "Components/WidgetComponent.h"
#include "Components/TextBlock.h"
#include "Kismet/GameplayStatics.h"
#include "DrawDebugHelpers.h"
#include "PJ_AURA/PJ_AURA.h"

ANPCCharacter::ANPCCharacter()
{
    // Set default values
    PrimaryActorTick.bCanEverTick = true;

    // Create components
    NPCInteractionComponent = CreateDefaultSubobject<UNPCInteractionComponent>(TEXT("NPCInteractionComponent"));
    NameTagWidget = CreateDefaultSubobject<UWidgetComponent>(TEXT("NameTagWidget"));

    // Setup components
    SetupComponents();
}

void ANPCCharacter::BeginPlay()
{
    Super::BeginPlay();
}

void ANPCCharacter::Tick(float DeltaTime)
{
    Super::Tick(DeltaTime);

    // Draw debug visuals if enabled
}

void ANPCCharacter::GetMiniMapIcon_Implementation(UTexture2D*& OutMiniMapIcon, bool& OutIsPermanent,
	bool& OutShouldRemoveIcon, bool& OutIgnoreMapRotation)
{
    OutMiniMapIcon = MiniMapIcon;
    OutShouldRemoveIcon = false;
}

void ANPCCharacter::GetMiniMapLocationAndRotation_Implementation(FVector& OutLocation, FRotator& OutRotation)
{
    OutLocation = GetActorLocation();
    OutRotation = GetActorRotation();
}

void ANPCCharacter::HighlightActor()
{
    GetMesh()->SetRenderCustomDepth(true);
    GetMesh()->SetCustomDepthStencilValue(CUSTOM_DEPTH_GREEN);
}

void ANPCCharacter::UnHighlightActor()
{
    GetMesh()->SetRenderCustomDepth(false);
}


// Helper functions
void ANPCCharacter::SetupComponents()
{

    // Setup Name Tag Widget
    if (NameTagWidget)
    {
        NameTagWidget->SetupAttachment(RootComponent);
    }
}
