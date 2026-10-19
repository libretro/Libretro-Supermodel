# Quad Shader Instruction And Driver Review

Date: 2026-10-13  
Source: Current local Supermodel Libretro changes over `4566c9470ab89ec8e2738bfb43c18281df580971`  
Upstream Reference: `ed608de1c5f0f61dcad957cffe0ff8234cbc8e6c`  
Scope: Follow-up to [Quad Rendering Performance Review](QUAD_RENDERING_PERFORMANCE_2026-10-13.md)

The matched instruction probes do not identify `fma`, `precise`, division, square-root calculation or array indexing as an explanation for the large game-rendering penalty on the tested Mac. The latest Quad/Triangle controls reproduce most of the CPU-visible delay with a pass-through geometry shader and even with a geometry shader that emits no vertices. This localizes the dominant penalty to the Apple geometry-enabled draw path, without identifying its private driver mechanism. Native Quad depth/interpolation dependencies also have a secondary Mac-specific scoped cost, but the targeted final division does not explain it. Subsequent CGL worker-thread and multi-draw tests preserve the large completion cost: returning sooner from a GL entry point does not establish faster frames. None of these probes establishes a production rendering correction.

## Actual API And Shader Versions

The Mac reports OpenGL 4.1 / Metal 90.5 on Apple M4. Batocera reports OpenGL 4.6 core / Mesa 25.3.6 on Radeon Vega 11. The staged core requests a minimum OpenGL 4.1 context, and both hosts compile identical `#version 410 core` shader files. Batocera's available driver version is distinct from the GLSL version selected by the program. These results concern the staged diagnostic core; the previously installed Batocera core was not replaced.

The port's native quad shader body matches the reviewed standalone upstream byte content after universal newline handling and removal of upstream's inline version directives. The equality result is retained in `geometry-driver-followup/upstream-shader-comparison.json` under the original evidence root.

## Real Game Draw-Call Localization

Private copies of New3D count and time `glDrawArrays` calls. A frame-scope destructor includes early returns when Model 3 disables 3D. The existing frontend runner requests fast-forward only after the first telemetry window, past the startup context/reset. These changes belong to dated probes, not the production renderer or permanent Makefile checks.

The Scud attract comparison uses the original configuration and core telemetry windows 6–30. Each window contains 61 frames. Calls and submitted vertices match exactly across the original Quad, cheap-fragment and minimal-output variants. Splitting each eligible mesh at a complete quad boundary increases calls while preserving the submitted vertices and their order.

| Temporary Variant | Render p50 | Draw-Call CPU Time p50 | Calls Per Frame p50 |
| --- | ---: | ---: | ---: |
| Original Triangles | 1.1 ms | 0.246 ms | 328.2 |
| Original Quads | 26.9 ms | 26.117 ms | 328.2 |
| Original Quads Repeat | 26.3 ms | 25.652 ms | 328.2 |
| Full Quad Output, Cheap Fragment | 25.7 ms | 24.913 ms | 328.2 |
| Minimal Geometry Output, Cheap Fragment | 25.8 ms | 25.106 ms | 328.2 |
| Original Quads, Split Draws | 45.3 ms | 43.729 ms | 566.3 |

Across the measured frames, original Quad submits 564,147 calls and 23,604,496 vertices. Splitting submits 985,873 calls and exactly the same vertices, with no incomplete-primitive counts. The weighted mean CPU time per draw remains approximately 78 microseconds: 78.27 for original Quad, 77.50 for its repeat, and 77.86 for split draws. Triangles average 0.73 microseconds per draw. Shader simplification does not remove the approximately 76–78 microsecond Quad call cost.

The counter's CPU timing includes any wait inside the driver. It is not an instruction-level GPU measurement. The split variant preserves primitive boundaries and order but has not received game-image equivalence qualification; arithmetic/output-reduction variants intentionally change rendering and must not be shipped.

## Matched GPU Instruction Probes

Use the existing RetroArch environment/process runner and Batocera transport. A private diagnostic module executes before normal emulation using the actual shader sources, a shared binary vertex fixture and GLSL 4.10. Mac and Batocera manifests verify identical fixture and shader hashes.

Each measurement issues one draw. Three warm-up rounds precede ten measured rounds, with variants interleaved to reduce ordering/clock effects. Measurements use `GL_TIME_ELAPSED`, CPU draw-call time, and synchronized CPU completion time. Synchronization is deliberate for this isolated probe; its timings must not be substituted for asynchronous gameplay throughput.

