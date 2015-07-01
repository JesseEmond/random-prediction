#include <iostream>
#include <cstdio>
#include <cstdlib>
#include <vector>
#include <utility>
#include <stdexcept>
#include <string>
#include <cassert>

using namespace std;

using Outputs = vector<int>;
using Predictor = int(const Outputs&);
using RandomGenerator = int();
using ValueModifier = int(int);

// Random Number Generators
//
// These are the generators that we are trying to predict.
int glibc_rand_type1();
int linear_congruential_generator();

// Value modifiers
//
// These are modifiers that are applied to the observed generated
// values. For example, a value modifier could return the 4 least
// significant bits of an integer.
// Basically, the predictor tester does the following:
// generated = modifier(generator());
// ...
// predicted = modifier(predictor());
int identity(int x);
int lower_bits(int x);

// Predictors
//
// These are the predictors that take a look at previous values
// of a specific type of generators and try to predict the next
// value.
int predict_glibc_rand_type1(const Outputs& outputs);
int predict_linear_congruential_generator(const Outputs& outputs);

// Testing functions
//
// These are used to run a predictor against
// a generator and output how well the predictor did.
void test_predictor(const string& testName, Predictor predictor,
    RandomGenerator generator, ValueModifier modifier);

// Mathematical functions
unsigned int egcd(unsigned int a, unsigned int b, int& x, int& y);
unsigned int mod_inv(unsigned int a, unsigned int m);




int main() {
  test_predictor("glibc's TYPE_1 random (default)", predict_glibc_rand_type1,
      glibc_rand_type1, identity);

  // keeping lower bits only will not affect our predictor
  test_predictor("glibc's TYPE_1 random (default) & 0x0F",
      predict_glibc_rand_type1, glibc_rand_type1, lower_bits);

  test_predictor("linear congruential generator",
      predict_linear_congruential_generator, linear_congruential_generator,
      identity);

  return 0;
}


int glibc_rand_type1() {
  return rand();
}

static const unsigned int lcg_mod = (1 << 31);
int linear_congruential_generator() {
  // a and c constants taken from: http://pubs.opengroup.org/onlinepubs/009695399/functions/rand.html
  static unsigned long lcg_next = 1337; // arbitrary seed
  const unsigned int a = 1103515245;
  const unsigned int c = 12345;
  lcg_next = lcg_next * a + c;
  return static_cast<int>(lcg_next % lcg_mod);
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

  const int o31 = outputs[outputs.size() - 31];
  const int o3 = outputs[outputs.size() - 3];

  // random() will return (state[i-31] + state[i-3])>>1,
  // so we don't get to see the least significant bit (LSB) of the states when
  // we get the outputs. We must therefore guess if the unknown LSBs will influence
  // the LSB of the output or not. Here we care about the *second* least significant
  // bit resulting from the addition of the LSBs, as we will shift the result to the
  // right while returning the output.
  // Assuming a uniform ditribution of LSB from previous states, there is only one case out of 4 that will
  // produce a 1 from the addition of bits:
  // 0 + 0 = (0)0
  // 0 + 1 = (0)1
  // 1 + 0 = (0)1
  // 1 + 1 = (1)1 <- 1/4 probability with uniform distribution
  // Therefore, we can expect the LSB from states to only affect the resulting
  // LSB of the output 25% of the time.
  const int prediction1 = (o31 + o3) % (1u << 31); // ~75% more likely (http://stackoverflow.com/a/14679656/395386)
  //const int prediction2 = (o31 + o3) % (1u << 31) + 1; // 25% chance

  return prediction1; // pick the most likely
}

/* Linear congruential generator predictor */
int predict_linear_congruential_generator(const Outputs& outputs) {
  // view http://www.pcg-random.org/predictability.html for more info
  // r[n+1] = a r[n] + c
  //
  // with r[0],r[1],r[2] (the 3 last results):
  // r[1] = a r[0] + c mod m
  // r[2] = a r[1] + c mod m
  //
  // r[1] - r[2] = a r[0] + c - (a r[1] + c) mod m
  //             = a r[0] - a r[1] mod m
  //             = a (r[0] - r[1]) mod m
  // => a = (r[1] - r[2])/(r[0] - r[1]) mod m
  //
  // r[1] = a r[0] + c mod m
  // r[1] = (r[1] - r[2])/(r[0] - r[1]) r[0] + c mod m
  // r[1] - r[0] (r[1] - r[2])/(r[0] - r[1]) = c mod m
  // r[1] - (r[0] r[1] - r[0] r[2]) / (r[0] - r[1]) = c mod m
  // r[1] (r[0] - r[1])/(r[0] - r[1]) - (r[0] r[1] - r[0] r[2])/(r[0] - r[1]) = c mod m
  // (r[0] r[1] - r[1]^2 - r[0] r[1] - r[0] r[2])/(r[0] - r[1]) = c mod m
  // (r[0] r[2] - r[1]^2)/(r[0] - r[1]) = c mod m
  // => c = (r[0] r[2] - r[1]^2)/(r[0] - r[1]) mod m

  if (outputs.size() < 3) throw invalid_argument("only predicting with >= 3 outputs");

  const int r2 = outputs[outputs.size() - 1];
  const int r1 = outputs[outputs.size() - 2];
  const int r0 = outputs[outputs.size() - 3];

  // simply apply the formulas above
  const int denominator = mod_inv(r0 - r1, lcg_mod);
  const int a = ((r1 - r2) * denominator) % lcg_mod;
  const int c = ((r0 * r2 - r1 * r1) * denominator) % lcg_mod;

  return (a * outputs.back() + c) % lcg_mod;
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

unsigned int egcd(unsigned int a, unsigned int b, int& x, int& y) {
  // https://en.wikibooks.org/wiki/Algorithm_Implementation/Mathematics/Extended_Euclidean_algorithm#Modular_inverse
  int c = 0, d = 1, u = 1, v = 0;
  while (a != 0) {
    int q = b / a, r = b % a;
    int m = c - u * q, n = d - v * q;
    b = a;
    a = r;
    c = u;
    d = v;
    u = m;
    v = n;
  }
  x = c;
  y = d;
  return b;
}
unsigned int mod_inv(unsigned int a, unsigned int m) {
  // https://en.wikibooks.org/wiki/Algorithm_Implementation/Mathematics/Extended_Euclidean_algorithm#Modular_inverse
  int x, y;
  const unsigned int gcd = egcd(a, m, x, y);
  assert(gcd == 1);
  return x % m;
}
