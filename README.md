<h1>Tetris</h1>

## About

<p>A raw version of the tetris video game.</p>

## Building

<p>
    The project is built with CMake (3.21 or newer). On Windows/MSVC, CMake
    downloads SDL2, SDL2_image and SDL2_ttf into the build folder by itself, so
    there is nothing to install or copy by hand.
</p>

```bash
cmake --preset default
cmake --build --preset default-release
```

<p>
    The game ends up in <code>build/default/Release</code>, together with the SDL
    DLLs and the <code>assets</code> folder. Visual Studio can also just open the
    repository folder (File &gt; Open &gt; Folder), it picks up
    <code>CMakePresets.json</code> on its own.
</p>

<p>
    The <code>default</code> preset pins the <em>Visual Studio 18 2026</em>
    generator. With another Visual Studio installed, change the
    <code>generator</code> field in <code>CMakePresets.json</code>, or use one of
    the <code>ninja-*</code> presets.
</p>

<p>
    To use an SDL that is already installed on the system (vcpkg, apt, brew, ...)
    instead of the downloaded one, configure with
    <code>-DTETRIS_FETCH_SDL=OFF</code>, or use the <code>system-sdl</code>
    preset.
</p>

## Features

<ul>
    <li>Piece fall speed up</li>
    <li>Pieces fall preview position</li>
</ul>

### Piece fall speed up

<img src="https://davidrapaz.netlify.app//images/tetris/piece_speed_up_fall.gif" alt="piece_speed_up_fall" />

### Piece fall preview position

<img src="https://davidrapaz.netlify.app//images/tetris/piece_fall_preview.gif" alt="piece_fall_preview" />

### Developed With

[![C++](https://img.shields.io/badge/c++-%2300599C.svg?style=for-the-badge&logo=c%2B%2B&logoColor=white)](https://img.shields.io/badge/C++-00599C.svg?style=for-the-badge&logo=C++&logoColor=white)