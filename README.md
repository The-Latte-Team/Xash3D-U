# Xash3D U <img align="right" width="128" height="128" src="https://github.com/The-Latte-Team/Xash3D-U/raw/main/engine/platform/wiiu/icon.png" alt="Xash3D U icon" />

# HEADS UP!
## This is still a WIP, if you compile the game, it will crash due to it being extremely incomplete

[Xash3D FWGS](hhttps://github.com/FWGS/xash3d-fwgs) is a game engine, aimed to provide compatibility with Half-Life Engine and extend it, as well as to give game developers well known workflow.

Xash3D FWGS is a heavily modified fork of an original [Xash3D Engine](https://www.moddb.com/engines/xash3d-engine) by Unkle Mike.

## Donate to the original project
[![Donate to FWGS button](https://img.shields.io/badge/Donate_to_FWGS-%3C3-magenta)](https://github.com/FWGS/xash3d-fwgs/blob/master/Documentation/donate.md) \
If you like Xash3D FWGS, consider supporting individual engine maintainers. By supporting us, you help to continue developing this game engine further. The sponsorship links are available in [documentation](https://github.com/FWGS/xash3d-fwgs/blob/master/Documentation/donate.md).

## Fork features
* Steam Half-Life (HLSDK 2.4) support.
* Crossplatform and modern compilers support: supports Windows, Linux, BSD & Android on x86 & ARM and [many more](Documentation/ports.md).
* Better multiplayer support: multiple master servers, headless dedicated server, voice chat and IPv6 support.
* Multiple renderers support: OpenGL, GLESv1, GLESv2 and Software.
* Advanced virtual filesystem: `.pk3` and `.pk3dir` support, compatibility with GoldSrc FS module, fast case-insensitivity emulation for crossplatform.
* Mobility API: better game integration on mobile devices (vibration, touch controls)
* Different input methods: touch and gamepad in addition to mouse & keyboard.
* TrueType font rendering, as a part of mainui_cpp.
* External VGUI support module.
* PNG & KTX2 image format support.
* [A set of small improvements](Documentation/), without broken compatibility.

## Contributing
* Before sending an issue, check if someone already reported your issue. Make sure you're following "How To Ask Questions The Smart Way" guide by Eric Steven Raymond. Read more: http://www.catb.org/~esr/faqs/smart-questions.html
* Issues are accepted in both English and Spanish
* Before sending a PR, check if you followed our contribution guide in CONTRIBUTING.md file.

## Build instructions
For this specific port, I'm using a devkitPro Makefile, which is far from the Waf system (especially self documentation wise), but it works

NOTE: NEVER USE GitHub's ZIP ARCHIVES. GitHub doesn't include external dependencies we're using!

### Prerequisites

- [wut](https://github.com/devkitPro/wut)
- [wiiu-sdl2](https://github.com/devkitPro/SDL/tree/wiiu-sdl2-2.28)
- wiiu-sdl2_gfx
- wiiu-sdl2_mixer
- wiiu-sdl2_ttf
- wiiu-curl

### Building

Just navigate to the root folder of the project, type `make` on a new terminal window and it should start to compile