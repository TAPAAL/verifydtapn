/*
 * WaitingList.hpp
 *
 *  Created on: 01/03/2012
 *      Author: MathiasGS
 */

#ifndef WAITINGLIST_HPP_
#define WAITINGLIST_HPP_

#include "Core/QueryParser/AST.hpp"
#include "Core/QueryParser/NormalizationVisitor.hpp"

#include "DiscreteVerification/DataStructures/NonStrictMarking.hpp"
#include "DiscreteVerification/DataStructures/TimeDart.hpp"
#include "DiscreteVerification/SearchStrategies/WeightQueryVisitor.hpp"
#include "DiscreteVerification/SearchStrategies/LivenessWeightQueryVisitor.hpp"

#include <queue>
#include <stack>
#include <vector>
#include <queue>
#include <deque>
#include <stack>
#include <vector>
#include <cassert>

namespace VerifyTAPN::DiscreteVerification {

        template<class T>
        class WaitingList {
        public:
            WaitingList() = default;;

            virtual ~WaitingList() = default;;

            virtual void add(NonStrictMarkingBase *weight, T payload) = 0;

            virtual T peek() = 0;

            virtual T pop() = 0;

            virtual size_t size() = 0;

            virtual void flushBuffer() {};

            template<class S>
            friend std::ostream &operator<<(std::ostream &out, WaitingList<S> &x);
        };

        template<class T>
        class StackWaitingList : public WaitingList<T> {
        public:
            StackWaitingList() : stack() {};

            virtual ~StackWaitingList();

        public:
            virtual void add(NonStrictMarkingBase *weight, T payload);

            virtual T peek();

            virtual T pop();

            virtual size_t size() { return stack.size(); };

            virtual void flushBuffer() {};
        protected:
            std::stack<T> stack;
        };

        template<class T>
        struct WeightedItem {
            T item;
            int weight;
        };

        template<class T>
        struct less : public std::binary_function<WeightedItem<T>, WeightedItem<T>, bool> {
            bool operator()(const WeightedItem<T> x, const WeightedItem<T> y) const {
                return x.weight > y.weight;
            }
        };

        template<class T>
        class HeuristicStackWaitingList : public StackWaitingList<T> {
        public:
            typedef std::priority_queue<WeightedItem<T>, std::vector<WeightedItem<T> >, less<T> > priority_queue;

            explicit HeuristicStackWaitingList(AST::Query *q) : buffer(), query(normalizeQuery(q)) {};

            ~HeuristicStackWaitingList() = default;;

            virtual void add(NonStrictMarkingBase *weight, T payload);

            virtual T peek();

            virtual T pop();

            virtual size_t size() { return this->stack.size() + buffer.size(); };

            virtual void flushBuffer();

        protected:
            virtual int calculateWeight(NonStrictMarkingBase *marking);

            AST::Query *normalizeQuery(AST::Query *q);

            priority_queue buffer;
            AST::Query *query;
        };

        template<class T>
        class QueueWaitingList : public WaitingList<T> {
        public:
            QueueWaitingList() : queue() {};

            virtual ~QueueWaitingList();

        public:
            virtual void add(NonStrictMarkingBase *weight, T payload);

            virtual T peek();

            virtual T pop();

            virtual size_t size() { return queue.size(); };
        private:
            std::queue<T> queue;
        };

        template<class T>
        class HeuristicWaitingList : public WaitingList<T> {
        public:
            typedef std::priority_queue<WeightedItem<T>, std::vector<WeightedItem<T> >, less<T> > priority_queue;
        public:
            explicit HeuristicWaitingList(AST::Query *q) : queue(), query(normalizeQuery(q)) {};

            virtual ~HeuristicWaitingList();

        public:
            virtual void add(NonStrictMarkingBase *weight, T payload);

            virtual T peek();

            virtual T pop();

            virtual size_t size() { return queue.size(); };
        protected:
            AST::Query *normalizeQuery(AST::Query *q);

            virtual int calculateWeight(NonStrictMarkingBase *marking);

            priority_queue queue;
            AST::Query *query;
        };

        template<class T>
        class MinFirstWaitingList : public WaitingList<T> {
        public:
            typedef std::priority_queue<WeightedItem<T>, std::vector<WeightedItem<T> >, less<T> > priority_queue;
        public:
            explicit MinFirstWaitingList(AST::Query *q) : queue(), query(q) {};

