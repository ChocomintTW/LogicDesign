#ifndef QMC_H
#define QMC_H

#include <algorithm>
#include <iterator>
#include <set>
#include <string>
#include <vector>
using namespace std;

int count_1(int x) {
    int count = 0;
    while (x) {
        count += (x & 1);
        x >>= 1;
    }
    return count;
}

int count_different(string s1, string s2) {
    if (s1.size() != s2.size())
        return -1;

    int size = s1.size();
    int cnt = 0;
    for (int i = 0; i < size; i++) {
        if (s1[i] != s2[i])
            cnt++;
    }
    return cnt;
}

// convert integer to binary string
string b_str(size_t n, int x) {
    string result;
    while (x > 0) {
        result = (x % 2 == 0 ? "0" : "1") + result;
        x >>= 1;
    }

    while (result.length() < n)
        result = "0" + result;

    return result;
}

// same bits remaining, different bits to '-'
string combine_forms(string form1, string form2) {
    string result = form1;
    for (size_t i = 0; i < form1.length(); ++i) {
        if (form1[i] != form2[i]) {
            result[i] = '-';
        }
    }

    return result;
}

// grouping column to next column
vector<set<string>> grouping(vector<set<string>> last_group) {
    int n = last_group.size();
    vector<set<string>> result_group(n - 1, set<string>());
    for (int i = 0; i < n - 1; i++) {
        set<string> g1 = last_group[i];
        set<string> g2 = last_group[i + 1];
        for (string f1 : g1) {
            for (string f2 : g2) {
                if (count_different(f1, f2) == 1) {
                    result_group[i].insert(combine_forms(f1, f2));
                }
            }
        }
    }
    return result_group;
}

// determine what terms remain (left)
set<string> remains(vector<set<string>> group) {
    // extract all forms
    set<string> forms;
    for (auto s : group) {
        forms.insert(s.begin(), s.end());
    }

    int n = group.size();
    for (int i = 0; i < n - 1; i++) {
        set<string> g1 = group[i];
        set<string> g2 = group[i + 1];
        for (string f1 : g1) {
            for (string f2 : g2) {
                if (count_different(f1, f2) == 1) {
                    forms.erase(f1);
                    forms.erase(f2);
                }
            }
        }
    }

    return forms;
}

// convert form string to terms (m_i)
vector<int> to_terms(string form) {
    std::vector<int> pos;  // where '-' occurs
    for (size_t i = 0; i < form.size(); i++) {
        if (form[i] == '-') {
            pos.push_back(i);
        }
    }
    int n = pos.size();

    vector<int> terms;
    for (int i = 0; i < (1 << n); i++) {
        string str = form;
        for (int j = 0; j < n; j++) {
            if ((i >> j) & 1) {
                str[pos[j]] = '1';
            } else {
                str[pos[j]] = '0';
            }
        }
        terms.push_back(stoi(str, nullptr, 2));
    }

    return terms;
}

string to_expression(string form, char start = 'A') {
    string expr = "";
    for (size_t i = 0; i < form.size(); i++) {
        switch (form[i]) {
            case '0':
                expr += start + i;
                expr += "'";
                break;
            case '1':
                expr += start + i;
                break;
            default:
                break;
        }
    }
    return expr;
}

bool is_empty_column(vector<set<string>> group) {
    for (auto s : group) {
        if (!s.empty())
            return false;
    }
    return true;
}

void forall_possibility(vector<vector<string>> chart, set<int> mi, set<string> temp, set<set<string>>& combinations) {
    if (mi.empty()) {
        combinations.insert(temp);
        return;
    }

    int selected = *mi.begin();
    for (string form : chart[selected]) {
        set<string> s;
        copy(temp.begin(), temp.end(), inserter(s, s.end()));
        s.insert(form);

        set<int> temp_mi;
        copy(mi.begin(), mi.end(), inserter(temp_mi, temp_mi.end()));
        temp_mi.erase(selected);

        forall_possibility(chart, temp_mi, s, combinations);
    }
}

class Result {
   public:
    vector<string> essentials;
    vector<set<string>> optionals;

    Result(vector<string> essentials, vector<set<string>> optionals) : essentials(essentials), optionals(optionals) {}

    void print() {
        cout << "Essential:\n";
        for (string form : this->essentials) {
            cout << to_expression(form) << "\n";
        }

        cout << "\nOptional:\n";
        for (set<string> s : this->optionals) {
            for (size_t i = 0; i < s.size(); i++) {
                cout << to_expression(*next(s.begin(), i));
                if (i != s.size() - 1) {
                    cout << " + ";
                }
            }
            cout << "\n";
        }
    }
};

Result qmc_with_petrick_simplify(vector<int> minterm, vector<int> dontcare = vector<int>()) {
    vector<int> all_terms = minterm;
    all_terms.insert(all_terms.end(), dontcare.begin(), dontcare.end());

    int n = 0;
    int max_t = *max_element(all_terms.begin(), all_terms.end());
    while (max_t > 0) {
        max_t >>= 1;
        n++;
    }

    // first grouping
    vector<set<string>> column1(n + 1, set<string>());
    for (int t : all_terms) {
        int cnt = count_1(t);
        column1[cnt].insert(b_str(n, t));
    }

    // grouping until next column is empty
    auto col = column1;
    vector<vector<set<string>>> column_list;
    set<string> remain_forms;
    while (!is_empty_column(col)) {
        column_list.push_back(col);

        set<string> r = remains(col);
        remain_forms.insert(r.begin(), r.end());

        col = grouping(col);
    }

    // making prime implicant chart
    vector<vector<string>> chart(1 << n, vector<string>());
    for (string f : remain_forms) {
        for (int t : to_terms(f)) {
            if (find(minterm.begin(), minterm.end(), t) != minterm.end())  // is in minterm
                chart[t].push_back(f);
        }
    }

    set<int> mi;  // a duplicated set of minterm vector
    copy(minterm.begin(), minterm.end(), inserter(mi, mi.end()));
    vector<string> essentials;
    for (int r = 0; r < (1 << n); r++) {
        if (chart[r].size() == 1) {
            string form = chart[r][0];
            essentials.push_back(form);
            for (int t : to_terms(form)) {
                chart[t].clear();
                mi.erase(t);
            }
        }
    }

    // Petrick's Method (Programming version - "brute-force")
    set<string> temp;
    set<set<string>> combi;
    forall_possibility(chart, mi, temp, combi);

    vector<set<string>> combinations;
    for (auto s : combi) {
        combinations.push_back(s);
    }
    sort(combinations.begin(), combinations.end(), [](set<string> a, set<string> b) { return a.size() < b.size(); });

    // find minimum expression
    vector<set<string>> min;
    size_t min_size = combinations[0].size();
    for (auto s : combinations) {
        if (s.size() != min_size)
            break;
        min.push_back(s);
    }

    return Result(essentials, min);
}

#endif