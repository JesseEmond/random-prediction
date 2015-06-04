#include <iostream>
#include <cstdio>
#include <cstdlib>
#include <vector>
#include <utility>
#include <stdexcept>

using namespace std;

// pair of possible outcomes
typedef pair<int, int> predictions_t;
predictions_t predict_glibc_rand(const vector<int>& values);

template <class Predictor>
void test_predictor(Predictor predictor);

int main() {
  test_predictor(predict_glibc_rand);

  return 0;
}


/* Additive feedback generator predictor
 * Specifically for glibc's TYPE_1 rand (which is the default because by default srand gives a state
 * of 128 bytes to initstate, which gives TYPE_1 random, according to: http://stackoverflow.com/a/25819262/395386)
 */
predictions_t predict_glibc_rand(const vector<int>& values) {
  // info coming from:
  // http://www.mathstat.dal.ca/~selinger/random/
  // for simplicity, only focus on predicting the output after 31 outputs have already been
  // generated (equation #4)
  if (values.size() < 31) throw invalid_argument("can only predict with >= 31 outputs");

  int o0 = values[values.size() - 31];
  int o28 = values[values.size() - 3];

  // random() will return (state[i-31] + state[i-3])>>1,
  // so we don't get to see the least significant bits (LSB) of the states when
  // we get the outputs. We must therefore guess if the unknown LSBs will influence
  // the LSB of the output or not. Here we care about the *second* least significant
  // bit resulting from the addition of the LSBs, as we will shift the result to the
  // right while returning the output.
  // Assuming a uniform ditribution of LSBs from previous states, there is only one case out of 4 that will
  // produce a 1 from the addition of bits:
  // 0 + 0 = (0)0
  // 0 + 1 = (0)1
  // 1 + 0 = (0)1
  // 1 + 1 = (1)1 <- 1/4 probability with uniform distribution
  // Therefore, we can expect the LSBs from states to only affect the resulting
  // LSB of the output 25% of the time.
  int prediction1 = (o0 + o28) % (1u << 31); // ~75% more likely (http://stackoverflow.com/a/14679656/395386)
  int prediction2 = (o0 + o28) % (1u << 31) + 1;

  return make_pair(prediction1, prediction2);
}

template <class Predictor>
void test_predictor(Predictor predictor) {
  const int INITIAL = 31;
  vector<int> values;
  for (int i = 0; i < INITIAL; ++i) {
    int value = rand();
    values.push_back(value);
    cout << i << ": " << value << endl;
  }


  const int TOTAL_TESTS = 10000;
  int first = 0;
  int second = 0;
  for (int i = 0; i < TOTAL_TESTS; ++i) {
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
  if (TOTAL_TESTS != first + second)
    cout << "!! Did not always guess right." << endl;
}
