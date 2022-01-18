/* Copyright (C) 2021 Peter G. Jensen <root@petergjoel.dk>
 *
 * This program is free software: you can redistribute it and/or modify
 * it under the terms of the GNU Lesser General Public License as published by
 * the Free Software Foundation, either version 3 of the License, or
 * (at your option) any later version.
 *
 * This program is distributed in the hope that it will be useful,
 * but WITHOUT ANY WARRANTY; without even the implied warranty of
 * MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
 * GNU Lesser General Public License for more details.
 *
 * You should have received a copy of the GNU Lesser General Public License
 * along with this program.  If not, see <http://www.gnu.org/licenses/>.
 */

#define BOOST_TEST_MODULE load_test


#include <boost/test/unit_test.hpp>
#include <string>
#include <fstream>
#include <sstream>

#include "verifydtapn.h"
#include "Core/TAPN/TAPNModelBuilder.hpp"


using namespace VerifyTAPN;

BOOST_AUTO_TEST_CASE(DirectoryTest) {
    BOOST_REQUIRE(getenv("TEST_FILES"));
}

std::ifstream loadFile(const char* file) {
    std::stringstream ss;
    ss << getenv("TEST_FILES") << "/build_net/" << file;
    std::cerr << "Loading '" << ss.str() << "'" << std::endl;
    return std::ifstream(ss.str());
}

BOOST_AUTO_TEST_CASE(arc_guard)
{
    unfoldtacpn::ColoredPetriNetBuilder builder;
    auto file = loadFile("arc_guard.xml");
    builder.parseNet(file);
    auto [initialPlacement, tapn] = build_net(builder);
    BOOST_REQUIRE_EQUAL(tapn->getTransitions().size(), 1);
    const TimedTransition* trans = tapn->getTransitions()[0];
    BOOST_REQUIRE(trans->isUrgent());
    for(const TimedInputArc* ia : trans->getPreset())
    {
        BOOST_REQUIRE(ia->getInterval().isZeroInfinity());
    }
}

BOOST_AUTO_TEST_CASE(covid_test_model)
{
    unfoldtacpn::ColoredPetriNetBuilder builder;
    auto file = loadFile("covid_test_model.xml");
    builder.parseNet(file);
    auto [initialPlacement, tapn] = build_net(builder);
}

BOOST_AUTO_TEST_CASE(inhib_guard)
{
    unfoldtacpn::ColoredPetriNetBuilder builder;
    auto file = loadFile("inhib_zeroinf_guard.xml");
    builder.parseNet(file);
    auto [initialPlacement, tapn] = build_net(builder);
    BOOST_REQUIRE_EQUAL(tapn->getInputArcs().size(), 0);
    BOOST_REQUIRE_EQUAL(tapn->getOutputArcs().size(), 0);
    BOOST_REQUIRE_EQUAL(tapn->getInhibitorArcs().size(), 1);
    BOOST_REQUIRE_EQUAL(tapn->getTransportArcs().size(), 0);
    BOOST_REQUIRE_EQUAL(tapn->getPlaces().size(), 1);
    const TimedPlace& p = tapn->getPlace(0);
    BOOST_REQUIRE(p.getName() == "P");
    BOOST_REQUIRE_EQUAL(p.getInhibitorArcs().size(), 1);
    BOOST_REQUIRE_EQUAL(tapn->getTransitions().size(), 1);
    const TimedTransition* trans = tapn->getTransitions()[0];
    BOOST_REQUIRE_EQUAL(trans->getIndex(), 0);
    BOOST_REQUIRE_EQUAL(trans->getInhibitorArcs().size(), 1);
    BOOST_REQUIRE_EQUAL(trans->getNumberOfInputArcs(), 0);
    BOOST_REQUIRE_EQUAL(trans->getNumberOfTransportArcs(), 0);

}
