#include "TraceStore.hpp"
#include "../../Core/TAPN/TAPN.hpp"
#include "TraceInfo.hpp"
#include "EntrySolver.hpp"
#include "../../Core/SymbolicMarking/SymbolicMarking.hpp"

#include "dbm/print.h"
#include "../../Core/SymbolicMarking/DBMMarking.hpp"



namespace VerifyTAPN{
	class Token{
		friend bool operator==(const Token&, const Token&);
		friend std::ostream& operator<<( std::ostream&, const Token& );
	public:
		Token(const std::string& place) : place(place), age(0) {};
		Token(const std::string& place, double age) : place(place), age(age) {};
		inline void Reset() { age = 0; }
		inline void Delay(double amount) { age += amount; }
		inline void SetPlace(const std::string& place) { this->place = place; }
	private:
		std::string place;
		double age;
	};

	std::ostream& operator<<( std::ostream& out, const Token& token )
	{
		out << "(" << token.place << "," << token.age << ")";
		return out;
	}

	bool operator==( const Token& a, const Token& b)
	{
		return a.place == b.place && a.age == b.age;
	}

	bool operator!=( const Token& a, const Token& b)
	{
		return !(a == b);
	}


	class ConcreteMarking{
		friend std::ostream& operator<<( std::ostream&, const ConcreteMarking& );
	public:
		ConcreteMarking() : tokens() { };

		inline void Add(const Token& token) { tokens.push_back(token); }
		inline void Remove(const Token& token) { tokens.erase(find(tokens.begin(), tokens.end(), token)); }
		inline void Remove(unsigned int i) { tokens.erase(tokens.begin() + i); };
		inline void Delay(double amount)
		{
			for(std::deque<Token>::iterator iter = tokens.begin(); iter != tokens.end(); ++iter)
				iter->Delay(amount);
		}
		Token& operator[](int i)
		{
			assert(i < static_cast<int>(tokens.size()));
			return tokens[i];
		}
	private:
		std::deque<Token> tokens;
	};

	std::ostream& operator<<( std::ostream& out, const ConcreteMarking& marking )
	{
		out << "Marking: ";
		for(std::deque<Token>::const_iterator iter = marking.tokens.begin(); iter != marking.tokens.end(); ++iter)
		{
			out << *iter << " ";
		}
		out << std::endl;
		return out;
	}

	ConcreteMarking CreateConcreteInitialMarking(SymbolicMarking* initialMarking, const VerifyTAPN::TAPN::TimedArcPetriNet& tapn)
	{
		ConcreteMarking marking;
		for(unsigned int i = 0; i < initialMarking->NumberOfTokens(); i++)
		{
			Token token(tapn.GetPlace(initialMarking->GetTokenPlacement(i)).GetName());
			marking.Add(token);
		}
		return marking;
	}

//	void UpdateMarking(ConcreteMarking& marking, const TraceInfo& traceInfo, const VerifyTAPN::TAPN::TimedArcPetriNet& tapn)
//	{
//		const std::vector<Participant>& participants = traceInfo.Participants();
//		for(std::vector<Participant>::const_iterator iter = participants.begin(); iter != participants.end(); ++iter)
//		{
//			if(iter->TokenIndex() != -1)
//			{
//				if(iter->IndexAfterFiring() == -1)
//				{
//					marking.Remove(iter->TokenIndex());
//				}else{
//					Token& token = marking[iter->TokenIndex()];
//					int placeIndex = traceInfo.Marking().GetTokenPlacement(iter->IndexAfterFiring());
//					token.SetPlace(tapn.GetPlace(placeIndex).GetName());
//					token.Reset();
//				}
//			}
//			else
//			{
//				int placeIndex = traceInfo.Marking().GetTokenPlacement(iter->IndexAfterFiring());
//				marking.Add(Token(tapn.GetPlace(placeIndex).GetName()));
//			}
//		}
//	}

	void dumpTraceInfo(const TraceInfo& traceInfo, const TAPN::TimedArcPetriNet& tapn)
	{
		std::cout << tapn.GetTransitions()[traceInfo.TransitionIndex()]->GetName() << " (table size: " << traceInfo.GetSymmetricMapping().Size() << ")\n";
		std::cout << "  Participants:\n";
		for(std::vector<Participant>::const_iterator it = traceInfo.Participants().begin(); it != traceInfo.Participants().end(); it++)
		{
			std::cout << "  " << it->TokenIndex() << ":" << it->ClockIndex() << ":" << it->IndexAfterFiring() << ":" << it->ClockIndexAfterDiscreteUpdate() << "\n";
		}
		std::cout << "\n";
		std::cout << "\n  Transition firing indirection table:\n";
		for(unsigned int i = 0; i < traceInfo.GetTransitionFiringMapping().Size(); i++)
		{
			std::cout << "  " << i << " <--> " << traceInfo.GetTransitionFiringMapping().MapForward(i) << ", ";
		}
		std::cout << "\n";
		std::cout <<  "\n  Symmetry indirection table:\n";
		for(unsigned int i = 0; i < traceInfo.GetSymmetricMapping().Size(); i++)
		{
			std::cout << "  " << i << " <--> " << traceInfo.GetSymmetricMapping().MapForward(i) << ", ";
		}

		std::cout << "\n";
		std::cout << "\n  OriginalMapping:\n";
		for(unsigned int i = 0; i < traceInfo.GetOriginalMapping().size(); i++)
		{
			std::cout << "  " << i << ":" << traceInfo.GetOriginalMapping()[i] << ", ";
		}
		std::cout << "\n\n";
	}

