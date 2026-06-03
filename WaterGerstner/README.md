# Water Gerstner for Unreal Engine 5.7

Runtime plugin for a configurable, interactive Gerstner ocean. The plugin ships C++ gameplay systems and shader helper code so projects can build realistic water materials without committing binary `.uasset` files.

## Features

- `AGerstnerWaterBody` actor with configurable Gerstner wave assets, shoreline shoaling, foam, material parameters and ripple injection.
- `UWaterGerstnerWaveAsset` data asset with hand-authored waves or procedural Beaufort/wind spectrum generation.
- `UWaterBuoyancyComponent` that applies pontoon forces, water drag, angular damping and water-velocity matching so physics objects float.
- `UWaterInteractionComponent` for moving actors to create material ripples.
- `UWaterBreakerComponent` for rocks/cliffs: nearby waves curl, steepen and produce breaker foam.
- `/Plugin/WaterGerstner/WaterGerstnerCommon.ush` shader include with Gerstner evaluation, foam masks, absorption helpers and anti-tiling noise.

## Quick start

1. Copy `WaterGerstner` into `Plugins/WaterGerstner` or keep it as a project plugin.
2. Enable the plugin and regenerate project files.
3. Create a `WaterGerstnerWaveAsset`, then call `Build Beaufort Spectrum` or add wave entries manually.
4. Place `AGerstnerWaterBody` in the level, assign a large plane/static mesh, the wave asset and a Single Layer Water material.
5. Add `UWaterBuoyancyComponent` to boats, crates or debris. Ensure the root primitive simulates physics.
6. Add `UWaterBreakerComponent` to rock actors and cliffs where waves should break.
7. Add `UWaterInteractionComponent` to characters/boats that should create ripples.

## Material setup

Create a Single Layer Water material and include the shader helper from a Custom node or material function:

```hlsl
#include "/Plugin/WaterGerstner/WaterGerstnerCommon.ush"
```

Recommended parameters:

- `WaterTime` scalar driven by `AGerstnerWaterBody`.
- `FoamIntensity` scalar driven by `AGerstnerWaterBody`.
- `LastRippleWorldPosition` vector (`xyz` position, `w` radius) updated by interactions.
- `LastRippleStrength` scalar updated by interactions.

Use at least two normal-map octaves with different scales and feed `WaterTriPlanarAntiTile` into UV offsets to break visible tiling. Blend crest foam, shore foam and breaker foam through `WaterFoamMask`.

## Shoreline and breakers

Shoreline surf is inferred from water depth by visibility traces under the sample point. Use blocking landscape/static geometry on the visibility channel for accurate surf bands. Rock breakers are explicit: add `UWaterBreakerComponent` to rocks and tune `RockBreakerRadius`, `RockFoamIntensity`, `CurlAmount` and `Turbulence` on the wave asset.

## Buoyancy tuning

Use four or more pontoons on boats. Increase `Buoyancy` per pontoon for heavier actors, `LinearWaterDrag` for calmer bobbing and `VelocityMatch` for objects that should drift with waves. Enable `bDrawDebug` to visualize pontoon immersion.
