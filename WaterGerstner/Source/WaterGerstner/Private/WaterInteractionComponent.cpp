#include "WaterInteractionComponent.h"

#include "GameFramework/Actor.h"
#include "GerstnerWaterBody.h"
#include "WaterGerstnerSubsystem.h"

UWaterInteractionComponent::UWaterInteractionComponent()
{
    PrimaryComponentTick.bCanEverTick = true;
}

void UWaterInteractionComponent::TickComponent(const float DeltaTime, const ELevelTick TickType, FActorComponentTickFunction* ThisTickFunction)
{
    Super::TickComponent(DeltaTime, TickType, ThisTickFunction);

    if (DeltaTime <= UE_SMALL_NUMBER || !GetOwner())
    {
        return;
    }

    const FVector CurrentLocation = GetOwner()->GetActorLocation();
    if (!bHasPreviousLocation)
    {
        PreviousLocation = CurrentLocation;
        bHasPreviousLocation = true;
        return;
    }

    const float Speed = FVector::Dist(CurrentLocation, PreviousLocation) / DeltaTime;
    if (Speed >= MinimumSpeedForRipple)
    {
        UWaterGerstnerSubsystem* Subsystem = GetWorld() ? GetWorld()->GetSubsystem<UWaterGerstnerSubsystem>() : nullptr;
        AGerstnerWaterBody* Body = Subsystem ? Subsystem->FindBestWaterBody(CurrentLocation) : nullptr;
        if (Body)
        {
            Body->PushRipple(CurrentLocation, Speed * RippleStrengthScale, RippleRadius);
        }
    }

    PreviousLocation = CurrentLocation;
}
