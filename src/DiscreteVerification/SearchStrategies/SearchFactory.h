/*
 * SearchFactory.h
 *
 *  Created on: 20/09/2012
 *      Author: jakob
 */

#ifndef SEARCHFACTORY_H_
#define SEARCHFACTORY_H_

#include "SearchStrategies.hpp"
#include "../DataStructures/WaitingList.hpp"

namespace VerifyTAPN {
namespace DiscreteVerification {

template <class T>
WaitingList<T>* GetWaitingList(AST::Query* query, VerificationOptions& options);

}
}

#endif /* SEARCHFACTORY_H_ */
