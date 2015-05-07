#include <cstdlib>
#include <fstream>
#include <ctime>

using namespace std;

int main() {
  const unsigned int seed = static_cast<unsigned int>(time(NULL));

  srand(seed);

  ofstream file("rand.txt");
  for (int i = 0; i < 1000; ++i) {
    file << rand() << endl;
  }
  return 0;
}
