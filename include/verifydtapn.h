/*
 * File:   verifydtapn.h
 * Author: Peter G. Jensen
 *
 * Created on 1 December 2021, 14.54
 */

#ifndef VERIFYDTAPN_H
#define VERIFYDTAPN_H

#include "Core/VerificationOptions.hpp"
#include "Core/TAPN/TAPN.hpp"
#include <Colored/ColoredPetriNetBuilder.h>

#include <memory>
#include <vector>

using namespace VerifyTAPN;
namespace VerifyTAPN {
std::unique_ptr<AST::Query> parse_queries(const VerificationOptions& options, const unfoldtacpn::ColoredPetriNetBuilder& builder, const TAPN::TimedArcPetriNet& net);

std::pair<std::vector<int>,std::unique_ptr<TAPN::TimedArcPetriNet>>
build_net(unfoldtacpn::ColoredPetriNetBuilder& builder);

std::pair<std::vector<int>,std::unique_ptr<TAPN::TimedArcPetriNet>>
parse_net_file(unfoldtacpn::ColoredPetriNetBuilder& builder, const std::string& filename);

std::unique_ptr<AST::Query> make_query(const unfoldtacpn::ColoredPetriNetBuilder& builder, VerificationOptions& options, const TAPN::TimedArcPetriNet& net);
}
#endif /* VERIFYDTAPN_H */

