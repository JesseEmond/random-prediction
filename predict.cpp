#include <iostream>
#include <cstdio>
#include <cstdlib>
#include <vector>
#include <utility>
#include <stdexcept>
#include <string>

using namespace std;

using Outputs = vector<int>;
using Predictor = int(const Outputs&);
using RandomGenerator = int();
using ValueModifier = int(int);

// Generators
int glibc_rand_type1();

// Value modifiers
int identity(int x);
int lower_bits(int x);

// Predictors
int predict_glibc_rand_type1(const Outputs& outputs);

// Testing functions
void test_predictor(const string& testName, Predictor predictor,
    RandomGenerator generator, ValueModifier modifier);




int main() {
  test_predictor("glibc's TYPE_1 random (default)", predict_glibc_rand_type1,
      glibc_rand_type1, identity);

  // keeping lower bits only will not affect our predictor
  test_predictor("glibc's TYPE_1 random (default) & 0x0F",
      predict_glibc_rand_type1, glibc_rand_type1, lower_bits);

  return 0;
}


int glibc_rand_type1() {
  return rand();
}

int identity(int x) {
  return x;
}
int lower_bits(int x) {
  return x & 0x0F;
}

/* Additive feedback generator predictor
 * Predicts with ~75% success
 * Specifically for glibc's TYPE_1 rand (which is the default because by default srand gives a state
 * of 128 bytes to initstate, which gives TYPE_1 random, according to: http://stackoverflow.com/a/25819262/395386)
 */
int predict_glibc_rand_type1(const vector<int>& outputs) {
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

void test_predictor(const string& testName, Predictor predictor,
    RandomGenerator generator, ValueModifier modifier) {
  cout << "Predicting output for " << testName << ":" << endl;

  const int INITIAL = 31;
  const int TOTAL_TESTS = 10000 + INITIAL;

  vector<int> outputs;
  int correct_guesses = 0;
  int guesses = 0;

  for (int i = 0; i < TOTAL_TESTS; ++i) {
    const int output = modifier(generator());

    if (i >= INITIAL) {
      const int prediction = modifier(predictor(outputs));
      const bool was_right = prediction == output;

      correct_guesses += was_right;
      ++guesses;
    }

    outputs.push_back(output);
  }

  cout << "Success rate: " << correct_guesses << " / " << guesses;
  cout << " (" << (static_cast<float>(correct_guesses)/guesses) << ")" << endl;

  cout << endl;
}
