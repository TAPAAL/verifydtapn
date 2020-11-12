#ifndef DRIVER_HPP_
#define DRIVER_HPP_

#include "Core/TAPN/TimedArcPetriNet.hpp"

#include <string>

namespace VerifyTAPN {

    class location;
    namespace AST {
        class Query;
    }

    class TAPNQueryParser {
    public:
        explicit TAPNQueryParser(const VerifyTAPN::TAPN::TimedArcPetriNet &tapn) : ast(nullptr), net(tapn) {};

        virtual ~TAPNQueryParser() = default;

    private:
        void scan_begin();

        void scan_end();

    public:
        int parse(const std::string &file);

        void setAST(VerifyTAPN::AST::Query *ast);

        AST::Query *getAST();

        const VerifyTAPN::TAPN::TimedArcPetriNet &getTAPN() { return net; };

    public: // error handling
        static void error(const location &l, const std::string &m);

        static void error(const std::string &m);

        // must be public for the generated lexer and parser to work!
        std::string file;
    private:
        VerifyTAPN::AST::Query *ast;
        const VerifyTAPN::TAPN::TimedArcPetriNet &net;
    };
}

#endif /* DRIVER_HPP_ */
