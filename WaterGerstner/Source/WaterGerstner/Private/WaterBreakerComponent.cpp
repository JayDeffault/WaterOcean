#include "WaterBreakerComponent.h"

#include "WaterGerstnerSubsystem.h"

UWaterBreakerComponent::UWaterBreakerComponent()
{
    PrimaryComponentTick.bCanEverTick = false;
}

void UWaterBreakerComponent::BeginPlay()
{
    Super::BeginPlay();
    if (UWorld* World = GetWorld())
    {
        if (UWaterGerstnerSubsystem* Subsystem = World->GetSubsystem<UWaterGerstnerSubsystem>())
        {
            Subsystem->RegisterBreaker(GetComponentLocation(), Radius);
        }
    }
}

void UWaterBreakerComponent::EndPlay(const EEndPlayReason::Type EndPlayReason)
{
    if (UWorld* World = GetWorld())
    {
        if (UWaterGerstnerSubsystem* Subsystem = World->GetSubsystem<UWaterGerstnerSubsystem>())
        {
            Subsystem->UnregisterBreaker(GetComponentLocation());
        }
    }
    Super::EndPlay(EndPlayReason);
}
