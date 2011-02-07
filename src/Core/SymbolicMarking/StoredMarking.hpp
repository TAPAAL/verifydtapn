namespace VerifyTAPN
{
	enum { DIFFERENT = 0, SUPERSET = 1, SUBSET = 2, EQUAL = 3 } relation_t;

	class StoredMarking
	{
	public:
		virtual ~StoredMarking() { };

		virtual relation_t Relation(const StoredMarking& other) const = 0;
		// virtual Save() // see below
		// virtual Load() // not sure what these should do exactly
	};
}
