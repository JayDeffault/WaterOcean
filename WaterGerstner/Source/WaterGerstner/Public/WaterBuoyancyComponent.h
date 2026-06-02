#pragma once

#include "CoreMinimal.h"
#include "Components/ActorComponent.h"
#include "WaterBuoyancyComponent.generated.h"

class UPrimitiveComponent;

USTRUCT(BlueprintType)
struct WATERGERSTNER_API FWaterPontoon
{
    GENERATED_BODY()

    UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Pontoon")
    FVector LocalOffset = FVector::ZeroVector;

    UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Pontoon", meta = (ClampMin = "1.0"))
    float Radius = 45.0f;

    UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Pontoon", meta = (ClampMin = "0.0"))
    float Buoyancy = 1.0f;

    UPROPERTY(BlueprintReadOnly, Category = "Pontoon")
    float Immersion = 0.0f;
};

UCLASS(ClassGroup = (Water), meta = (BlueprintSpawnableComponent))
class WATERGERSTNER_API UWaterBuoyancyComponent : public UActorComponent
{
    GENERATED_BODY()

public:
    UWaterBuoyancyComponent();
    virtual void BeginPlay() override;
    virtual void TickComponent(float DeltaTime, ELevelTick TickType, FActorComponentTickFunction* ThisTickFunction) override;

    UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Buoyancy")
    TArray<FWaterPontoon> Pontoons;

    UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Buoyancy")
    TObjectPtr<UPrimitiveComponent> SimulatedComponent;

    UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Buoyancy", meta = (ClampMin = "0.0"))
    float FluidDensity = 1025.0f;

    UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Buoyancy", meta = (ClampMin = "0.0"))
    float LinearWaterDrag = 1.8f;

    UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Buoyancy", meta = (ClampMin = "0.0"))
    float AngularWaterDrag = 0.35f;

    UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Buoyancy", meta = (ClampMin = "0.0"))
    float VelocityMatch = 0.45f;

    UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Buoyancy")
    bool bAutoCreatePontoonsFromBounds = true;

    UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Debug")
    bool bDrawDebug = false;

private:
    void ResolveSimulatedComponent();
    void AutoCreatePontoons();
};
