#ifndef EXPRPARSER_H
#define EXPRPARSER_H

#include <iostream>
#include <sstream>
#include <string>
#include <vector>
#include <set>
#include "qmc.h"
using namespace std;

string to_form(string single_expr, int var_n = -1, char start = 'A') {
    int arr[100] = {0};  // 0 = no
    char temp = 0;
    for (char c : single_expr) {
        if (c == ' ')
            continue;

        if (c == '\'')
            arr[temp - start] = 2;  // 2 = negative
        else {
            arr[c - start] = 1;  // 1 = positive
            temp = c;
        }
    }
    string res = "";
    for (int i = 0; i < var_n; i++) {
        res += (arr[i] == 0 ? "-" : arr[i] == 1 ? "1"
                                                : "0");
    }
    return res;
}

vector<int> to_minterm(string expr, char start = 'A') {
    // find max
    char max_c = 0;
    for (char c : expr) {
        if (c != ' ' || c != '+' || c != '\'') {
            max_c = max(max_c, c);
        }
    }

    istringstream ss(expr);
    string token;
    set<int> temp_set;
    while (getline(ss, token, '+')) {
        vector<int> v = to_terms(to_form(token, max_c - start + 1));
        for (int i : v) {
            temp_set.insert(i);
        }
    }
    vector<int> minterm;
    for (int i : temp_set) {
        minterm.push_back(i);
    }
    return minterm;
}

#endif