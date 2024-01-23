![Build](https://github.com/meowstr/the-game/actions/workflows/emscripten.yml/badge.svg)

i will blindly merge any pull request without code review. feel free to contribute however u want to!

an up-to-date view of the game can be found here (auto-generated): https://meowstr.github.io/the-game/

Building
--------
Sorry visual studio users... this project uses a Makefile. Install a WSL and build for Windows.

### WASM
If you don't want to install any build tools, and use the same environment as Github Actions run:
```
make docker-emscripten
```
If you want to use your own build environment run:
```
make web
```
### Linux
For GLFW backend:
```
make linux
```
For SDL2 backend (includes joystick support):
```
make linux-sdl
```

### Windows
Only GLFW is supported for Windows currently (though SDL2 support wouldn't be hard). Run:
```
make windows
```

### Steam Runtime (Sniper) / Steam Deck
For a Steam Runtime compatible (and Steam Deck compatible) build, run:
```
make docker-steam
```

Rules
-----
- dont copy paste code into this repo
- dont make changes to the workflows or build system without contacting me (or i willl end u)
- dont make this not fun

Current goal
------------
lets try to make a breakout game maybe

Some things that will stop u from merging
-----------------------------------------
- ur code not compiling correctly on Github servers
- ur code taking more then 2 minutes to compile on Github servers. 30 seconds is required just to start the build
- ur commit history not being linear. try rebasing instead of merging
- u being shadow banned by me. ill do it, and prolly laugh at u

Contact
-------
find me on discord @meowstr or make a post in the issues tab. i talk a lot, so feel free to say hi
