/*
 * NonStrictDFS.hpp
 *
 *  Created on: 05/03/2012
 *      Author: MathiasGS
 */

#ifndef NONSTRICTDFS_HPP_
#define NONSTRICTDFS_HPP_

#include "PWList.hpp"
#include "boost/smart_ptr.hpp"
#include "../Core/TAPN/TAPN.hpp"
#include "../Core/QueryParser/AST.hpp"
#include "../Core/VerificationOptions.hpp"

namespace VerifyTAPN {
namespace DiscreteVerification {

class NonStrictDFS {
public:
	NonStrictDFS(boost::shared_ptr<TAPN::TimedArcPetriNet>& tapn, NonStrictMarking& initialMarking, AST::Query* query, VerificationOptions options);
	virtual ~NonStrictDFS();
	bool Verify();
private:
	PWList pwList;
	boost::shared_ptr<TAPN::TimedArcPetriNet>& tapn;
	NonStrictMarking& initialMarking;
	AST::Query* query;
	VerificationOptions options;
};

}
}
#endif /* NONSTRICTDFS_HPP_ */