	void print(const SymbolicMarking& marking)
	{
		const DBMMarking& dbmMarking = static_cast<const DBMMarking&>(marking);
		std::cout << "id: " << dbmMarking.UniqueId() << "\n";
		std::cout << "dp: ";
		for(unsigned int i = 0; i < dbmMarking.NumberOfTokens(); i++)
		{
			if(i != 0) std::cout << ", ";
			std::cout << dbmMarking.GetTokenPlacement(i);
		}
		std::cout << "\nmapping: ";
		for(unsigned int i = 0; i < dbmMarking.NumberOfTokens(); i++)
		{
			if(i != 0) std::cout << ", ";
			std::cout << i << ":" << dbmMarking.GetClockIndex(i);
		}
		std::cout << "\ndbm:\n";
		std::cout << dbmMarking.GetDBM();
		std::cout << "\n";
	}

	void TraceStore::AugmentSymmetricMappings(std::deque<TraceInfo>& traceInfos) const
	{
		for(std::deque<TraceInfo>::iterator it = traceInfos.begin(); it != traceInfos.end(); ++it)
		{
			IndirectionTable& table = it->GetSymmetricMapping();
			for(unsigned int i = table.Size(); i < static_cast<unsigned int>(options.GetKBound()); i++)
			{
				table.AddMapping(i,i);
			}
		}
	}

	void TraceStore::ComputeIndexMappings(std::deque<TraceInfo>& traceInfos) const
	{
		std::vector<unsigned int> map(identity_map);
		TraceInfo& first = traceInfos[0];
		const IndirectionTable& table = first.GetSymmetricMapping();
		for(unsigned int symmetric_index = 0; symmetric_index < static_cast<unsigned int>(options.GetKBound()); symmetric_index++)
		{
			map[symmetric_index] = symmetric_index >= table.Size() ? symmetric_index : table.MapBackwards(symmetric_index);
		}
		first.SetOriginalMapping(map);

		for(unsigned int i = 1; i < traceInfos.size(); i++)
		{
			const TraceInfo& previous = traceInfos[i-1];
			TraceInfo& current = traceInfos[i];

			const std::vector<unsigned int>& previousTable = previous.GetOriginalMapping();
			const IndirectionTable& currentTable = current.GetSymmetricMapping();
			const IndirectionTable& transitionMapping = current.GetTransitionFiringMapping();

			std::vector<unsigned int> mapping(identity_map);
			for(unsigned int token = 0; token < static_cast<unsigned int>(options.GetKBound()); token++)
			{
				unsigned int remapped_index = token >= currentTable.Size() ? token : currentTable.MapBackwards(token);
				int indexBeforeTransitionFiring = transitionMapping.MapBackwards(remapped_index);
				assert(indexBeforeTransitionFiring != -1);
				int originalIndex = previousTable[indexBeforeTransitionFiring];
				assert(originalIndex != -1);
				mapping[token] = originalIndex;
			}

			current.SetOriginalMapping(mapping);
		}
	}

	void TraceStore::OutputTraceTo(const TAPN::TimedArcPetriNet& tapn) const
	{
		id_type currentId = finalMarking->UniqueId();
		std::deque<TraceInfo> traceInfos;
		while(currentId != 0){
			const TraceInfo& info = store.find(currentId)->second;
			traceInfos.push_front(info);
			currentId = info.PreviousStateId();
		}

		AugmentSymmetricMappings(traceInfos);
		ComputeIndexMappings(traceInfos);

		print(*initialMarking);
		for(unsigned int i = 0; i < traceInfos.size(); ++i)
		{
			const TraceInfo& traceInfo = traceInfos[i];
			dumpTraceInfo(traceInfo, tapn);
			print(traceInfo.Marking());
		}


		std::vector<decimal> delays;
		CalculateDelays(traceInfos, delays);


		TAPN::TimedTransition::Vector transitions = tapn.GetTransitions();
		std::cerr << std::endl << "Trace: " << std::endl;
//		ConcreteMarking marking = CreateConcreteInitialMarking(this->initialMarking, tapn);
//		std::cerr << "\t" << marking;
//		print(*initialMarking);
		for(unsigned int i = 0; i < traceInfos.size(); ++i)
		{
			const TraceInfo& traceInfo = traceInfos[i];

			int index = traceInfo.TransitionIndex();
			const TAPN::TimedTransition& transition = *transitions[index];

			if(delays[i] > decimal(0)){
				std::cerr << "\tDelay: " << delays[i] << std::endl;
//				marking.Delay(delays[i]);
//				std::cerr << "\t" << marking;
			}
			std::cerr << "\tTransition: " << transition.GetName()  << std::endl;
//			UpdateMarking(marking, traceInfo, tapn);
//			std::cerr << "\t" << marking;
		}
	}

	void TraceStore::CalculateDelays(const std::deque<TraceInfo>& traceInfos, std::vector<decimal>& delays) const
	{
		EntrySolver solver(options.GetKBound(), traceInfos);
		std::vector<decimal> calculatedDelays = solver.CalculateDelays();
		delays.swap(calculatedDelays);
	}
}