            ~MinFirstWaitingList() override;

        public:
            void add(NonStrictMarkingBase *weight, T payload) override;

            T peek() override;

            T pop() override;

            size_t size() override { return queue.size(); };
        protected:
            virtual int calculateWeight(T payload);

            priority_queue queue;
            AST::Query *query;
        };

        template<class T>
        class RandomStackWaitingList : public StackWaitingList<T> {
        public:
            typedef std::priority_queue<WeightedItem<T>, std::vector<WeightedItem<T> >, less<T> > priority_queue;
        public:
            RandomStackWaitingList() : buffer() {};

            virtual ~RandomStackWaitingList();

        public:
            virtual void add(NonStrictMarkingBase *weight, T payload);

            virtual T peek();

            virtual T pop();

            virtual size_t size() { return this->stack.size() + buffer.size(); };

            virtual void flushBuffer();

        private:
            virtual int calculateWeight(NonStrictMarkingBase *marking);

            priority_queue buffer;
        };

        template<class T>
        class RandomWaitingList : public WaitingList<T> {
        public:
            typedef std::priority_queue<WeightedItem<T>, std::vector<WeightedItem<T> >, less<T> > priority_queue;
        public:
            RandomWaitingList() : queue() {};

            virtual ~RandomWaitingList();

        public:
            virtual void add(NonStrictMarkingBase *weight, T payload);

            virtual T peek();

            virtual T pop();

            virtual size_t size() { return queue.size(); };
        private:
            virtual int calculateWeight(NonStrictMarkingBase *marking);

            priority_queue queue;
        };


//IMPLEMENTATIONS
        template<class T>
        void StackWaitingList<T>::add(NonStrictMarkingBase *weight, T payload) {
            stack.push(payload);
        }

        template<class T>
        T StackWaitingList<T>::pop() {
            T marking = stack.top();
            stack.pop();
            return marking;
        }

        template<class T>
        T StackWaitingList<T>::peek() {
            T marking = stack.top();
            return marking;
        }

        template<class T>
        StackWaitingList<T>::~StackWaitingList() = default;

        template<class T>
        std::ostream &operator<<(std::ostream &out, WaitingList<T> &x) {
            out << x.size();
            return out;
        }

        template<class T>
        void HeuristicStackWaitingList<T>::add(NonStrictMarkingBase *weight, T payload) {
            WeightedItem<T> weighted_item;
            weighted_item.item = payload;
            weighted_item.weight = calculateWeight(weight);
            buffer.push(weighted_item);
        }

        template<class T>
        void HeuristicStackWaitingList<T>::flushBuffer() {
            while (!buffer.empty()) {
                this->stack.push(buffer.top().item);
                buffer.pop();
            }
        }

        template<class T>
        T HeuristicStackWaitingList<T>::pop() {
            flushBuffer();
            T marking = this->stack.top();
            this->stack.pop();
            return marking;
        }

        template<class T>
        T HeuristicStackWaitingList<T>::peek() {
            flushBuffer();
            T marking = this->stack.top();
            return marking;
        }

        template<class T>
        AST::Query *HeuristicStackWaitingList<T>::normalizeQuery(AST::Query *q) {
            AST::NormalizationVisitor visitor;
            return visitor.normalize(*q);
        }

        template<class T>
        int HeuristicStackWaitingList<T>::calculateWeight(NonStrictMarkingBase *marking) {
            LivenessWeightQueryVisitor weight(*marking);
            IntResult weight_c;
            query->accept(weight, weight_c);
            return weight_c.value;
        }

        template<class T>
        void QueueWaitingList<T>::add(NonStrictMarkingBase *weight, T payload) {
            queue.push(payload);
        }

        template<class T>
        T QueueWaitingList<T>::pop() {
            T marking = queue.front();
            queue.pop();
            return marking;
        }

        template<class T>
        T QueueWaitingList<T>::peek() {
            T marking = queue.front();
            return marking;
        }

        template<class T>
        QueueWaitingList<T>::~QueueWaitingList() = default;

        template<class T>
        void HeuristicWaitingList<T>::add(NonStrictMarkingBase *weight, T payload) {
            WeightedItem<T> weighted_item;
            weighted_item.item = payload;
            weighted_item.weight = calculateWeight(weight);
            queue.push(weighted_item);
        }

