constexpr int UNSET_ELEMENT = -1;

// Define the structure to represent a subset for union-find
typedef struct Set {
	int elements;
	int* canonicalElements;
	int* rank;
} Set;