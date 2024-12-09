# Indiana Jones and the Great Circle Fix
[![Patreon-Button](https://raw.githubusercontent.com/Lyall/GreatCircleFix/refs/heads/master/.github/Patreon-Button.png)](https://www.patreon.com/Wintermance) [![ko-fi](https://ko-fi.com/img/githubbutton_sm.svg)](https://ko-fi.com/W7W01UAI9)<br />
[![Github All Releases](https://img.shields.io/github/downloads/Lyall/GreatCircleFix/total.svg)](https://github.com/Lyall/GreatCircleFix/releases)

This is an ASI plugin for Indiana Jones and the Great Circle that fixes cutscene FOV issues when using an ultrawide display and more.

## Features
### General
- Remove restrictions on entering certain console commands.

### Ultrawide
- Fixes vert- FOV in cutscenes when using the "Fullscreen" picture framing option.

## Installation
- Grab the latest release of GreatCircleFix from [here.](https://github.com/Lyall/GreatCircleFix/releases)
- Extract the contents of the release zip in to the the game folder.
e.g. ("**XboxGames\Indiana Jones and the Great Circle\Content**" for Xbox).

### Steam Deck/Linux Additional Instructions
🚩**You do not need to do this if you are using Windows!**
- Open up the game properties in Steam and add `WINEDLLOVERRIDES="dinput8=n,b" %command%` to the launch options.

## Screenshots

|  |
|:--------------------------:|
| Cutscene |

## Known Issues
Please report any issues you see.

## Credits
[Ultimate ASI Loader](https://github.com/ThirteenAG/Ultimate-ASI-Loader) for ASI loading. <br />
[inipp](https://github.com/mcmtroffaes/inipp) for ini reading. <br />
[spdlog](https://github.com/gabime/spdlog) for logging. <br />
[safetyhook](https://github.com/cursey/safetyhook) for hooking.
