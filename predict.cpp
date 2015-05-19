#include <iostream>
#include <cstdio>
#include <cstdlib>
#include <vector>
#include <utility>
#include <stdexcept>

using namespace std;

// pair of possible outcomes
pair<int, int> predict_next_rand(const vector<int>& values);

int main() {
  vector<int> values;
  for (int i = 0; i < 31; ++i) {
    int value = rand();
    values.push_back(value);
    cout << i << ": " << value << endl;
  }


  for (int i = 0; i < 100; ++i) {
    const auto predictions = predict_next_rand(values);
    int actual = rand();
    cout << "actual: " << actual << " ";
    cout << "got it? " << (predictions.first == actual || predictions.second == actual) << endl;
    values.push_back(actual);
  }
}


pair<int, int> predict_next_rand(const vector<int>& values) {
  // http://www.mathstat.dal.ca/~selinger/random/

  if (values.size() < 31) throw invalid_argument("can only predict with >= 31 outputs");

  int o0 = values[values.size() - 31];
  int o28 = values[values.size() - 3];

  int prediction1 = (o0 + o28) % (1u << 31);
  int prediction2 = (o0 + o28 + 1) % (1u << 31);

  return make_pair(prediction1, prediction2);
}
