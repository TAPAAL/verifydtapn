# verifydtapn
## Linux

Install dependencies:
```bash
sudo apt update
sudo apt install cmake ninja-build flex bison gcc-16 g++-16
```

Build a release:
```bash
cmake --workflow release
```

## Mac OS

Install xcode through App Store.

Install dependencies:

```bash
brew install cmake ninja flex bison gcc@16
```

Build a release:
```bash
cmake --workflow release
```

## Windows (Cross Compile)

Install dependencies:
```bash
sudo apt update
sudo apt install cmake ninja-build flex bison mingw-w64
```

Build a Windows release:

```bash
cmake --workflow win64-release
```

## CMake Workflows

| Workflow | Purpose | Build directory |
| --- | --- | --- |
| `release` | Release build | `build-release` |
| `debug` | Debug build | `build-debug` |
| `test` | Release build and tests | `build-test` |
| `win64-release` | Windows cross-compiled release | `build-win64-release` |

The default GCC version can be overridden by setting the `GCC_VERSION` environment variable:
```bash
GCC_VERSION=12 cmake --workflow release
```
