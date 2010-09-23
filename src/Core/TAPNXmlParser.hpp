#ifndef VERIFYTAPN_TAPNXMLPARSER_HPP_
#define VERIFYTAPN_TAPNXMLPARSER_HPP_

#include "../TAPN/TAPN.hpp"
#include "boost/smart_ptr.hpp"

namespace VerifyTAPN {
	using namespace VerifyTAPN::TAPN;

	class TAPNXmlParser
	{
	public: // Construction
		TAPNXmlParser();
		virtual ~TAPNXmlParser() { }

	public:
	//	boost::shared_ptr<TimedArcPetriNet> Parse(const std::string& filename) const;
	};
}

#endif /* VERIFYTAPN_TAPNXMLPARSER_HPP_ */
