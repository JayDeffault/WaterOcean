#pragma once

#include "CoreMinimal.h"
#include "Subsystems/WorldSubsystem.h"
#include "WaterGerstnerTypes.h"
#include "WaterGerstnerSubsystem.generated.h"

class AGerstnerWaterBody;

UCLASS()
class WATERGERSTNER_API UWaterGerstnerSubsystem : public UTickableWorldSubsystem
{
    GENERATED_BODY()

public:
    virtual void Tick(float DeltaTime) override;
    virtual TStatId GetStatId() const override;
    virtual bool ShouldCreateSubsystem(UObject* Outer) const override;

    void RegisterWaterBody(AGerstnerWaterBody* WaterBody);
    void UnregisterWaterBody(AGerstnerWaterBody* WaterBody);
    void RegisterBreaker(const FVector& Location, float Radius);
    void UnregisterBreaker(const FVector& Location);

    UFUNCTION(BlueprintCallable, Category = "Water", meta = (WorldContext = "WorldContextObject"))
    static bool QueryWaterSurface(const UObject* WorldContextObject, const FVector& WorldLocation, FWaterSurfaceSample& OutSample);

    AGerstnerWaterBody* FindBestWaterBody(const FVector& WorldLocation) const;
    const TArray<FWaterBreakerSource>& GetBreakers() const { return BreakerSources; }

private:
    UPROPERTY(Transient)
    TArray<TObjectPtr<AGerstnerWaterBody>> WaterBodies;

    TArray<FWaterBreakerSource> BreakerSources;
};
