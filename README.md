# Ame.

## What is Ame?

Ame inspired by the Japanese word for rain, 雨 (ame), is a game engine with editor written in C++ 23, with a focus on modern C++ and ease of use.

This term is also represented through the acronym 'Agile Modular Engine' (Ame) as a reflection of the project's core values.

## Why Ame?

Ame is a modular engine, which means that it is easy to extend and customize.

## How to use Ame?

### Prerequisites

* CMake 3.26 or higher
* C++ 23 compiler
* vcpkg
* dotnet 6.0 or higher

### Supported platforms
* Windows
* Linux

### Setup

* Run `Setup.ps1` to install all dependencies.

### Build

```bash
git clone
cd Ame
git submodule update --init --recursive
cmake --build --preset "<platform>-<configuration>"
```

## Roadmap
* Modern C++.
* Easy usage.
* Appeal to different demographics
    * Beginners
    * Intermediate
    * Advanced
    * Developper
* Basic Features
    * Frameworks
        - [ ] Modules can be extended / overridden depending on what the user wants.

    * Graphics
        - [x] DirectX 12
        - [x] Vulkan
        - [ ] Metal

    * Window
        * Basic window
        * Customized window
        * Editor
        * Dockable window

    - [ ] Audio
        * Basic audio
        * Customized audio
        * Editor
        * Audio graph
        * Audio effects

    - [ ] Physics

    * Input
        * Basic pooling
        * Input hooking

    * Math types
        * Vector
        * Matrix
        * Rect
        * Viewport
        * Color

    * Logging
        * Console logging
        * File logging

    * Event system
        * Private events
        * Public events

    * Scripting
        * C# Support

    * Concurrency
        * Thread pooling
        * Coroutines

    * Editor
        - [ ] Compose Assets
        - [ ] Prepare the runtime

    * Runtime
        * Run only on assets
        * Can Run without any assets
        * Extendable

    * Resource Management
        * Async
        * Customizable Loaders, Savers, Exporters, Importers
        * Multiple packs and resource format
        * Resource dependecy resolution

    * Rendering
        - [ ] 2D
        - [ ] 3D
        - [ ] 2.5D

        * Render graph
            * Automatic nodes dependecy resolution
            * Customizable nodes
            * Extendable nodes

        - [ ] GUI
            * ImGui
            * Customizable with editor
        - Frustum culling
            - [ ] GPU Side
            - [ ] CPU Side
        - [ ] Ray tracing
            * GPU Side
            * CPU Side
        - [ ] Light management
        - [ ] Post process effects
        - [ ] Customizable shaders
        - [ ] Customizable materials
        - [ ] Customizable render passes

    * Scene
        - [ ] ECS (Flecs)
        - Components
            - [ ] Transform
            - [ ] Mesh
            - [ ] CSG
            - [ ] Sprite
            - [ ] Audio player
            - [ ] Lights

    - [ ] Networking

    * Cross-platform
        - [x] Windows
        - [x] Linux
        - [ ] Mac
        - [ ] Mobile

## Layer of usage.

* Simple
    * Easy to use, Easy to understand.
    * Game loop won't be visible.
    * Everything is hidden in simple classes/functions when needed.
    * Customizable, user will write simple main function, hook up any needed callbacks and call it a day.
    * Self contained resource management.

* Advanced
    * Internal API is exposed.
    * User must provide a propper management of game loop.
    * User can choose what framework / module to enable / disable.

* Framework
    * Individual modules can be constructed / changed.
