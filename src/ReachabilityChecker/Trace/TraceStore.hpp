#ifndef TRACESTORE_HPP_
#define TRACESTORE_HPP_


#include "boost/functional/hash.hpp"
#include "google/sparse_hash_map"
#include <deque>
#include "../../Core/VerificationOptions.hpp"
#include "../../typedefs.hpp"
#include "TraceInfo.hpp"
namespace VerifyTAPN
{
	namespace TAPN { class TimedArcPetriNet; }
	class SymbolicMarking;

	class TraceStore
	{
	private:
		typedef google::sparse_hash_map<id_type, TraceInfo, boost::hash<id_type> > HashMap;
	public: // constructors / destructors
		TraceStore(const VerificationOptions& options, SymbolicMarking* initialMarking) : store(), initialMarking(initialMarking), options(options), identity_map(options.GetKBound(), -1)
		{
			for(unsigned int i = 0; i < static_cast<unsigned int>(options.GetKBound()); ++i)
			{
				identity_map[i] = i;
			}
		};
		~TraceStore() {};
	public:
		inline void Save(const id_type& id, const TraceInfo& traceInfo){
			store.insert(std::pair<id_type, TraceInfo>(id, traceInfo));
		};
		inline void SetFinalMarking(SymbolicMarking* marking) { finalMarking = marking; };

		void OutputTraceTo(const TAPN::TimedArcPetriNet& tapn) const;

	private:
		void CalculateDelays(const std::deque<TraceInfo>& traceInfos, std::vector<decimal>& delays) const;

		void ComputeIndexMappings(std::deque<TraceInfo>& traceInfos) const;
		void AugmentSymmetricMappings(std::deque<TraceInfo>& traceInfos) const;
	private: // data
		HashMap store;
		SymbolicMarking* initialMarking;
		SymbolicMarking* finalMarking;
		const VerificationOptions& options;

		std::vector<unsigned int> identity_map;
	};
}

#endif /* TRACESTORE_HPP_ */
