#pragma once

#include "CoreMinimal.h"
#include "Engine/DataAsset.h"
#include "WaterGerstnerTypes.generated.h"

USTRUCT(BlueprintType)
struct WATERGERSTNER_API FWaterGerstnerWave
{
    GENERATED_BODY()

    UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Wave", meta = (ClampMin = "0.0"))
    float Amplitude = 40.0f;

    UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Wave", meta = (ClampMin = "1.0"))
    float Wavelength = 1200.0f;

    UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Wave", meta = (ClampMin = "0.0", ClampMax = "1.0"))
    float Steepness = 0.55f;

    UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Wave")
    float Speed = 1.0f;

    UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Wave")
    float PhaseOffset = 0.0f;

    UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Wave")
    FVector2D Direction = FVector2D(1.0f, 0.0f);

    UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Foam", meta = (ClampMin = "0.0"))
    float FoamContribution = 1.0f;

    UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Breakers", meta = (ClampMin = "0.0"))
    float BreakerInfluence = 1.0f;
};

USTRUCT(BlueprintType)
struct WATERGERSTNER_API FWaterSurfaceSample
{
    GENERATED_BODY()

    UPROPERTY(BlueprintReadOnly, Category = "Water")
    FVector Position = FVector::ZeroVector;

    UPROPERTY(BlueprintReadOnly, Category = "Water")
    FVector Normal = FVector::UpVector;

    UPROPERTY(BlueprintReadOnly, Category = "Water")
    FVector Velocity = FVector::ZeroVector;

    UPROPERTY(BlueprintReadOnly, Category = "Water")
    float Height = 0.0f;

    UPROPERTY(BlueprintReadOnly, Category = "Water")
    float Foam = 0.0f;

    UPROPERTY(BlueprintReadOnly, Category = "Water")
    float ShoreMask = 0.0f;

    UPROPERTY(BlueprintReadOnly, Category = "Water")
    float BreakerMask = 0.0f;
};

USTRUCT(BlueprintType)
struct WATERGERSTNER_API FWaterShorelineSettings
{
    GENERATED_BODY()

    UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Shoreline", meta = (ClampMin = "0.0"))
    float ShorelineWidth = 2500.0f;

    UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Shoreline", meta = (ClampMin = "0.0"))
    float SurfBandWidth = 900.0f;

    UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Shoreline", meta = (ClampMin = "0.0"))
    float ShoalingHeightMultiplier = 1.6f;

    UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Shoreline", meta = (ClampMin = "0.0"))
    float ShoreFoamIntensity = 2.0f;

    UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Shoreline")
    bool bUseSignedDistanceToLandscape = true;
};

USTRUCT(BlueprintType)
struct WATERGERSTNER_API FWaterBreakerSettings
{
    GENERATED_BODY()

    UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Breakers", meta = (ClampMin = "0.0"))
    float RockBreakerRadius = 1200.0f;

    UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Breakers", meta = (ClampMin = "0.0"))
    float RockFoamIntensity = 3.0f;

    UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Breakers", meta = (ClampMin = "0.0"))
    float CurlAmount = 0.35f;

    UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Breakers", meta = (ClampMin = "0.0"))
    float Turbulence = 0.65f;
};

USTRUCT(BlueprintType)
struct WATERGERSTNER_API FWaterBreakerSource
{
    GENERATED_BODY()

    UPROPERTY(BlueprintReadOnly, Category = "Water")
    FVector Location = FVector::ZeroVector;

    UPROPERTY(BlueprintReadOnly, Category = "Water", meta = (ClampMin = "0.0"))
    float Radius = 1200.0f;
};

UCLASS(BlueprintType)
class WATERGERSTNER_API UWaterGerstnerWaveAsset : public UDataAsset
{
    GENERATED_BODY()

public:
    UPROPERTY(EditAnywhere, BlueprintReadOnly, Category = "Waves")
    TArray<FWaterGerstnerWave> Waves;

    UPROPERTY(EditAnywhere, BlueprintReadOnly, Category = "Waves", meta = (ClampMin = "0.0"))
    float GlobalAmplitude = 1.0f;

    UPROPERTY(EditAnywhere, BlueprintReadOnly, Category = "Waves", meta = (ClampMin = "0.0"))
    float GlobalSpeed = 1.0f;

    UPROPERTY(EditAnywhere, BlueprintReadOnly, Category = "Shoreline")
    FWaterShorelineSettings Shoreline;

    UPROPERTY(EditAnywhere, BlueprintReadOnly, Category = "Breakers")
    FWaterBreakerSettings Breakers;

    UFUNCTION(BlueprintCallable, Category = "Water")
    void BuildBeaufortSpectrum(int32 Seed, int32 WaveCount, float WindSpeedMetersPerSecond, FVector2D WindDirection);
};
