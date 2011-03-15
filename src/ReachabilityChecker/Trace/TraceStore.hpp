#ifndef TRACESTORE_HPP_
#define TRACESTORE_HPP_

#include "TraceInfo.hpp"
#include "../../Core/SymbolicMarking/SymbolicMarking.hpp"
#include "boost/functional/hash.hpp"
#include "google/sparse_hash_map"
#include "../../../lib/rapidxml-1.13/rapidxml.hpp"
#include <deque>
#include "../../Core/VerificationOptions.hpp"
#include "EntrySolver.hpp"

namespace VerifyTAPN
{
	namespace TAPN { class TimedArcPetriNet; }

	class TraceStore
	{
	private:
		typedef google::sparse_hash_map<id_type, TraceInfo, boost::hash<id_type> > HashMap;
	public: // constructors / destructors
		TraceStore(const VerificationOptions& options, SymbolicMarking* initialMarking) : store(), initialMarking(initialMarking), options(options) { };
		~TraceStore() {};

	public:
		inline void Save(const id_type& id, const TraceInfo& traceInfo){
			store.insert(std::pair<id_type, TraceInfo>(id, traceInfo));
		};
		inline void SetFinalMarking(SymbolicMarking* marking) { finalMarking = marking; };

		void OutputTraceTo(const TAPN::TimedArcPetriNet& tapn) const;

	private:
		void CalculateDelays(const std::deque<TraceInfo>& traceInfos, std::vector<decimal>& delays) const;
//		rapidxml::xml_document<>* CreateInputDocForCTU(const std::deque<TraceInfo>& traceInfos) const;
//		void GenerateSystemDescription(rapidxml::xml_document<>& doc, rapidxml::xml_node<>& root, const std::deque<TraceInfo>& traceInfos) const;
//		std::string GenerateGuardFromTraceInfo(const TraceInfo& traceInfo) const;
//		std::string GenerateUpdateFromTraceInfo(const TraceInfo& traceInfo) const;
//		void GenerateLocationVectors(rapidxml::xml_document<>& doc, rapidxml::xml_node<>& root, const std::deque<TraceInfo>& traceInfos) const;
//		void GenerateNodeDescriptions(rapidxml::xml_document<>& doc, rapidxml::xml_node<>& root, const std::deque<TraceInfo>& traceInfos) const;
//		void GenerateDBMS(rapidxml::xml_document<>& doc, rapidxml::xml_node<>& root, const std::deque<TraceInfo>& traceInfos) const;
//		void GenerateDBMDescription(rapidxml::xml_document<>& doc, rapidxml::xml_node<>& dbmNode, const dbm::dbm_t& info) const;
//		void GenerateStates(rapidxml::xml_document<>& doc, rapidxml::xml_node<>& root, const std::deque<TraceInfo>& traceInfos) const;
//
//		void RunCTUToObtainDelays(rapidxml::xml_document<>& doc, std::vector<double>& delays) const;
//		void RunCTU(const std::string& tempFile, std::string& output) const;
//		void ParseDelays(const std::string& input, std::vector<double>& delays) const;
	private: // data
		HashMap store;
		SymbolicMarking* initialMarking;
		SymbolicMarking* finalMarking;
		const VerificationOptions& options;
	};
}

#endif /* TRACESTORE_HPP_ */
