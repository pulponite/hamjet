### Preparing to build

Before building, the source for the third party dependencies needs to be fetched.
This can either be done by cloning with the `--recursive` flag, or doing the following
in each submodule folder in `hamjet/thirdparty`

    git submodule init
    git submodule update

### Windows

To build on windows as a windows native app:

    cd hamjet
    cmake -H. -Bbuild
    cmake --build build --config Release

This requires Visual Studio to be installed. It will compile the third party
dependencies, and then the hamjet binary itself. There is also now a .sln which
can be used to develop from inside of VS. The binary will be in
`hamjet/build/Release/Hamjet.exe`.

The build for HTML5/JS:

    cd hamjet
    emcmake -H. -Bembuild
    cd embuild
    emmake make

This requires emscripten and mingw be installed, and in the system path. This
will result in a html file in `hamjet/embuild/Hamjet.html`
