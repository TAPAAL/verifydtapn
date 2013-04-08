#!/bin/bash

parser=Core/QueryParser

inc64="-I$HOME/dev/ia64/include"

inc32="-I$HOME/dev/ia32/include"

incw32="-I$HOME/dev/iaw32/include"

src=`find . -name "*.cpp"`

flex -o $parser/Generated/lexer.cpp $parser/flex.ll
bison -o $parser/Generated/parser.cpp $parser/grammar.yy

g++ -fpermissive  -DBOOST_DISABLE_THREADS -DNDEBUG -DDISABLE_ASSERTX -static -O3 -Wall -mtune=core2 \
    $src $inc64 -o verifydtapn64 && \
strip verifydtapn64 && \
echo "64-bit linux OK" &

#g++-4.4  -DBOOST_DISABLE_THREADS -DNDEBUG -DDISABLE_ASSERTX -static -O3 -Wall -mtune=core2 -m32 \
#    $src $inc32 -o verifydtapn32 && \
#strip verifydtapn32 && \
#echo "32-bit linux OK" &

wait
