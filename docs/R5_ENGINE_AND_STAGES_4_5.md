# R5 — 3D engine, Stage 4 render, Stage 5 lighting, animation (technique notes)

## Files
| File | Role |
|---|---|
| `src/engine3d/Mat4.h` | Hand-written 4x4 matrices (translate, rotateX/Y, perspective), column-major |
| `src/engine3d/OrbitCamera.h` | Camera orbiting the nucleus; input sets targets, `update()` glides toward them |
| `src/engine3d/Shader.*` | Compile/link GLSL programs with readable error logs |
| `src/engine3d/CloudBuffer.*` | GPU point storage; two buffers (A current, B target) for morphing |
| `src/core/Animation.h` | `smoothstep`, `easeInOutCubic`, `Tween`, `approach` (shared by all stages) |
| `src/stage4_clouds/CloudStage.*` | Stage 4: additive-blended probability cloud |
| `src/stage5_shaded/ShadedStage.*` | Stage 5: lit spheres, Blinn-Phong, texture lookup, depth fog, cutaway |
| `src/stage4_clouds/Orbital.*` | Hydrogen sampling (the data contract: `sampleOrbital(n,l,m,count,seed)`) |

## Pipeline: Model -> View -> Projection
`clip = Projection * View * Model * position`. Model = spin about Y. View = translate(0,0,-d)·Rx(pitch)·Ry(yaw).
Projection = perspective (f = 1/tan(fov/2); w' = -z). The GPU divides by w to get normalised device coords.

## Stage 4 — why additive blending, no depth test
Each point is a semi-transparent sample of a probability cloud. Summing contributions is
commutative, so no sorting is needed; dense regions brighten = higher probability.
Point size ∝ 1/w gives a depth cue.

## Stage 5 — lighting
* **Sphere impostor**: a point sprite; in the fragment shader `n = (x, y, sqrt(1-x²-y²))`, the pixel's
  view-space position is `centre + n * radius`, and `gl_FragDepth` is written from it, so spheres
  intersect correctly under the depth test.
* **Blinn-Phong**: `colour = albedo*(ambient + I*max(n·L,0)) + I*(n·H)^48`, `H = normalize(L+V)`.
  Done in view space (camera at origin => V = -position).
* **Texture mapping**: albedo = `texture(1D ramp, density^0.4)` — a transfer function stored as a 256-texel texture.
* **Depth shading**: fog = smoothstep(2.2, 6.0, depth) blends far spheres toward the background.
* **Cutaway (C)**: fragments with model-space x > 0 are discarded to show the inside.
* **Density cut-off (, .)**: vertices with density below a threshold are clipped, removing the faint haze.

## Animation
* Orbital change (N/B): both clouds live on the GPU; the vertex shader blends A->B with
  `uMorph` driven by a `Tween` (1.3 s, eased) and a per-vertex delay hash so points peel away
  at different times.
* Camera: exponential smoothing `x += (target-x)*(1-exp(-rate*dt))` — identical at any frame rate.
* Spin and the orbiting light are driven by `dt`, never by frame count.
