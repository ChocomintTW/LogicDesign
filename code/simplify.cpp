#include <iostream>
#include <string>
#include "exprparser.h"
using namespace std;

int main() {
    cout << "Input expression: ";
    string expr;
    getline(cin, expr);

    qmc_with_petrick_simplify(to_minterm(expr)).print();

    return 0;
}