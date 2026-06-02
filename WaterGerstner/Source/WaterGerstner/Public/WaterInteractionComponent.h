#pragma once

#include "CoreMinimal.h"
#include "Components/ActorComponent.h"
#include "WaterInteractionComponent.generated.h"

UCLASS(ClassGroup = (Water), meta = (BlueprintSpawnableComponent))
class WATERGERSTNER_API UWaterInteractionComponent : public UActorComponent
{
    GENERATED_BODY()

public:
    UWaterInteractionComponent();
    virtual void TickComponent(float DeltaTime, ELevelTick TickType, FActorComponentTickFunction* ThisTickFunction) override;

    UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Interaction", meta = (ClampMin = "0.0"))
    float MinimumSpeedForRipple = 20.0f;

    UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Interaction", meta = (ClampMin = "0.0"))
    float RippleStrengthScale = 0.02f;

    UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Interaction", meta = (ClampMin = "0.0"))
    float RippleRadius = 500.0f;

private:
    FVector PreviousLocation = FVector::ZeroVector;
    bool bHasPreviousLocation = false;
};
