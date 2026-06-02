#include "GerstnerWaterBody.h"

#include "Components/StaticMeshComponent.h"
#include "DrawDebugHelpers.h"
#include "Engine/World.h"
#include "Materials/MaterialInstanceDynamic.h"
#include "WaterGerstnerSubsystem.h"

namespace
{
float WaterSmoothStep(const float Edge0, const float Edge1, const float Value)
{
    const float X = FMath::Clamp((Value - Edge0) / FMath::Max(Edge1 - Edge0, UE_SMALL_NUMBER), 0.0f, 1.0f);
    return X * X * (3.0f - 2.0f * X);
}
}

AGerstnerWaterBody::AGerstnerWaterBody()
{
    PrimaryActorTick.bCanEverTick = true;
    WaterMesh = CreateDefaultSubobject<UStaticMeshComponent>(TEXT("WaterMesh"));
    SetRootComponent(WaterMesh);
    WaterMesh->SetCollisionEnabled(ECollisionEnabled::QueryOnly);
    WaterMesh->SetCollisionResponseToAllChannels(ECR_Ignore);
    WaterMesh->SetCollisionResponseToChannel(ECC_WorldDynamic, ECR_Overlap);
    WaterMesh->SetGenerateOverlapEvents(false);
}

void AGerstnerWaterBody::OnConstruction(const FTransform& Transform)
{
    Super::OnConstruction(Transform);

    if (WaterMaterial)
    {
        DynamicWaterMaterial = WaterMesh->CreateDynamicMaterialInstance(0, WaterMaterial);
    }
}

void AGerstnerWaterBody::BeginPlay()
{
    Super::BeginPlay();

    if (WaterMaterial && !DynamicWaterMaterial)
    {
        DynamicWaterMaterial = WaterMesh->CreateDynamicMaterialInstance(0, WaterMaterial);
    }

    if (UWaterGerstnerSubsystem* Subsystem = GetWorld()->GetSubsystem<UWaterGerstnerSubsystem>())
    {
        Subsystem->RegisterWaterBody(this);
    }
}

void AGerstnerWaterBody::EndPlay(const EEndPlayReason::Type EndPlayReason)
{
    if (UWorld* World = GetWorld())
    {
        if (UWaterGerstnerSubsystem* Subsystem = World->GetSubsystem<UWaterGerstnerSubsystem>())
        {
            Subsystem->UnregisterWaterBody(this);
        }
    }

    Super::EndPlay(EndPlayReason);
}

void AGerstnerWaterBody::Tick(float DeltaSeconds)
{
    Super::Tick(DeltaSeconds);
    UpdateMaterialParameters(GetWorld() ? GetWorld()->GetTimeSeconds() : 0.0f);
}

FWaterSurfaceSample AGerstnerWaterBody::SampleWaterSurface(const FVector& WorldLocation, const float WorldTime) const
{
    FWaterSurfaceSample Sample;
    Sample.Position = FVector(WorldLocation.X, WorldLocation.Y, GetActorLocation().Z);
    Sample.Height = Sample.Position.Z;

    if (!WaveAsset || WaveAsset->Waves.IsEmpty())
    {
        return Sample;
    }

    const float ShoreMask = ComputeShoreMask(WorldLocation);
    const float BreakerMask = ComputeBreakerMask(WorldLocation);
    const float ShoalMultiplier = FMath::Lerp(1.0f, WaveAsset->Shoreline.ShoalingHeightMultiplier, ShoreMask);
    const float BreakerCurl = WaveAsset->Breakers.CurlAmount * BreakerMask;
    const int32 WaveCount = WaveAsset->Waves.Num();

    auto EvaluateAt = [&](const FVector& EvaluationLocation, FVector& OutHorizontalOffset, float& OutHeightOffset, FVector& OutNormal, FVector& OutVelocity, float& OutFoam)
    {
        OutHorizontalOffset = FVector::ZeroVector;
        OutHeightOffset = 0.0f;
        OutNormal = FVector::UpVector;
        OutVelocity = FVector::ZeroVector;
        OutFoam = 0.0f;

        for (const FWaterGerstnerWave& Wave : WaveAsset->Waves)
        {
            const FVector2D Direction2D = Wave.Direction.IsNearlyZero() ? FVector2D(1.0f, 0.0f) : Wave.Direction.GetSafeNormal();
            const FVector Direction(Direction2D.X, Direction2D.Y, 0.0f);
            const float Wavelength = FMath::Max(1.0f, Wave.Wavelength);
            const float WaveNumber = UE_TWO_PI / Wavelength;
            const float AngularFrequency = FMath::Sqrt(980.0f * WaveNumber) * Wave.Speed * WaveAsset->GlobalSpeed;
            const float Amplitude = Wave.Amplitude * WaveAsset->GlobalAmplitude * ShoalMultiplier;
            const float Steepness = FMath::Clamp(Wave.Steepness + BreakerCurl * Wave.BreakerInfluence, 0.0f, 0.98f);
            const float Qi = Steepness / FMath::Max(1, WaveCount);
            const float Phase = WaveNumber * FVector::DotProduct(Direction, EvaluationLocation) - AngularFrequency * WorldTime + Wave.PhaseOffset;
            const float SinPhase = FMath::Sin(Phase);
            const float CosPhase = FMath::Cos(Phase);

            OutHorizontalOffset += Direction * (Qi * Amplitude * CosPhase);
            OutHeightOffset += Amplitude * SinPhase;
            OutNormal.X -= Direction.X * WaveNumber * Amplitude * CosPhase;
            OutNormal.Y -= Direction.Y * WaveNumber * Amplitude * CosPhase;
            OutNormal.Z -= Qi * WaveNumber * Amplitude * SinPhase;
            OutVelocity += FVector(Direction.X * Qi * Amplitude * AngularFrequency * SinPhase,
                                   Direction.Y * Qi * Amplitude * AngularFrequency * SinPhase,
                                   -Amplitude * AngularFrequency * CosPhase);

            const float Crest = WaterSmoothStep(0.62f, 0.97f, SinPhase);
            OutFoam += Crest * Wave.FoamContribution * FMath::Max(ShoreMask * WaveAsset->Shoreline.ShoreFoamIntensity, BreakerMask * WaveAsset->Breakers.RockFoamIntensity);
        }
    };

    FVector EvaluationLocation = WorldLocation;
    FVector HorizontalOffset = FVector::ZeroVector;
    FVector NormalAccumulator = FVector::UpVector;
    FVector Velocity = FVector::ZeroVector;
    float HeightOffset = 0.0f;
    float Foam = 0.0f;

    for (int32 Iteration = 0; Iteration < FMath::Max(1, SolverIterations); ++Iteration)
    {
        EvaluateAt(EvaluationLocation, HorizontalOffset, HeightOffset, NormalAccumulator, Velocity, Foam);
        EvaluationLocation.X = WorldLocation.X - HorizontalOffset.X;
        EvaluationLocation.Y = WorldLocation.Y - HorizontalOffset.Y;
    }

    EvaluateAt(EvaluationLocation, HorizontalOffset, HeightOffset, NormalAccumulator, Velocity, Foam);

    Sample.Position = FVector(WorldLocation.X, WorldLocation.Y, GetActorLocation().Z) + HorizontalOffset;
    Sample.Position.Z += HeightOffset;
    Sample.Height = Sample.Position.Z;
    Sample.Normal = NormalAccumulator.GetSafeNormal(FVector::UpVector);
    Sample.Velocity = Velocity;
    Sample.Foam = FMath::Clamp(Foam, 0.0f, 8.0f);
    Sample.ShoreMask = ShoreMask;
    Sample.BreakerMask = BreakerMask;
    return Sample;
}

