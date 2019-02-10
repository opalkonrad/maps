#include <iostream>
#include <string>
#include <chrono>

#include "../include/HashMap.h"
#include "../include/TreeMap.h"


void addToMapTest(int size1, int size2, int size3, int size4, int size5, int size6, int size7, int size8);
void deleteFromMapTest(int size1, int size2, int size3, int size4, int size5, int size6, int size7, int size8);
void valueOfTest(int size1, int size2, int size3, int size4, int size5, int size6, int size7, int size8);


int main() {
  addToMapTest(10, 100, 250, 500, 750, 1000, 2500, 5000);

  std::cout << "\n";

  deleteFromMapTest(10, 100, 250, 500, 750, 1000, 2500, 5000);

  std::cout << "\n";

  valueOfTest(10, 100, 250, 500, 750, 1000, 2500, 5000);

  return 0;
}


void addToMapTest(int size1, int size2, int size3, int size4, int size5, int size6, int size7, int size8) {
  int size;

  for(int i = 0; i < 8; ++i) {
    if(i == 0) {
      size = size1;
    }
    else if(i == 1) {
      size = size2;
    }
    else if(i == 2) {
      size = size3;
    }
    else if(i == 3) {
      size = size4;
    }
    else if(i == 4) {
      size = size5;
    }
    else if(i == 5) {
      size = size6;
    }
    else if(i == 6) {
      size = size7;
    }
    else {
      size = size8;
    }

    std::chrono::nanoseconds::rep elapsedHash = 0;
    std::chrono::nanoseconds::rep elapsedTree = 0;

    for(int j = 0; j < 1000; ++j) {
      aisdi::HashMap<int, std::string> hash;
      aisdi::TreeMap<int, std::string> tree;

      for(int k = 0; k < size - 1; ++k) {
        hash[k] = "test";
        tree[k] = "test";
      }

      auto testStartHash = std::chrono::high_resolution_clock::now();
      hash[size] = "test";
      auto testStopHash = std::chrono::high_resolution_clock::now();

      auto testStartTree = std::chrono::high_resolution_clock::now();
      tree[size] = "test";
      auto testStopTree = std::chrono::high_resolution_clock::now();

      elapsedHash += std::chrono::duration_cast<std::chrono::nanoseconds>(testStopHash - testStartHash).count();
      elapsedTree += std::chrono::duration_cast<std::chrono::nanoseconds>(testStopTree - testStartTree).count();
    }

    std::cout << "Adding to HashMap (" << size << "th. element): " << elapsedHash / 1000 << " nanoseconds\n";
    std::cout << "Adding to TreeMap (" << size << "th. element): " << elapsedTree / 1000 << " nanoseconds\n\n";
  }
}

void deleteFromMapTest(int size1, int size2, int size3, int size4, int size5, int size6, int size7, int size8) {
  int size;

  for(int i = 0; i < 8; ++i) {
    if(i == 0) {
      size = size1;
    }
    else if(i == 1) {
      size = size2;
    }
    else if(i == 2) {
      size = size3;
    }
    else if(i == 3) {
      size = size4;
    }
    else if(i == 4) {
      size = size5;
    }
    else if(i == 5) {
      size = size6;
    }
    else if(i == 6) {
      size = size7;
    }
    else {
      size = size8;
    }

    std::chrono::nanoseconds::rep elapsedHash = 0;
    std::chrono::nanoseconds::rep elapsedTree = 0;

    for(int j = 0; j < 1000; ++j) {
      aisdi::HashMap<int, std::string> hash;
      aisdi::TreeMap<int, std::string> tree;

      for(int k = 0; k < size - 1; ++k) {
        hash[k] = "test";
        tree[k] = "test";
      }

      auto itHash = hash.begin();
      auto itTree = tree.begin();

      auto testStartHash = std::chrono::high_resolution_clock::now();
      hash.remove(itHash);
      auto testStopHash = std::chrono::high_resolution_clock::now();

      auto testStartTree = std::chrono::high_resolution_clock::now();
      tree.remove(itTree);
      auto testStopTree = std::chrono::high_resolution_clock::now();

      elapsedHash += std::chrono::duration_cast<std::chrono::nanoseconds>(testStopHash - testStartHash).count();
      elapsedTree += std::chrono::duration_cast<std::chrono::nanoseconds>(testStopTree - testStartTree).count();
    }

    std::cout << "Deleting from HashMap (" << size << "th. element): " << elapsedHash / 1000 << " nanoseconds\n";
    std::cout << "Deleting from TreeMap (" << size << "th. element): " << elapsedTree / 1000 << " nanoseconds\n\n";
  }
}

void valueOfTest(int size1, int size2, int size3, int size4, int size5, int size6, int size7, int size8) {
  int size;

  for(int i = 0; i < 8; ++i) {
    if(i == 0) {
      size = size1;
    }
    else if(i == 1) {
      size = size2;
    }
    else if(i == 2) {
      size = size3;
    }
    else if(i == 3) {
      size = size4;
    }
    else if(i == 4) {
      size = size5;
    }
    else if(i == 5) {
      size = size6;
    }
    else if(i == 6) {
      size = size7;
    }
    else {
      size = size8;
    }

    std::chrono::nanoseconds::rep elapsedHash = 0;
    std::chrono::nanoseconds::rep elapsedTree = 0;

    for(int j = 0; j < 1000; ++j) {
      aisdi::HashMap<int, std::string> hash;
      aisdi::TreeMap<int, std::string> tree;

      for(int k = 0; k < size - 1; ++k) {
        hash[k] = "test";
        tree[k] = "test";
      }

      int halfSize = size / 2;

      auto testStartHash = std::chrono::high_resolution_clock::now();
      hash.valueOf(halfSize);
      auto testStopHash = std::chrono::high_resolution_clock::now();

      auto testStartTree = std::chrono::high_resolution_clock::now();
      tree.valueOf(halfSize);
      auto testStopTree = std::chrono::high_resolution_clock::now();

      elapsedHash += std::chrono::duration_cast<std::chrono::nanoseconds>(testStopHash - testStartHash).count();
      elapsedTree += std::chrono::duration_cast<std::chrono::nanoseconds>(testStopTree - testStartTree).count();
    }

    std::cout << "Value Of element in HashMap (" << size << "th. element): " << elapsedHash / 1000 << " nanoseconds\n";
    std::cout << "Value Of element in TreeMap (" << size << "th. element): " << elapsedTree / 1000 << " nanoseconds\n\n";
  }
}