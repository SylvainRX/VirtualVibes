
![Virtual Vibes](/readme-assets/readme_cover.png)

# ***Virtual Vibes***

[![virtual-boy](https://img.shields.io/badge/topic-Virtual--Boy-red?logo=github&style=flat-square)](https://github.com/topics/virtual-boy) [![VUEngine](https://img.shields.io/badge/topic-VUEngine-red?logo=github&style=flat-square)](https://github.com/topics/vuengine) [![homebrew](https://img.shields.io/badge/topic-homebrew-blue?logo=github&style=flat-square)](https://github.com/topics/homebrew)


***Virtual Vibes*** is an upcoming fast-paced rhythm game for the [Nintendo Virtual Boy](), inspired by classics like StepMania, where arrows approach a central D-pad from all four directions. You'll use both of the system's D-pads simultaneously to hit regular and long arrows in sync with the music, while the triggers shoot forward arrows that appear closer to your eyes. It features 4-lane gameplay, precise timing mechanics, an engaging soundtrack optimized for the Virtual Boy's unique hardware, and even lets you save your score to the cartridge if you make it to the leaderboard!

**Note:** This repository is dedicated to sharing development progress, updates, and documentation. While development is mostly complete, the full game is not yet released. The complete codebase is not publicly available.


## Features

- 🎵 **4-lane rhythm gameplay**.
- ⬆ **Support for long, normal, and foreground arrows that pop with the Virtual Boy's 3D magic**.
- 🎮 **Controller input** that uses both D-Pads simultaneously to hit any direction, plus L and R triggers for foreground arrows, leveraging the Virtual Boy's unique dual D-Pad controller design.
- 🏆 **Leaderboard system** stored in SRAM, so your high scores stay saved even after powering off the console.
- 🔊 **MIDI-based audio.**
- 📊 **Three difficulty levels per song & scrolling and music speed options.**
- 💥 **Full support for the [RetroOnyx Rumble Pack](https://www.retroonyx.com/product-page/virtual-boy-rumble-pack)**, delivering immersive haptic feedback.


## Watch the Trailer

[![Watch the demo](/readme-assets/trailer_placeholder.png)](https://youtu.be/hE5KWLaTUK4)


## What's to Come

- 🎵 Original songs composed by musicians specifically for ***Virtual Vibes***.
- 🚀 A Kickstarter campaign is planned to fund a physical cartridge release once the game is complete.

### Nice to have
- 🔊 New sound effects in the splash screen and menus to enhance the experience.
- 🎬 More animations and visual feedback for smoother and livelier gameplay.
- 👾 Possibly a character or mascot that interacts with the player to add personality.


## Powered by VUEngine

<img align="right" src="https://avatars.githubusercontent.com/u/53521437?s=90">
Written in Virtual C, "a custom C-dialect that resembles some of C++’ syntax that is transpiled to plain C with macros
<a href="https://github.com/VUEngine">VUEngine</a> <a href="https://github.com/VUEngine/VUEngine-Studio/releases/tag/v0.6.0">0.6.0</a>.<br>
Featured on <a href="https://www.vuengine.dev">VUEngine.dev</a>


## Getting Started

***Virtual Vibes*** runs on real Virtual Boy hardware and all emulators. Real hardware is recommended to experience the full 3D visuals and rumble features.

The synchronization of arrows with the music is coded to remain accurate regardless of emulator or hardware speed. However, many emulators tend to run faster than the original hardware, which can make the game play faster and thus be more challenging.

For the most faithful experience, I recommend:
- [Lemur](https://git.virtual-boy.com/PVB/lemur/src/tag/v0.4.3)  for speed accuracy.
- [Red Viper](https://github.com/skyfloogle/red-viper) for its support of stereoscopic 3D effects on the Nintendo 3DS and New 3DS.


## Release and updates

***Virtual Vibes*** is currently in development and planned for release in **2025**.
Stay tuned for updates, demos, and final release details!

This repository is here to share development progress, updates, and documentation. The source code for ***Virtual Vibes*** is proprietary and not available to the public at this time.


## Credits

- 🕹️ **Development & Graphics**: [SylvainRx](https://github.com/SylvainRX).
- 🛠️ **VUEngine Developers**: [Jorgeche](https://github.com/jorgeche) and [KR155E](https://github.com/KR155E).
- 🧪 **Beta Testing**: [JnL](https://discord.com/users/918508493818966036).
- 🎵 **Upcoming Music**: [Tronimal](https://tronimal.bandcamp.com).


## Technical Aspects & Code Samples

### Coordinating Audio and Graphics Processing

The Virtual Boy’s audio is handled by the Virtual Sound Unit (VSU), a dedicated sound processor with six audio channels that operates independently from the CPU. Graphics rendering is managed by the Virtual Image Processor (VIP), another dedicated processor responsible for composing and outputting video frames. Both the VSU and VIP are controlled by the main CPU (the NEC V810), which coordinates their operation asynchronously.

Because the CPU, VSU, and VIP all run on independent clocks, music playback on the VSU proceeds without direct synchronization to the CPU’s processing cycles. Meanwhile, game logic, such as arrow movement, is traditionally updated based on CPU ticks. This asynchronous operation led to desynchronization between the visual arrows (managed by the CPU and rendered by the VIP) and the music playing on the VSU. To overcome this challenge, the game’s CPU code explicitly queries the current playback position (tick) directly from the VSU. This allows arrow movement and spawning to be manually synchronized with the actual music playback timing. As a result, gameplay remains smooth and responsive, staying perfectly in sync with the audio on both real hardware and emulators despite the asynchronous nature of the underlying subsystems.

More information on the [Virtual Boy Architecture](https://www.copetti.org/writings/consoles/virtual-boy/), by Rodrigo Copetti.

### Key Code Samples Overview

This repository includes a curated set of code samples to give you a clear look at how ***Virtual Vibes*** works under the hood. These files illustrate key parts of the engine, asset, and gameplay logic, all written in [**VirtualC**](#powered-by-vuengine); a custom transpiled C-dialect that emulates object-oriented patterns.

#### 💾 Game Systems

- [`source/Game.c`](code-samples/source/Game.c)
The entry point of the game. Sets up and starts the routing system that powers the entire state flow.

- [`source/Components/Router/`](code-samples/source/Components/Router/)
A basic router for switching between different game states (views); such as the main menu, song select, or gameplay.

- [`source/Components/GameSaveDataManager/`](code-samples/source/Components/GameSaveDataManager/)
Manages high score saving and loading using the Virtual Boy’s SRAM.

#### 🎮 Core Entities & Game Flow

- [`source/Entities/Arrow/`](code-samples/source/Entities/Arrow/)
Defines the `Arrow` actor, which uses its spec to render and animate itself on screen. Each arrow dynamically mutates its implementation at the instance level based on its direction, allowing it to compute movement offsets according to the selected scrolling speed.

- [`source/States/SongLevelGameState/`](code-samples/source/States/SongLevelGameState/)
Sets up and runs the full game logic for a song level; this is the core gameplay state. Each screen in the game has its own `GameState`.

#### 🎼 Arrow Timing & Gameplay Logic

- [`assets/ArrowSchedules/MysticQuest/MysticQuestHardArrowSchedule.c`](code-samples/assets/ArrowSchedules/MysticQuest/MysticQuestHardArrowSchedule.c)
Defines an arrow schedule (i.e., the timing of arrows) for a song; essentially the game's note chart.

- [`source/Components/ArrowScheduler/`](code-samples/source/Components/ArrowScheduler/)
Contains logic that uses these arrow schedules to send signals to different lanes, so arrows spawn at the correct time based on the current song tick provided by the `SongPlayer`.

- [`source/Components/Lane/`](code-samples/source/Components/Lane/)
Each lane manages arrow spawning, moves arrows across the screen, and determines whether the player hits them correctly. Lanes also provide the timing offset needed to sync the music with the travel time of arrows to the D-Pad.

- [`source/Components/SongPlayer/`](code-samples/source/Components/SongPlayer/)
Controls music playback during gameplay. It uses a somewhat hacky workaround by adding silent padding at the beginning of the song, allowing the arrow scheduler to start spawning arrows in sync with the song tick from the very first frame.

- [`source/Components/GameDPadInputHandler/`](code-samples/source/Components/GameDPadInputHandler/)
Handles player input from both D-Pads and triggers, and dispatches it to the appropriate lanes for scoring arrows.

#### 🎨 Visual Assets

- [`assets/Image/Arrow/ShortArrow/Converted/`](code-samples/assets/Image/Arrow/ShortArrow/Converted/)
Shows how raw bitmaps are prepared and stored for rendering.

- [`assets/Actor/Arrow/ShortArrow/ArrowUpSpec.c`](code-samples/assets/Actor/Arrow/ShortArrow/ArrowUpSpec.c)
Demonstrates how bitmap assets are used in a "spec" to define how an arrow is visually represented and displayed on screen.


## License

Copyright © 2025 Sylvain Roux. All rights reserved.
[License](LICENSE.md)
