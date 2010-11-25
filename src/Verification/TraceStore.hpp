#ifndef TRACESTORE_HPP_
#define TRACESTORE_HPP_

#include "TraceInfo.hpp"
#include "../Core/SymMarking.hpp"
#include "boost/functional/hash.hpp"
#include "google/sparse_hash_map"
#include "../../lib/rapidxml-1.13/rapidxml.hpp"

namespace VerifyTAPN
{
	namespace TAPN{ class TimedArcPetriNet; }

	class TraceStore
	{
	private:
		typedef google::sparse_hash_map<SymMarking::id_type, TraceInfo, boost::hash<SymMarking::id_type> > HashMap;
	public: // constructors / destructors
		TraceStore(int kbound) : store(), kbound(kbound) { };
		~TraceStore() { };

	public:
		inline void Save(const SymMarking::id_type& id, const TraceInfo& traceInfo){
			store.insert(std::pair<SymMarking::id_type, TraceInfo>(id, traceInfo));
		};

		void OutputTraceTo(const SymMarking::id_type& id, const TAPN::TimedArcPetriNet& tapn) const;

	private:
		rapidxml::xml_document<>* CreateInputDocForCTU(const std::vector<TraceInfo>& traceInfos) const;
		void GenerateSystemDescription(rapidxml::xml_document<>& doc, rapidxml::xml_node<>& root, const std::vector<TraceInfo>& traceInfos) const;
		std::string GenerateGuardFromTraceInfo(const TraceInfo& traceInfo) const;
		std::string GenerateUpdateFromTraceInfo(const TraceInfo& traceInfo) const;
	private: // data
		HashMap store;
		int kbound;
	};
}

#endif /* TRACESTORE_HPP_ */
