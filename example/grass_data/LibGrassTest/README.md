## Build Guide
- Go into directory `LibGrassTest`
- Put `libgrass.so` into `lib/`
- Build with CMake:
```shell
mkdir build && cd build
cmake ../
make
```
- Executable file will be `build/bin/test_grass`

## Test Guide
- Go into directory `LibGrassTest`
- Run:
```shell
./build/bin/test_grass
```