bool AGerstnerWaterBody::IsInsideWaterBounds(const FVector& WorldLocation) const
{
    if (!WaterMesh)
    {
        return false;
    }

    const FBoxSphereBounds Bounds = WaterMesh->Bounds;
    const FVector Extent = Bounds.BoxExtent + FVector(1000.0f, 1000.0f, 100000.0f);
    return FBox::BuildAABB(Bounds.Origin, Extent).IsInsideOrOn(WorldLocation);
}

void AGerstnerWaterBody::PushRipple(const FVector& WorldLocation, const float Strength, const float Radius)
{
    if (DynamicWaterMaterial)
    {
        DynamicWaterMaterial->SetVectorParameterValue(TEXT("LastRippleWorldPosition"), FLinearColor(WorldLocation.X, WorldLocation.Y, WorldLocation.Z, Radius));
        DynamicWaterMaterial->SetScalarParameterValue(TEXT("LastRippleStrength"), Strength * InteractionRippleStrength);
    }
}

void AGerstnerWaterBody::UpdateMaterialParameters(const float WorldTime)
{
    if (!DynamicWaterMaterial)
    {
        return;
    }

    DynamicWaterMaterial->SetScalarParameterValue(TimeParameterName, WorldTime);
    const float FoamIntensity = WaveAsset ? WaveAsset->Shoreline.ShoreFoamIntensity + WaveAsset->Breakers.RockFoamIntensity : 1.0f;
    DynamicWaterMaterial->SetScalarParameterValue(FoamParameterName, FoamIntensity);
}

float AGerstnerWaterBody::ComputeShoreMask(const FVector& WorldLocation) const
{
    if (!WaveAsset || WaveAsset->Shoreline.ShorelineWidth <= 0.0f || !GetWorld())
    {
        return 0.0f;
    }

    FHitResult Hit;
    const FVector TraceStart(WorldLocation.X, WorldLocation.Y, GetActorLocation().Z + 50000.0f);
    const FVector TraceEnd(WorldLocation.X, WorldLocation.Y, GetActorLocation().Z - 50000.0f);
    FCollisionQueryParams Params(SCENE_QUERY_STAT(WaterShoreMask), false, this);
    const bool bHitGround = GetWorld()->LineTraceSingleByChannel(Hit, TraceStart, TraceEnd, ECC_Visibility, Params);
    if (!bHitGround)
    {
        return 0.0f;
    }

    const float Depth = GetActorLocation().Z - Hit.ImpactPoint.Z;
    const float DepthMask = 1.0f - FMath::Clamp(Depth / WaveAsset->Shoreline.ShorelineWidth, 0.0f, 1.0f);
    return WaterSmoothStep(0.0f, 1.0f, DepthMask);
}

float AGerstnerWaterBody::ComputeBreakerMask(const FVector& WorldLocation) const
{
    const UWorld* World = GetWorld();
    const UWaterGerstnerSubsystem* Subsystem = World ? World->GetSubsystem<UWaterGerstnerSubsystem>() : nullptr;
    if (!WaveAsset || !Subsystem || WaveAsset->Breakers.RockBreakerRadius <= 0.0f)
    {
        return 0.0f;
    }

    float Mask = 0.0f;
    for (const FWaterBreakerSource& Breaker : Subsystem->GetBreakers())
    {
        const float EffectiveRadius = FMath::Max(WaveAsset->Breakers.RockBreakerRadius, Breaker.Radius);
        const float Distance = FVector::Dist2D(WorldLocation, Breaker.Location);
        const float LocalMask = 1.0f - FMath::Clamp(Distance / EffectiveRadius, 0.0f, 1.0f);
        Mask = FMath::Max(Mask, WaterSmoothStep(0.0f, 1.0f, LocalMask));
    }
    return Mask;
}
