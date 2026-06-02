#pragma once

#include "CoreMinimal.h"
#include "Components/SceneComponent.h"
#include "WaterBreakerComponent.generated.h"

UCLASS(ClassGroup = (Water), meta = (BlueprintSpawnableComponent))
class WATERGERSTNER_API UWaterBreakerComponent : public USceneComponent
{
    GENERATED_BODY()

public:
    UWaterBreakerComponent();
    virtual void BeginPlay() override;
    virtual void EndPlay(const EEndPlayReason::Type EndPlayReason) override;

    UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Water Breaker", meta = (ClampMin = "0.0"))
    float Radius = 1200.0f;
};
