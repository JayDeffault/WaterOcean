#pragma once

#include "CoreMinimal.h"
#include "GameFramework/Actor.h"
#include "WaterGerstnerTypes.h"
#include "GerstnerWaterBody.generated.h"

class UMaterialInstanceDynamic;
class UMaterialInterface;
class UStaticMeshComponent;

UCLASS(BlueprintType, Blueprintable)
class WATERGERSTNER_API AGerstnerWaterBody : public AActor
{
    GENERATED_BODY()

public:
    AGerstnerWaterBody();

    virtual void OnConstruction(const FTransform& Transform) override;
    virtual void BeginPlay() override;
    virtual void EndPlay(const EEndPlayReason::Type EndPlayReason) override;
    virtual void Tick(float DeltaSeconds) override;

    UPROPERTY(VisibleAnywhere, BlueprintReadOnly, Category = "Water")
    TObjectPtr<UStaticMeshComponent> WaterMesh;

    UPROPERTY(EditAnywhere, BlueprintReadOnly, Category = "Water")
    TObjectPtr<UWaterGerstnerWaveAsset> WaveAsset;

    UPROPERTY(EditAnywhere, BlueprintReadOnly, Category = "Rendering")
    TObjectPtr<UMaterialInterface> WaterMaterial;

    UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Rendering")
    FName TimeParameterName = TEXT("WaterTime");

    UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Rendering")
    FName FoamParameterName = TEXT("FoamIntensity");

    UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Interaction", meta = (ClampMin = "0.0"))
    float InteractionRippleStrength = 1.0f;

    UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Interaction", meta = (ClampMin = "0.0"))
    float InteractionRippleRadius = 650.0f;

    UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Sampling", meta = (ClampMin = "1", ClampMax = "12"))
    int32 SolverIterations = 4;

    UFUNCTION(BlueprintCallable, BlueprintPure = false, Category = "Water")
    FWaterSurfaceSample SampleWaterSurface(const FVector& WorldLocation, float WorldTime) const;

    UFUNCTION(BlueprintCallable, Category = "Water")
    bool IsInsideWaterBounds(const FVector& WorldLocation) const;

    UFUNCTION(BlueprintCallable, Category = "Water")
    void PushRipple(const FVector& WorldLocation, float Strength, float Radius);


private:
    UPROPERTY(Transient)
    TObjectPtr<UMaterialInstanceDynamic> DynamicWaterMaterial;


    void UpdateMaterialParameters(float WorldTime);
    float ComputeShoreMask(const FVector& WorldLocation) const;
    float ComputeBreakerMask(const FVector& WorldLocation) const;
};
