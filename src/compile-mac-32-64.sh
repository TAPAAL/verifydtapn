#!/bin/bash

parser=Core/QueryParser

export CPLUS_INCLUDE_PATH=$HOME/dev/boost_1_46_1

inc64="-I$HOME/dev/ia64/include"

inc32="-I$HOME/dev/ia32/include"

src=`find . -name "*.cpp"`

#flex -o $parser/Generated/lexer.cpp $parser/flex.ll
bison -o $parser/Generated/parser.cpp $parser/grammar.yy

g++ -DBOOST_DISABLE_THREADS -DNDEBUG -DDISABLE_ASSERTX -O3 -Wall -mtune=core2 \
    $src $inc64 -o verifydtapn64 && \
echo "64-bit mac OK" &

g++ -DBOOST_DISABLE_THREADS -DNDEBUG -DDISABLE_ASSERTX -O3 -Wall -mtune=core2 -m32 \
    $src $inc32 -o verifydtapn32 && \
echo "32-bit mac OK"  &

wait
