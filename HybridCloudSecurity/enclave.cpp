#include "enclave.h"
#include <vector>
#include <cmath>

using namespace std;

vector<double> sigmoid(vector<double> input)
{
    vector<double> output;

    for (double x : input)
        output.push_back(1.0 / (1.0 + exp(-x)));

    return output;
}
