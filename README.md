### Raycaster Engine

Made in *GCC*.

You can mess with some of the macros, but I've already tried them out and found the current ones comfortable.

#### Building

To build, you have to clone SDL's SDL2 branch into 'external' as 'SDL2'; you can use:

`git clone --branch SDL2 https://github.com/libsdl-org/SDL.git`

#### Notes
* Uses method 'getline' (*POSIX specific!*)

* You can edit *map.txt*; **1** is a wall, **0** is none for the raycaster