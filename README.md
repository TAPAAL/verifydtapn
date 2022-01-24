# verifydtapn

As a temporary solution, run cmake (see below) twice:

``` bash 
cmake -DCMAKE_BUILD_TYPE=Prebuild ..
make
cmake -DCMAKE_BUILD_TYPE=Release ..
make
```

## Linux
To compile verifydtapn:

``` bash 
sudo apt update
sudo apt install flex bison libboost-all-dev libsparsehash-dev cmake build-essential

mkdir build && cd build
cmake ..
make
```

## Max OS X

Install xcode through App Store.

Install cmake, gcc, boost and google-sparsehash,
for example using homebrew as follows:

``` bash
brew install cmake gcc boost google-sparsehash 
```

Set the correct gcc compiler

``` bash
export CC=gcc-11
export CXX=g++-11
```

If your flex and bison point to a wrong binary, run the cmake
with the folowing switches where the desired path is set:

``` bash
cmake -DBISON_EXECUTABLE=/usr/local/opt/bison/bin/bison -DFLEX_EXECUTABLE=/usr/local/opt/flex/bin/flex ..
cmake -DBISON_EXECUTABLE=/opt/homebrew/opt/bison/bin/bison -DFLEX_EXECUTABLE=/opt/homebrew/opt/flex/bin/flex   ..
```

## Windows (Cross Compile)

Install MinGW64
``` bash
sudo apt install mingw-w64-x86-64-dev mingw-w64-tools g++-mingw-w64-x86-64
```


Setup Build Env
``` bash
TARGET=x86_64-w64-mingw32
ADDRM=64
MTUNE=generic

CORES=$(nproc || 1)

export CC=x86_64-w64-mingw32-gcc
export CXX=x86_64-w64-mingw32-g++

export PREFIX=$(mktemp -d -p .) #Or set it to path where libs are saved
export PREFIX=$(realpath $PREFIX)

```

Download and compile Boost 
``` bash

BOOST=boost_1_72_0
VER=${BOOST#boost_}
VER=${VER//_/.}
wget -nv "https://boostorg.jfrog.io/artifactory/main/release/$VER/source/$BOOST.tar.bz2"
tar xf $BOOST.tar.bz2

cd $BOOST
CC= CXX= ./bootstrap.sh --prefix="$PREFIX" --without-icu
cat > win$ADDRM-config.jam << EOF
using gcc : m : $CXX
        :
;
EOF

./b2 toolset=gcc-m target-os=windows cflags="-mtune=$MTUNE" cxxstd=17 cxxflags="-mtune=$MTUNE" address-model="$ADDRM" binary-format="pe" abi="ms" threading="multi" threadapi="win32" variant=release --user-config="win$ADDRM-config.jam" --prefix="$PREFIX" --without-mpi --without-python --without-coroutine --without-graph --without-graph_parallel --without-wave --without-context -sNO_BZIP2=1 -j$CORES install
cd ..
```

Download and build google sparsehash

``` bash

sudo apt install unzip
wget https://github.com/sparsehash/sparsehash/archive/sparsehash-2.0.4.zip
unzip sparsehash-2.0.4.zip

cd sparsehash-sparsehash-2.0.4

./configure --prefix=$PREFIX CXXFLAGS=-std=c++11 --host win
make
make install

cd ..
```

Build verifydtapn

``` bash

mkdir build-win && cd build-win
cmake ../ -DCMAKE_TOOLCHAIN_FILE=../toolchain-x86_64-w64-mingw32.cmake -DBOOST_ROOT=$PREFIX
CPATH=$PREFIX/include make

```
