# Indiana Jones and the Great Circle Fix
[![Patreon-Button](https://raw.githubusercontent.com/Lyall/GreatCircleFix/refs/heads/master/.github/Patreon-Button.png)](https://www.patreon.com/Wintermance) [![ko-fi](https://ko-fi.com/img/githubbutton_sm.svg)](https://ko-fi.com/W7W01UAI9)<br />
[![Github All Releases](https://img.shields.io/github/downloads/Lyall/GreatCircleFix/total.svg)](https://github.com/Lyall/GreatCircleFix/releases)

This is an ASI plugin for Indiana Jones and the Great Circle that fixes cutscene FOV issues when using an ultrawide display and more.

## Features
### General
- Unrestrict available console commands.
- Allow changing read-only console commands.
- Skip intro videos. 

### Ultrawide
- Fix vert- FOV in cutscenes when using the "Fullscreen" picture framing option.
- Fix culling issues at wider aspect ratios.

### Framerate
- Allow frame generation in cutscenes.
- Unlock cutscene framerate.

## Installation
- Grab the latest release of GreatCircleFix from [here.](https://github.com/Lyall/GreatCircleFix/releases)
- Extract the contents of the release zip in to the the game folder.
e.g. ("**XboxGames\Indiana Jones and the Great Circle\Content**" for Xbox or "**steamapps\common\The Great Circle**" for Steam).

### Steam Deck/Linux Additional Instructions
🚩**You do not need to do this if you are using Windows!**
- Open up the game properties in Steam and add `WINEDLLOVERRIDES="dinput8=n,b" %command%` to the launch options.

## Screenshots

| ![ezgif-3-9601bb0c8b](https://github.com/user-attachments/assets/4a721a78-f3f6-496b-8b44-4d32f89f8261) |
|:--------------------------:|
| Main Menu |

## Known Issues
Please report any issues you see.

## Credits
[Ultimate ASI Loader](https://github.com/ThirteenAG/Ultimate-ASI-Loader) for ASI loading. <br />
[spdlog](https://github.com/gabime/spdlog) for logging. <br />
[safetyhook](https://github.com/cursey/safetyhook) for hooking.
