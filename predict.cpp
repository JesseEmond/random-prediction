#include <iostream>
#include <cstdio>
#include <cstdlib>
#include <vector>
#include <utility>
#include <stdexcept>
#include <string>

using namespace std;

int predict_glibc_rand(const vector<int>& outputs);

template <class Predictor>
void test_predictor(const string& testName, Predictor predictor);

int main() {
  test_predictor("glibc's TYPE_1 random (default)", predict_glibc_rand);

  return 0;
}


/* Additive feedback generator predictor
 * Predicts with ~75% success
 * Specifically for glibc's TYPE_1 rand (which is the default because by default srand gives a state
 * of 128 bytes to initstate, which gives TYPE_1 random, according to: http://stackoverflow.com/a/25819262/395386)
 */
int predict_glibc_rand(const vector<int>& outputs) {
  // info coming from:
  // http://www.mathstat.dal.ca/~selinger/random/

  // for simplicity, only focus on predicting the output after 31 outputs have already been
  // generated (equation #4)
  if (outputs.size() < 31) throw invalid_argument("only predicting with >= 31 outputs");

  int o31 = outputs[outputs.size() - 31];
  int o3 = outputs[outputs.size() - 3];

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
  int prediction1 = (o31 + o3) % (1u << 31); // ~75% more likely (http://stackoverflow.com/a/14679656/395386)
  //int prediction2 = (o31 + o3) % (1u << 31) + 1; // 25% chance

  return prediction1; // pick the most likely
}

template <class Predictor>
void test_predictor(const string& testName, Predictor predictor) {
  cout << "Predicting output for " << testName << ":" << endl;

  const int INITIAL = 31;
  vector<int> outputs;
  for (int i = 0; i < INITIAL; ++i) {
    int output = rand();
    outputs.push_back(output);
  }


  const int TOTAL_TESTS = 10000;
  int correct_guesses = 0;
  for (int i = 0; i < TOTAL_TESTS; ++i) {
    const auto prediction = predict_glibc_rand(outputs);

    const int actual = rand();
    outputs.push_back(actual);

    const bool was_right = prediction == actual;

    correct_guesses += was_right;
  }

  cout << "Success rate: " << correct_guesses << " / " << TOTAL_TESTS;
  cout << " (" << (static_cast<float>(correct_guesses)/TOTAL_TESTS) << ")" << endl;

  cout << endl;
}
