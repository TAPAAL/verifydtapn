#ifndef NODE_HPP_
#define NODE_HPP_

#include "../../Core/SymbolicMarking/StoredMarking.hpp"

namespace VerifyTAPN
{
	enum Color { WAITING, PASSED, COVERED };

	class Node{
	public:
		Node(StoredMarking* marking, Color color) : marking(marking), color(color) {};
		virtual ~Node() { delete marking; };
		inline Color GetColor() const { return color; };
		inline StoredMarking* GetMarking() const { return marking; };
	public:
		inline void Recolor(Color newColor) { color = newColor; };
	private:
		StoredMarking* marking;
		Color color;
	};
}

#endif /* NODE_HPP_ */
