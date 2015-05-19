#include <iostream>
#include <cstdio>
#include <cstdlib>
#include <vector>

using namespace std;

int main() {
  vector<int> values;
  for (int i = 0; i < 31; ++i) {
    int value = rand();
    values.push_back(value);
    cout << i << ": " << value << endl;
  }

  cout << "o0: " << values[values.size()-31] << endl;
  cout << "o28: " << values[values.size()- 3] << endl;
  int prediction1 = (values[values.size() - 31] + values[values.size() - 3]);
  int prediction2 = (values[values.size() - 31] + values[values.size() - 3] + 1);
  cout << "next one will be either: " << endl;
  cout << "1) " << prediction1 << endl;
  cout << "2) " << prediction2 << endl;

  cout << "actual: " << rand() << endl;
}

