# Chip8 Emulator

A fully featured Chip8 Emulator written in C++  for fun!

## Build Instructions

You will need `CMake` version 3.8 or higher.

* Clone the repository in a directory on your machine

* Run `CMake` build system generation by either using editor features or by running ```cmake -B"./build"```

* Build the executable using the editor features or by running ```cmake --build "./build"```

## Usage

Simply Run `Chip8Emulator.exe` and have fun!

The key bindings are the following:

| Key | Emulator Pad |
| ---- | ----- |
| 1 | 1 |
| 2 | 2 |
| 3 | 3 |
| 4 | C |
| Q | 4 |
| W | 5 |
| E | 6 |
| R | D |
| A | 7 |
| S | 8 |
| D | 9 |
| F | E |
| Z | A |
| X | 0 |
| C | B |
| V | F |

**NOTE:** There is currently no loading solution for your own ROMs as this project is intended for demonstration only, but it will be implemented soon enough;
in the meantime You can bring your TicTacToe skills to the next level :wink:

## Tests

The tests are built together with the main executable unless differently specified.
Full test coverage is not yet present, but all the current tests are passing so it is stable enough to fiddle with.

## ThirdParty

* The app uses [`SDL2`](https://github.com/libsdl-org/SDL) library to render the game, it is already included in the repo to make your experience as seamless as possible

* The tests are implemented using [`CLove-Unit`](https://github.com/fdefelici/clove-unit) header-only library, which is already included into the repository