        template<class T>
        T HeuristicWaitingList<T>::pop() {
            WeightedItem<T> weighted_item = queue.top();
            T marking = weighted_item.item;
            queue.pop();
            return marking;
        }

        template<class T>
        T HeuristicWaitingList<T>::peek() {
            WeightedItem<T> weighted_item = queue.top();
            T marking = weighted_item.item;
            return marking;
        }

        template<class T>
        AST::Query *HeuristicWaitingList<T>::normalizeQuery(AST::Query *q) {
            AST::NormalizationVisitor visitor;
            return visitor.normalize(*q);
        }

        template<class T>
        int HeuristicWaitingList<T>::calculateWeight(NonStrictMarkingBase *marking) {
            WeightQueryVisitor weight(*marking);
            IntResult weight_c;
            query->accept(weight, weight_c);
            return weight_c.value;
        }

        template<class T>
        HeuristicWaitingList<T>::~HeuristicWaitingList() = default;

        template<class T>
        void MinFirstWaitingList<T>::add(NonStrictMarkingBase *weight, T payload) {
            WeightedItem<T> weighted_item;
            weighted_item.item = payload;
            weighted_item.weight = calculateWeight(payload);
            queue.push(weighted_item);
        }

        template<class T>
        int MinFirstWaitingList<T>::calculateWeight(T tmp) {
            assert(false);
            return 0;
        }

        template<>
        int MinFirstWaitingList<NonStrictMarking *>::calculateWeight(NonStrictMarking *tmp);

        template<>
        int MinFirstWaitingList<ptriepointer_t<MetaData *> >::calculateWeight(ptriepointer_t<MetaData *>);

        template<class T>
        T MinFirstWaitingList<T>::pop() {
            WeightedItem<T> weighted_item = queue.top();
            T marking = weighted_item.item;
            queue.pop();
            return marking;
        }

        template<class T>
        T MinFirstWaitingList<T>::peek() {
            WeightedItem<T> weighted_item = queue.top();
            T marking = weighted_item.item;
            return marking;
        }

        template<class T>
        MinFirstWaitingList<T>::~MinFirstWaitingList() = default;


        template<class T>
        void RandomWaitingList<T>::add(NonStrictMarkingBase *weight, T payload) {
            WeightedItem<T> weighted_item;
            weighted_item.item = payload;
            weighted_item.weight = calculateWeight(weight);
            queue.push(weighted_item);
        }

        template<class T>
        T RandomWaitingList<T>::pop() {
            WeightedItem<T> weighted_item = queue.top();
            T marking = weighted_item.item;
            queue.pop();
            return marking;
        }

        template<class T>
        T RandomWaitingList<T>::peek() {
            WeightedItem<T> weighted_item = queue.top();
            T marking = weighted_item.item;
            return marking;
        }

        template<class T>
        int RandomWaitingList<T>::calculateWeight(NonStrictMarkingBase *marking) {
            return rand();
        }

        template<class T>
        RandomWaitingList<T>::~RandomWaitingList() = default;

        template<class T>
        void RandomStackWaitingList<T>::add(NonStrictMarkingBase *weight, T payload) {
            WeightedItem<T> weighted_item;
            weighted_item.item = payload;
            weighted_item.weight = calculateWeight(weight);
            buffer.push(weighted_item);
        }

        template<class T>
        void RandomStackWaitingList<T>::flushBuffer() {
            while (!buffer.empty()) {
                this->stack.push(buffer.top().item);
                buffer.pop();
            }
        }

        template<class T>
        T RandomStackWaitingList<T>::pop() {
            flushBuffer();
            T marking = this->stack.top();
            this->stack.pop();
            return marking;
        }

        template<class T>
        T RandomStackWaitingList<T>::peek() {
            flushBuffer();
            T marking = this->stack.top();
            return marking;
        }

        template<class T>
        int RandomStackWaitingList<T>::calculateWeight(NonStrictMarkingBase *marking) {
            return rand();
        }

        template<class T>
        RandomStackWaitingList<T>::~RandomStackWaitingList() = default;

    } /* namespace VerifyTAPN */
#endif /* WAITINGLIST_HPP_ */
