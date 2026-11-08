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

If `build/` was previously configured with a different generator, remove `build/CMakeCache.txt` and `build/CMakeFiles/` (or use another build directory) before configuring with `-G Xcode`. With the default Makefile generator the binaries are `build/part1` and `build/part2`.

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