- Vertex/geometry mode disables rasterization and captures all 64 output components, including `gl_Position`, through transform feedback. Sizes are 256 and 4096 quads. Each trial validates exactly two captured triangles per quad and retains finite output samples.
- Raster mode uses the native vertex/geometry shaders, a 64x64 framebuffer, and 16 or 256 quads. It changes only the fragment source for the targeted variants. Its GPU timer covers the complete scoped raster pipeline, not the fragment stage in isolation.
- Geometry variants test removal of `precise`, explicit non-fused multiply/add, removal of Kahan compensation, shared reciprocals, removal of division/area arithmetic and loop unrolling.
- Fragment variants test removal of `precise`, explicit non-fused multiply/add, loop unrolling, reciprocal-square-root length calculation and a deliberately cheap checksum/output shader.

Timer-query semantics: [Khronos ARB_timer_query](https://registry.khronos.org/OpenGL/extensions/ARB/ARB_timer_query.txt). Transform-feedback capture and primitive-count checks: [Khronos ARB_transform_feedback3](https://registry.khronos.org/OpenGL/extensions/ARB/ARB_transform_feedback3.txt). These APIs time/count scoped GL work; they do not expose individual GPU instructions or Apple's private shader translation.

Both final frontends exit 0 and produce 280 complete measurement rows, without compile/link/GL errors or primitive-count failures. Mac reports 32 timer bits and a 64-component transform-feedback limit; Vega reports 64 timer bits and a 128-component limit.

### Raster Results

GPU medians for 256 synthetic quads; no zero query values in these groups.

| Fragment Variant | Apple M4 | Vega 11 | Mac Paired Ratio To Native |
| --- | ---: | ---: | ---: |
| Native | 0.831 ms | 0.206 ms | 1.000 |
| No `precise` | 0.825 ms | 0.210 ms | 1.003 |
| Expanded `fma` | 0.871 ms | 0.207 ms | 1.047 |
| Unrolled Loops | 0.824 ms | 0.209 ms | 0.990 |
| Reciprocal-Square-Root Length | 0.830 ms | 0.214 ms | 0.991 |
| Cheap Checksum/Output | 0.361 ms | 0.129 ms | 0.433 |

The paired ratio uses each sample's native result from the same round; it is not necessarily the ratio of separately calculated medians. Small differences are not established improvements from ten samples. None of the targeted substitutions yields a large Mac gain. Expanding `fma` is slightly slower in this fixture. Bypassing the entire interpolation calculation shows that aggregate shader work has a real GPU cost, but the altered shader is not a valid production replacement and did not improve the real game's CPU-visible Quad delay.

The native and four targeted raster variants produce identical captured RGBA bytes at both sizes on each host. The cheap variant intentionally differs. This is synthetic-fixture evidence only; it does not qualify near-edge/coplanar precision, lighting, textures, game images or the full catalogue. Input images and selected shader branches differ from a real game, so the cross-host GPU ratio cannot be converted directly into game FPS.

### Geometry Results And Timer Limits

Some Mac transform-feedback samples report zero elapsed GPU time despite positive captured primitive counts. The cause/resolution behavior is unresolved. These values remain in raw logs and are marked as censored/unresolved; they are not free work, and affected GPU ratios are unavailable in the normalized report. Mac synchronized completion medians at 4096 quads are approximately 0.76–0.82 ms across these variants; Vega's are approximately 1.35–1.41 ms. Those are CPU/driver/GPU completion measurements, not pure geometry execution time.

No geometry substitution demonstrates a large Mac-specific completion penalty. The stable positive raster groups and the independent real-game draw-count experiment provide the clearer evidence. Ten synthetic samples cannot rule out every instruction, compiler lowering, GPU model or real content case. Register pressure, scheduling and private geometry-stage translation remain unmeasured mechanisms, not proven causes.

## Quad/Triangle Differences And Geometry-Stage Controls

The user's follow-up narrows the investigation to work added by Quad mode. The inspected sources contain 15 shared GLSL built-in functions and four additional Quad functions: `fma`, `length`, `sign` and `EmitVertex`. These are source functions, not GPU machine instructions. Arithmetic operators and qualifiers are separate. Shared `dot` and division operations still have additional Quad uses, so membership alone does not exclude different frequency or dependencies. The fragment body from fog/texture/shading through output, including the common helper source, is identical after comment/whitespace normalization.

Reuse the existing private module, smoke-test lifecycle, Linux builder and Batocera transport. The final matrix has 24 programs, three warm-up rounds and 12 measured rounds at each of two draw counts: **576 samples per host**. Every sample processes the same 512 positive-W quads in a 64x64 colour framebuffer, submitted in one draw or 128 draws of four quads. The triangle fixture duplicates the same positions/attributes in the native geometry shader's triangle order. Both hosts verify identical fixture, shader and case-list hashes. Lighting/fog are enabled; texture/specular branches and depth testing are disabled. There is no depth attachment, so colour captures do not qualify depth accuracy.

The stages are compared directly:

- Native Triangles has no geometry shader.
- Triangles Pass-Through GS adds a geometry shader that copies the triangle's existing attributes and positions. Captured RGBA bytes match native Triangles exactly for both draw counts on each host.
- Triangles Empty GS uses a geometry program whose main function returns without `EmitVertex` and produces no primitives. Its fragment shader has no inputs from undefined geometry outputs; captures remain fully clear.
- Quad Minimal keeps a geometry stage but replaces native interpolation/output with a cheap diagnostic path.
- Targeted Quad cases retain native surrounding code while changing precision, FMA, compensated products, reciprocals, division, area arithmetic, loop/modulo indexing, length or sign.

### Dominant Geometry-Enabled Draw Cost

Median CPU time spent inside all 128 draw calls, in milliseconds; the geometry total and its order are held constant within each program.

| Pipeline | Apple M4 | Vega 11 |
| --- | ---: | ---: |
| Native Triangles | 0.020 | 0.0014 |
| Triangles Pass-Through GS | 9.545 | 0.0014 |
| Triangles Empty GS, No `EmitVertex` | 8.795 | 0.0014 |
| Native Quad | 10.687 | 0.0014 |
| Quad Minimal | 9.380 | 0.0014 |

The empty stage preserves approximately 82% of native Quad's CPU-visible draw time despite executing neither native Quad mathematics nor vertex emission. The pass-through case reproduces the delay while retaining the tested triangle image. The earlier independent 21-case matrix gives the same signature: 0.020 ms for triangles, 9.987 ms for pass-through GS, 9.239 ms for empty GS and 11.070 ms for native Quad. All final scoped GPU timer values are positive; median/P95 series and individual rounds are retained. Vega CPU timings are very small; do not interpret their rounded equality or derive precise cross-host CPU ratios from them.

Scoped GL elapsed-query medians for 128 draws are 0.389/10.214/9.377/11.548 ms for Triangles/Pass-Through/Empty/Quad on M4, and 0.173/0.180/0.008/0.390 ms on Vega. Query elapsed intervals can include idle gaps while the CPU/driver supplies commands. They are not an isolated measure of geometry arithmetic or 128 independent instruction executions.

These controls provide causal evidence for the presence of the geometry-enabled pipeline on this Mac/driver. The large cost does not require executing `EmitVertex`, `fma`, `length`, `sign`, Kahan compensation or native Quad interpolation. No targeted arithmetic substitution removes the dominant draw-count-dependent penalty. The internal Apple allocation, synchronization, translation or scheduling mechanism remains unresolved, and different GPUs prevent an OS-only conclusion.

### Secondary Depth/Interpolation Dependencies

Changing only the final depth output to `gl_FragCoord.z` permits the compiler to remove original depth calculations/dependencies. In the complete matrix, the within-round pipeline-time ratio is 0.412 on M4 and 1.000 on Vega for one draw. With 128 draws, M4 still retains a ratio of 0.899: this does not remove its fixed draw penalty.

An additional 11-program matrix uses the same runner and inputs, with **264 samples per host**. Preserve native depth calculation through a small colour checksum, retain it in modified stored depth, or remove explicit depth writing while retaining the calculation in colour. Separately force each native depth branch and test removal/replacement of only the general branch's final `interp_oneOverW / uSum` division. Diagnostic changes deliberately alter depth/colour semantics.

Paired median GL elapsed ratio to native Quad, one draw:

| Depth Control | Apple M4 | Vega 11 |
| --- | ---: | ---: |
| Raster Depth, Original Calculation Unused | 0.415 | 0.969 |
| Original Calculation Retained In Colour | 1.002 | 1.077 |
| No Explicit Depth Store, Calculation Retained In Colour | 0.979 | 1.065 |
| Original Calculation Retained In Modified Stored Depth | 1.004 | 0.993 |
| General Branch Only | 0.907 | 0.972 |
| Coplanar Branch Only | 0.477 | 0.964 |
| General Branch, XY Retained In Colour | 0.955 | 1.048 |
| Final Division Removed, `uSum` Retained In Colour | 1.038 | 1.023 |
| Final Division Replaced By Signed Reciprocal Square Root | 0.994 | 0.997 |

Removing the explicit depth store does not produce the large gain when the original computation remains live. Removing/replacing the single final division also does not produce it. Bypassing full native depth dependencies or forcing the coplanar branch does. The result concerns a combination of calculation, dependency chains, compiled interfaces and possible scheduling/register pressure, rather than a measured slow individual instruction. Register occupancy and private generated code are not exposed by this probe. The input scene does not ordinarily take the coplanar branch; forcing it is a sensitivity control, not an accuracy-preserving solution.

Single-draw medians vary across separately scheduled matrices, so use paired ratios within each matrix instead of comparing unrelated absolute times. All 840 final samples per host pass compile/link/GL and hash checks. Synthetic colour equality and the empty-stage control qualify only the stated fixture. No gameplay, negative-W clipping, native depth/coplanar accuracy, texture/specular, Windows or catalogue-wide conclusion is added.

## Pristine Standalone Upstream Check

Build the reviewed upstream archive with its normal macOS Makefile and existing SDL2/SDL2_net frameworks. All 315 tracked source files are verified against Git blob hashes after building and after execution. The executable is ARM64. No source edit or installed standalone replacement is made.

Use isolated Config, Assets, NVRAM and Saves directories, the same ROM, native resolution, no throttle/vsync/network, and disabled sound emulation. Both modes use the same standalone-compatible NVRAM seed copied from the original core baseline. The native interpreter, initialized seed and upstream renderer differ from the Libretro fresh-boot/JIT baseline, so compare ON/OFF inside this native frontend and compare the driver signature rather than treating absolute FPS as equivalent.

A private external observer counts SDL swaps and times GL draws without changing GL state or draw arguments. It requests a normal SDL quit after 1830 swaps. Both native runs exit 0 and provide 30 timing windows; analysis selects windows 6–30. Median draw-call time increases from 0.453 ms/frame with triangles to 44.044 ms/frame with Quads. The weighted mean Quad call time is **77.88 microseconds**, closely matching the core's **78.27 microseconds**. Native interval medians are 5.062 and 46.071 ms; these are not directly comparable to the core callback timer.

A separate, un-injected native Quad run receives a three-second CPU sample, excluded from numerical timings. It reproduces the original driver signature: 459 main-thread samples in `glDrawArrays_ACC_GL3Exec`, prominent `AppleMetalOpenGLRenderer::gldRenderVertexArray` branches, and 216 samples in a Metal command-buffer initialization/semaphore-wait branch. The penalty therefore exists in pristine upstream too; it is not solely introduced by the Libretro adapter. This still does not prove the driver's private implementation or every port change harmless.

## Retained Diagnostic Failures

The first draw counter missed early-return frames, and its early fast-forward request did not establish unthrottled execution. That run is excluded; the frame-scope counter and deferred request fix the harness.

An initial fragment-unrolling generator changed an unsigned wraparound expression into signed arithmetic. Its unrolled results are excluded. The corrected generator preserves `uint` literals, and the final matched raster captures agree with native. Functional GL success alone is not a semantic correctness check.

The first native observer recursively called its own dyld interposition through `RTLD_NEXT`. Its stack-guard crash belongs to the profiler and is excluded. Direct original bindings from the interposer image fix it. A subsequent launch lacked the upstream crosshair bitmap Assets and correctly failed before gameplay; supplying the existing upstream Assets resolves that prerequisite. The final native runs and un-injected sample are separate successful evidence.

The first empty-GS control did not write attributes still requested by its fragment shader and failed program linking. That 0-row Mac run is excluded. A fragment shader without those inputs provides the valid no-emission control; this is a diagnostic interface repair, not a native Supermodel failure.

## CGL Worker And Multi-Draw Workaround Tests

The next bounded tests use unchanged shaders and the existing stage-control fixture. Apple's [OpenGL threading guide](https://developer.apple.com/library/archive/documentation/GraphicsImaging/Conceptual/OpenGL-MacProgGuide/opengl_threading/opengl_threading.html) documents `kCGLCEMPEngine` as a driver worker-thread option. This retired guide is a candidate source, not evidence of a current Apple Silicon geometry-shader fix. Runtime queries, rather than the requested environment variable alone, establish that the tested M4 context defaults to OFF, accepts OFF/ON with no CGL errors and reports states `0`/`1`. Synthetic probes restore the original OFF state; game probes keep the requested state for their owned context's lifetime, reapply it after context initialization and query it in every recorded window.

The CGL matrix retains Triangles, Pass-Through GS, Empty GS and Native Quad. It uses the same 512 quads, 64x64 RGBA framebuffer, 1/128 draw counts, three warm-up rounds and twelve measured rounds per case. A default run and three alternating OFF/ON repeats provide 672 valid rows. All elapsed queries are positive; all corresponding RGBA captures are byte-identical across modes and repeats. Pass-through matches Triangles; Empty GS intentionally produces zeros. These checks qualify this fixture, which has no depth attachment, not real-game images or native depth accuracy.

Native Quad medians for 128 draws:

| Measurement | CGL OFF | CGL ON |
| --- | ---: | ---: |
| CPU Time Inside Draw Calls | 11.181 ms | 0.000542 ms |
| Synchronized Completion | 12.393 ms | 11.977 ms |
| Scoped GL Elapsed Query | 12.097 ms | 11.716 ms |

ON makes command submission asynchronous, but completion improves only 3.4% in this matrix. The full 128-draw penalty remains. Query intervals include command-supply/queue gaps and cannot identify private driver work or individual shader instructions. Completion P95 is 13.908 ms OFF and 12.359 ms ON; a sooner API return must not be reported as an equivalent GPU or game-speed improvement.

### Fixed-State Scud Verification

Two OFF and two ON runs reuse the validated Scud frontend procedure in OFF–ON–ON–OFF order. Every run starts from the same retained Save RAM sample, SHA-256 `af4fd4eaf4f217ca7fc5ab0f54da25db60c1523c3098f80b94426292134609b8`, with Automatic Initial NVRAM and NVRAM Settings disabled. Other settings remain New3D/native/Quad enabled, core `multi_gpu`, frameskip zero, frontend video threading disabled, audio disabled and vsync disabled. Fast-forward is requested after the first timing window, past startup context/reset.

The first comparison has the normal timing overlay enabled. A second four-run comparison disables its drawing while retaining timing logs through one private log condition. Both use the same native draw counter and CGL instrumentation. Each comparison selects windows 6–30, or 1,525 core frames per run after 305 warm-up frames. All eight selected workloads have exactly the same per-window draw/vertex vectors: 564,147 calls and 23,604,496 vertices, with no incomplete primitive counts. Each state contributes fifty selected windows per overlay mode.

| Scud Window Median | CGL OFF | CGL ON |
| --- | ---: | ---: |
| Render, Overlay Enabled | 28.25 ms | 1.20 ms |
| Overlay, Overlay Enabled | 0.00 ms | 26.05 ms |
| Complete Callback, Overlay Enabled | 28.70 ms | 29.35 ms |
| Measured Window FPS, Overlay Enabled | 35.45 | 34.70 |
| Render, Overlay Disabled | 28.30 ms | 28.05 ms |
| Complete Callback, Overlay Disabled | 28.90 ms | 28.30 ms |
| Measured Window FPS, Overlay Disabled | 35.30 | 35.90 |

With the overlay enabled, the large wait reappears in overlay drawing rather than timed draw submission. Without the overlay, draw-call CPU time still falls from 27.480 to 0.005837 ms, but most of the Render interval remains: 28.30 versus 28.05 ms. The private driver work/wait has moved outside the timed entry points. The exact remaining GL operation and internal driver mechanism are not localized by these runs. The 1.7% difference in median window FPS without the overlay does not resolve the slowdown; callback P95 remains 86.4 versus 86.2 ms. These are attract-sequence throughput checks, not display-vsync, physical input, audio or game-image qualification.

The initial game validator demanded exactly thirty CGL windows and rejected an otherwise clean 31-window execution. Its log and validation note remain separately under `game-runs/off-r1-initial-count-rejected`; it is excluded from the fixed-state comparisons. Subsequent runs request `--max-frames=1830` but retain 31–36 raw core timing windows. Their extra tail count is unresolved. Analysis selects the same first thirty core windows and verifies the exact selected workload instead of comparing total wall times or assuming frontend frame limits equal internal core counters.

### Multi-Draw Submission Control

A second candidate sends the same ordered primitive ranges through one `glMultiDrawArrays` call rather than a loop of `glDrawArrays`. CGL worker mode is forced OFF, with no shader, uniform, vertex-data or topology change. Range-array preparation is outside the timed call interval. The unchanged core/program remains bound for the entire synthetic sample; this deliberately avoids the real renderer's per-mesh state changes. The driver exposes the function, all four runs in Loop–Multi–Multi–Loop order succeed, and 384 rows plus all corresponding captures validate.

| Native Quad, 128 Ranges | Draw Loop | One Multi-Draw Call |
| --- | ---: | ---: |
| CPU Time Inside Submission | 11.054 ms | 10.992 ms |
| Synchronized Completion | 12.262 ms | 12.155 ms |
| Completion P95 | 12.601 ms | 12.931 ms |

The 0.9% median completion difference leaves the geometry-enabled cost intact and is not a demonstrated practical correction. Reducing public API entry count alone does not remove the cost for these 128 logical draws. This does not disprove merging truly compatible meshes into fewer logical draws, but that is a distinct structural change with ordering/state/accuracy obligations; it is not implemented or approved by this diagnostic result.

Neither candidate changes production sources, default options or installed binaries. There is no basis here to enable CGL worker mode by default or rewrite the renderer around multi-draw. The tested alternatives are retained as negative/limited results; the geometry-enabled draw path remains the dominant unresolved issue.

## Reproduction And Delivery

Evidence remains under `outputs/quad-performance-20261013/`:

- `geometry-driver-followup/`: builder/runner snapshots, private shader sources, game draw statistics, pristine source verification, native observer and un-injected sample.
- `shader-instruction-followup/`: shared GLSL sources, generated vertex fixture, probe module, matched manifests, raw logs, normalized comparison and synthetic captures. Final hosts are `mac-r3` and `batocera-r2/benchmark`; earlier runs remain separate.
- `quad-specific-followup/`: source difference inventory, shared 24-case matrix, copied triangle fixtures, independent pass-through/empty-stage captures, normal-build commands and Linux dependency checks. Final hosts are `mac-r4` and `batocera-r2/benchmark`; the initial 21-case hosts remain separate. The additional 11-case depth controls are under `depth-followup/mac-r2` and `depth-followup/batocera-r2/benchmark`. Each final run retains its case list and source manifest. `analyze.py` validates and recomputes both comparisons, including P95 series and paired ratios.
- `cgl-workaround-test/`: shared four-case GLSL/input payload, private builders and frontend runner, queried CGL states/restoration, 672 synthetic rows and two four-run Scud comparisons. `game-runs/` enables the overlay; `game-no-overlay-runs/` retains logs with overlay drawing disabled. The initial count-rejected run remains explicitly separate.
- `multidraw-workaround-test/`: the same payload and a private submission-only modification, with 384 rows, effective submission markers and exact corresponding RGBA capture checks.

Recompute matched statistics with:

```sh
python3 outputs/quad-performance-20261013/shader-instruction-followup/analyze.py
python3 outputs/quad-performance-20261013/quad-specific-followup/analyze.py
python3 outputs/quad-performance-20261013/analyze-workarounds.py
```

No permanent Makefile check, production shader change, option/default change, commit or push is part of this review. The installed macOS core remains SHA-256 `17cdde5e287691f8fae90981bdda1b5ecebccab77ec0f1b5137bbcd1af3fe74c`. The installed Batocera core remains `97ab5c54dbbf622b33eba712a5a01bb03753c09faa31320ac56897061eaadc08`; all remote staging uses marked test directories left for review. Owned probe processes have exited.

The dominant geometry-enabled draw cost is now reproduced independently of native Quad mathematics and vertex emission. The depth/interpolation dependency sensitivity is secondary and does not establish a safe shader change. CGL worker threading moves the CPU-visible wait without materially changing game throughput, while one multi-draw entry point retains the logical-draw cost. Do not weaken native precision, bypass depth branches or recommend a replacement renderer from these diagnostics. A production correction requires a demonstrated link to the cause, the upstream boundary, native quad interpolation, shader/backend coverage and controlled game-image validation. F16/F17 Framework feedback records reusable measurement and probe-validation lessons; it does not automatically modify other cores. Historical report fingerprints remain attributable to their respective snapshots.
