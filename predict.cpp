#include <iostream>
#include <cstdio>
#include <cstdlib>
#include <vector>
#include <utility>
#include <stdexcept>

using namespace std;

// pair of possible outcomes
pair<int, int> predict_glibc_rand(const vector<int>& values);

int main() {
  vector<int> values;
  for (int i = 0; i < 31; ++i) {
    int value = rand();
    values.push_back(value);
    cout << i << ": " << value << endl;
  }


  const int total = 10000;
  int first = 0;
  int second = 0;
  for (int i = 0; i < total; ++i) {
    const auto predictions = predict_glibc_rand(values);

    const int actual = rand();
    cout << "actual: " << actual << " ";

    const bool wasRight = predictions.first == actual ||
      predictions.second == actual;
    cout << "got it? " << wasRight;

    if (wasRight) {
      const bool firstWasRight = predictions.first == actual;
      cout << " first or second guess: " << (firstWasRight ? 1 : 2);

      if (firstWasRight) ++first;
      else ++second;
    }
    cout << endl;

    values.push_back(actual);
  }

  cout << "first right : " << first  << endl;
  cout << "second right: " << second << endl;
  if (total != first + second)
    cout << "!! Did not always guess right." << endl;
}


/* For glibc's TYPE_1 rand (because by default srand gives a state
 * of 128 bytes, according to: http://stackoverflow.com/a/25819262/395386)
 * predicting an additive feedback generator
 */
pair<int, int> predict_glibc_rand(const vector<int>& values) {
  // http://www.mathstat.dal.ca/~selinger/random/

  if (values.size() < 31) throw invalid_argument("can only predict with >= 31 outputs");

  int o0 = values[values.size() - 31];
  int o28 = values[values.size() - 3];

  int prediction1 = (o0 + o28) % (1u << 31); // ~75% more likely (http://stackoverflow.com/a/14679656/395386)
  int prediction2 = (o0 + o28) % (1u << 31) + 1;

  return make_pair(prediction1, prediction2);
}
