#ifndef ARGSPARSER_HPP_
#define ARGSPARSER_HPP_

#include "VerificationOptions.hpp"
#include <argparse/argparse.hpp>

#include <utility>
#include <vector>
#include <map>
#include <string>
#include <iosfwd>

namespace VerifyTAPN {

    class ArgsParser {

    public:
        ArgsParser();

        VerificationOptions parse(int argc, char *argv[]);

    };
}

#endif /* ARGSPARSER_HPP_ */
