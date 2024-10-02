#include <iostream>
#include <vector>
#include <set>
#include "qmc.h"
using namespace std;

int main() {
    vector<int> minterm;
    vector<int> dontcare;

    cout << "Input minterm (end with -1): ";
    while (true) {
        int temp;
        cin >> temp;
        if (temp == -1)
            break;
        minterm.push_back(temp);
    }

    cout << "Input don't care (end with -1): ";
    while (true) {
        int temp;
        cin >> temp;
        if (temp == -1)
            break;
        dontcare.push_back(temp);
    }

    qmc_with_petrick_simplify(minterm, dontcare).print();

    return 0;
}