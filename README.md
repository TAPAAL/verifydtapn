# verifydtapn
## Linux

Install the build tool:
```bash
sudo apt update
sudo apt install cmake ninja-build flex bison build-essential
```

Build a release:
```bash
cmake --workflow release
```

## Mac OS

Install xcode through App Store.

Install cmake, gcc, boost and google-sparsehash,
for example using homebrew as follows:

```bash
brew install cmake gcc flex bison ninja
```

Build a release:
```bash
cmake --workflow release
```

## Windows (Cross Compile)

Install MinGW64:
```bash
sudo apt update
sudo apt install cmake ninja-build flex bison mingw-w64
```

Build the Windows release:

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
