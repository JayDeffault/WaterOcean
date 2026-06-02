#include "WaterGerstnerSubsystem.h"

#include "Engine/World.h"
#include "GerstnerWaterBody.h"
#include "Stats/Stats.h"

void UWaterGerstnerSubsystem::Tick(float DeltaTime)
{
    WaterBodies.RemoveAll([](const TObjectPtr<AGerstnerWaterBody>& Body) { return !IsValid(Body); });
}

TStatId UWaterGerstnerSubsystem::GetStatId() const
{
    RETURN_QUICK_DECLARE_CYCLE_STAT(UWaterGerstnerSubsystem, STATGROUP_Tickables);
}

bool UWaterGerstnerSubsystem::ShouldCreateSubsystem(UObject* Outer) const
{
    return true;
}

void UWaterGerstnerSubsystem::RegisterWaterBody(AGerstnerWaterBody* WaterBody)
{
    if (IsValid(WaterBody))
    {
        WaterBodies.AddUnique(WaterBody);
    }
}

void UWaterGerstnerSubsystem::UnregisterWaterBody(AGerstnerWaterBody* WaterBody)
{
    WaterBodies.Remove(WaterBody);
}

void UWaterGerstnerSubsystem::RegisterBreaker(const FVector& Location, const float Radius)
{
    FWaterBreakerSource Source;
    Source.Location = Location;
    Source.Radius = Radius;
    BreakerSources.Add(Source);
}

void UWaterGerstnerSubsystem::UnregisterBreaker(const FVector& Location)
{
    BreakerSources.RemoveAll([&Location](const FWaterBreakerSource& Source)
    {
        return Source.Location.Equals(Location, 1.0f);
    });
}

bool UWaterGerstnerSubsystem::QueryWaterSurface(const UObject* WorldContextObject, const FVector& WorldLocation, FWaterSurfaceSample& OutSample)
{
    if (!WorldContextObject)
    {
        return false;
    }

    const UWorld* World = WorldContextObject->GetWorld();
    UWaterGerstnerSubsystem* Subsystem = World ? World->GetSubsystem<UWaterGerstnerSubsystem>() : nullptr;
    AGerstnerWaterBody* Body = Subsystem ? Subsystem->FindBestWaterBody(WorldLocation) : nullptr;
    if (!Body)
    {
        return false;
    }

    OutSample = Body->SampleWaterSurface(WorldLocation, World->GetTimeSeconds());
    return true;
}

AGerstnerWaterBody* UWaterGerstnerSubsystem::FindBestWaterBody(const FVector& WorldLocation) const
{
    AGerstnerWaterBody* BestBody = nullptr;
    float BestDistanceSq = TNumericLimits<float>::Max();

    for (AGerstnerWaterBody* Body : WaterBodies)
    {
        if (!IsValid(Body) || !Body->IsInsideWaterBounds(WorldLocation))
        {
            continue;
        }

        const float DistanceSq = FVector::DistSquared2D(Body->GetActorLocation(), WorldLocation);
        if (DistanceSq < BestDistanceSq)
        {
            BestDistanceSq = DistanceSq;
            BestBody = Body;
        }
    }

    return BestBody;
}
