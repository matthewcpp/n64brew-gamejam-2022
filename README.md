# n64brew-gamejam-2022
Entry into N64Brew 2022 GameJam

### Requirements
- Docker
- NodeJS

Additionally for desktop dev build (recommended)
- CMake
- vcpkg

### Quick Setup
```bash
git clone --recursive https://github.com/matthewcpp/n64brew-gamejam-2022.git
cd n64brew-gamejam-2022
npm install
npm run prepare-game-assets n64 dead_ritual
npm run prepare-game-assets desktop dead_ritual
```

Refer to detailed instructions for [Desktop](https://github.com/matthewcpp/framework64/wiki/Building-For-Desktop) or [N64](https://github.com/matthewcpp/framework64/wiki/Building-for-N64)

### Build Targets
|Name| Description|
|---|---|
|dead_ritual| main game|
|zombie_config| small tool to view zombie animations and color palettes|

