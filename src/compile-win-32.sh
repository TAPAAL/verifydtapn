#!/bin/bash

parser=Core/QueryParser

incw32="-I$HOME/dev/iaw32/include -I$HOME/dev/iaw32/uppaal/include"
libw32="-L$HOME/dev/iaw32/uppaal/lib -ludbm"

src=`find . -name "*.cpp"`

#flex -o $parser/Generated/lexer.cpp $parser/flex.ll
bison -o $parser/Generated/parser.cpp $parser/grammar.yy

i586-mingw32msvc-g++ -DBOOST_DISABLE_THREADS -DNDEBUG -DDISABLE_ASSERTX -static -O3 -Wall \
    $src $incw32 $libw32 -o verifytapn32.exe && \
i586-mingw32msvc-strip verifytapn32.exe && \
echo "32-bit win OK" 

