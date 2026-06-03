#include "WaterGerstnerTypes.h"

void UWaterGerstnerWaveAsset::BuildBeaufortSpectrum(const int32 Seed, const int32 WaveCount, const float WindSpeedMetersPerSecond, const FVector2D WindDirection)
{
    Waves.Reset(FMath::Max(0, WaveCount));

    FRandomStream Stream(Seed);
    const FVector2D SafeWind = WindDirection.IsNearlyZero() ? FVector2D(1.0f, 0.0f) : WindDirection.GetSafeNormal();
    const float ClampedWind = FMath::Clamp(WindSpeedMetersPerSecond, 0.5f, 40.0f);
    const float BaseWavelength = FMath::Square(ClampedWind) * 5.5f;

    for (int32 WaveIndex = 0; WaveIndex < WaveCount; ++WaveIndex)
    {
        const float Fraction = WaveCount > 1 ? static_cast<float>(WaveIndex) / static_cast<float>(WaveCount - 1) : 0.0f;
        const float AngleJitter = Stream.FRandRange(-35.0f, 35.0f) * (1.0f - Fraction * 0.45f);
        const FVector2D Direction = SafeWind.GetRotated(AngleJitter).GetSafeNormal();

        FWaterGerstnerWave Wave;
        Wave.Direction = Direction;
        Wave.Wavelength = FMath::Max(80.0f, BaseWavelength * FMath::Lerp(0.25f, 2.2f, Fraction) * Stream.FRandRange(0.75f, 1.25f));
        Wave.Amplitude = FMath::Max(3.0f, Wave.Wavelength * FMath::Lerp(0.012f, 0.035f, Fraction) * Stream.FRandRange(0.7f, 1.35f));
        Wave.Steepness = FMath::Clamp(FMath::Lerp(0.72f, 0.34f, Fraction) * Stream.FRandRange(0.85f, 1.1f), 0.05f, 0.95f);
        Wave.Speed = FMath::Lerp(1.2f, 0.75f, Fraction) * FMath::Sqrt(ClampedWind);
        Wave.PhaseOffset = Stream.FRandRange(0.0f, UE_TWO_PI);
        Wave.FoamContribution = FMath::Lerp(0.4f, 1.4f, Fraction);
        Wave.BreakerInfluence = FMath::Lerp(1.2f, 0.55f, Fraction);
        Waves.Add(Wave);
    }
}
