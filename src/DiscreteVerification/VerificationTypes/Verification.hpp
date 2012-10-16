#ifndef VERIFICATION_HPP_
#define VERIFICATION_HPP_

namespace VerifyTAPN {
namespace DiscreteVerification {

class Verification {
public:
virtual bool Verify() = 0;
virtual ~Verification(){};
virtual void printStats() = 0;
virtual void PrintTransitionStatistics() const = 0;
virtual unsigned int MaxUsedTokens() = 0;
virtual NonStrictMarking* GetLastMarking() = 0;

public:
stack< NonStrictMarking* > trace;
};

}
}

#endif
