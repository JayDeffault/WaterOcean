#include "WaterBuoyancyComponent.h"

#include "Components/PrimitiveComponent.h"
#include "DrawDebugHelpers.h"
#include "GameFramework/Actor.h"
#include "WaterGerstnerSubsystem.h"

UWaterBuoyancyComponent::UWaterBuoyancyComponent()
{
    PrimaryComponentTick.bCanEverTick = true;
    PrimaryComponentTick.TickGroup = TG_PrePhysics;
}

void UWaterBuoyancyComponent::BeginPlay()
{
    Super::BeginPlay();
    ResolveSimulatedComponent();
    if (bAutoCreatePontoonsFromBounds && Pontoons.IsEmpty())
    {
        AutoCreatePontoons();
    }
}

void UWaterBuoyancyComponent::TickComponent(const float DeltaTime, const ELevelTick TickType, FActorComponentTickFunction* ThisTickFunction)
{
    Super::TickComponent(DeltaTime, TickType, ThisTickFunction);

    if (!SimulatedComponent || !SimulatedComponent->IsSimulatingPhysics())
    {
        return;
    }

    UWorld* World = GetWorld();
    if (!World)
    {
        return;
    }

    const float GravityMagnitude = FMath::Abs(World->GetGravityZ());
    const FTransform ComponentTransform = SimulatedComponent->GetComponentTransform();

    for (FWaterPontoon& Pontoon : Pontoons)
    {
        const FVector PontoonWorld = ComponentTransform.TransformPosition(Pontoon.LocalOffset);
        FWaterSurfaceSample Sample;
        if (!UWaterGerstnerSubsystem::QueryWaterSurface(this, PontoonWorld, Sample))
        {
            Pontoon.Immersion = 0.0f;
            continue;
        }

        const float SignedDepth = Sample.Height - PontoonWorld.Z;
        Pontoon.Immersion = FMath::Clamp((SignedDepth + Pontoon.Radius) / (2.0f * Pontoon.Radius), 0.0f, 1.0f);
        if (Pontoon.Immersion <= UE_SMALL_NUMBER)
        {
            continue;
        }

        const float DisplacedVolumeCm3 = (4.0f / 3.0f) * UE_PI * FMath::Pow(Pontoon.Radius, 3.0f) * Pontoon.Immersion;
        const float DensityKgPerCm3 = FluidDensity / 1000000.0f;
        const FVector BuoyantForce = FVector::UpVector * DisplacedVolumeCm3 * DensityKgPerCm3 * GravityMagnitude * Pontoon.Buoyancy;
        SimulatedComponent->AddForceAtLocation(BuoyantForce, PontoonWorld);

        const FVector PointVelocity = SimulatedComponent->GetPhysicsLinearVelocityAtPoint(PontoonWorld);
        const FVector RelativeVelocity = PointVelocity - Sample.Velocity * VelocityMatch;
        const FVector DragForce = -RelativeVelocity * LinearWaterDrag * Pontoon.Immersion * SimulatedComponent->GetMass();
        SimulatedComponent->AddForceAtLocation(DragForce, PontoonWorld);

        if (bDrawDebug)
        {
            DrawDebugSphere(World, PontoonWorld, Pontoon.Radius, 12, FColor::Cyan, false, 0.0f);
            DrawDebugLine(World, PontoonWorld, Sample.Position, FColor::Blue, false, 0.0f, 0, 2.0f);
        }
    }

    const FVector AngularVelocity = SimulatedComponent->GetPhysicsAngularVelocityInRadians();
    SimulatedComponent->AddTorqueInRadians(-AngularVelocity * AngularWaterDrag * SimulatedComponent->GetMass());
}

void UWaterBuoyancyComponent::ResolveSimulatedComponent()
{
    if (SimulatedComponent)
    {
        return;
    }

    AActor* Owner = GetOwner();
    SimulatedComponent = Owner ? Cast<UPrimitiveComponent>(Owner->GetRootComponent()) : nullptr;
}

void UWaterBuoyancyComponent::AutoCreatePontoons()
{
    if (!SimulatedComponent)
    {
        return;
    }

    const FVector Extent = SimulatedComponent->Bounds.BoxExtent;
    const float Radius = FMath::Max(20.0f, FMath::Min3(Extent.X, Extent.Y, Extent.Z) * 0.35f);
    const float Z = -Extent.Z * 0.45f;
    Pontoons.Reset(4);
    const FVector Offsets[4] = {
        FVector( Extent.X * 0.65f,  Extent.Y * 0.65f, Z),
        FVector( Extent.X * 0.65f, -Extent.Y * 0.65f, Z),
        FVector(-Extent.X * 0.65f,  Extent.Y * 0.65f, Z),
        FVector(-Extent.X * 0.65f, -Extent.Y * 0.65f, Z)
    };

    for (const FVector& Offset : Offsets)
    {
        FWaterPontoon Pontoon;
        Pontoon.LocalOffset = Offset;
        Pontoon.Radius = Radius;
        Pontoon.Buoyancy = 1.0f;
        Pontoons.Add(Pontoon);
    }
}
