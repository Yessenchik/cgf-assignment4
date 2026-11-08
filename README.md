## Build and Run

```bash
git clone https://github.com/Yessenchik/cgf-assignment4.git
cd cgf-assignment4
cmake -S . -B build -G Xcode
cmake --build build --config Debug
```

Run Part 1 (built-in control points, or pass a file):

```bash
./build/Debug/part1
./build/Debug/part1 models/patch_control_points.txt
```

Run Part 2:

```bash
./build/Debug/part2
```

Run the Part 2 automated check (opens a window for about two seconds, exits with code 0 on success):

```bash
./build/Debug/part2 --self-test
```

Run Part 3 (2D texture on the Bézier patch; 3D procedural texture on an SMF model):

```bash
./build/Debug/part3_2d
./build/Debug/part3_3d
./build/Debug/part3_3d models/bound-lo-sphere.smf
```

Shaders (`shaders/`) and models (`models/`) are found through absolute paths that CMake compiles in,
so the programs can be started from any directory.

If `build/` was previously configured with a different generator, remove `build/CMakeCache.txt` and `build/CMakeFiles/` (or use another build directory) before configuring with `-G Xcode`. With the default Makefile generator the binaries are directly in `build/`.

Dependencies: macOS OpenGL and GLUT frameworks only (Parts 1–2 use the legacy OpenGL 2.1 context,
Part 3 uses an OpenGL 4.1 core-profile context with GLSL 4.10). No third-party libraries.

## Part 1 — Bézier Patch

A flat-shaded tessellation of a bicubic Bézier patch with editable control points.

- 16 control points in a 4×4 grid, hard-coded or loaded from a text file.
- Iterative Bernstein evaluation; initial resolution 10×10 (200 triangles).
- One normal and one diffuse color per triangle (flat shading).
- Light direction attached to the camera.
- Control points drawn with `GL_POINTS`; the selected one is larger and red.
- X/Y/Z axes as short red/green/blue lines at the origin.
- Console output for selection, point coordinates, tessellation and camera reset.

| Key | Action |
|---|---|
| ← / → | Rotate camera horizontally |
| ↑ / ↓ | Rotate camera vertically (clamped to ±85°) |
| R | Reset camera |
| Tab | Select next control point |
| J / L | Move selected point −X / +X |
| I / K | Move selected point +Y / −Y |
| U / O | Move selected point +Z / −Z |
| + / − | Increase / decrease tessellation (2…50) |
| ESC | Quit |

### Control point file format

Three floats per line, 16 lines, X-Y ordering `[0,0], [1,0], [2,0], [3,0], [0,1], [1,1], …`
(the first index varies fastest). `models/patch_control_points.txt` contains the built-in points.

## Part 2 — Anti-aliasing and Picking

Three smooth-shaded triangle-mesh objects under perspective projection:

| ID | Object | Initial color |
|---|---|---|
| 1 | Sphere (left) | red |
| 2 | Rounded cube (center) | green |
| 3 | Torus (right, tilted 65°) | blue |

| Input | Action |
|---|---|
| ← / → / ↑ / ↓ | Orbit camera around the scene |
| R | Reset camera |
| A | Toggle anti-aliasing (4× MSAA on this machine, ON at startup) |
| Left click | Pick an object and give it a random diffuse color |
| ESC | Quit (releases GL resources) |

## Part 3A — 2D Texture Mapping on the Bézier Patch (`part3_2d`)

- The Part 1 patch (same 16 control points and Bernstein evaluation, `src/common/BezierPatch`) is
  tessellated into a **12×12** mesh: 169 vertices, 288 triangles, drawn from a VAO with an index buffer.
- Each vertex stores position, an analytic normal `normalize(∂S/∂u × ∂S/∂v)` and the texture coordinate
  `(u, v)` — the same parameters used to compute the position — so one image covers `[0,1]×[0,1]`.
- The 512×512 RGB image is generated in C++ (`generatePatchImage`): gradient background, a u/v grid,
  a central rosette and colored corner markers — red (0,0), green (1,0), blue (0,1), yellow (1,1) — that
  make the orientation of the mapping easy to check. It is uploaded as `GL_RGB8` with mipmaps,
  trilinear filtering and clamp-to-edge wrapping.
- The fragment shader samples the texture and uses the RGB as the **diffuse** color of Phong shading
  (ambient + diffuse + white specular), so highlights stay white on top of the image.

## Part 3B — 3D Procedural Texture on an SMF Model (`part3_3d`)

- Loads an SMF file (default `models/bound-bunny_200.smf`, the bunny from Assignment 3; any SMF path
  can be passed as an argument). The model is centered, scaled to unit size and turned Z-up by its
  model matrix; vertex normals are area-weighted averages of the face normals.
- The fragment shader computes a **marble** texture from the world-space position: 3D gradient noise,
  5-octave fBm, two levels of domain warping, and veins where a warped oblique phase crosses zero.
  The resulting RGB is the diffuse color of Phong shading with a polished-stone specular highlight.
- Because the pattern is a function of world-space (X, Y, Z), it is fixed to the model and does not
  slide when the camera moves.

### Part 3 controls (both programs)

| Key | Action |
|---|---|
| ← / → / ↑ / ↓ | Rotate camera |
| + / − | Zoom in / out |
| W | Toggle wireframe |
| R | Reset camera |
| ESC | Quit (releases GL resources) |

The light is attached to the camera (up and to the right of the eye), so the highlight follows the view.

### Part 3 shaders

| File | Purpose |
|---|---|
| `shaders/part3_2d.vert` | World position, normal and `(u, v)` to the fragment stage |
| `shaders/part3_2d.frag` | `texture()` lookup → diffuse color, Phong lighting, two-sided normal |
| `shaders/part3_3d.vert` | World position and normal to the fragment stage |
| `shaders/part3_3d.frag` | Gradient noise, fBm, domain-warped marble → diffuse color, Phong lighting |


