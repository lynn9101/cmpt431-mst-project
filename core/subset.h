constexpr int UNSET_ELEMENT = -1;

// Define the structure to represent a subset for union-find
class Subset {
   public:
      int numElements;
      std::vector<int> parent;
      std::vector<int> rank;

      Subset(int elements) : numElements(elements), parent(elements, UNSET_ELEMENT), rank(elements, 0) {

      }

};