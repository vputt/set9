#include <iostream>
#include <string>
#include <utility>
#include <vector>

using namespace std;

int charAt(const string& s, int depth) {
    if (depth == (int)s.size()) return -1;
    return (unsigned char)s[depth];
}

void stringQuickSort(vector<string>& a, int left, int right, int depth) {
    if (right - left <= 1) return;

    int pivot = charAt(a[left], depth);
    int lt = left;
    int gt = right;
    int i = left + 1;

    while (i < gt) {
        int current = charAt(a[i], depth);
        if (current < pivot) {
            swap(a[lt++], a[i++]);
        } else if (current > pivot) {
            swap(a[i], a[--gt]);
        } else {
            ++i;
        }
    }

    stringQuickSort(a, left, lt, depth);
    if (pivot != -1) {
        stringQuickSort(a, lt, gt, depth + 1);
    }
    stringQuickSort(a, gt, right, depth);
}

int main() {
    ios::sync_with_stdio(false);
    cin.tie(nullptr);

    int n;
    cin >> n;

    string s;
    getline(cin, s);

    vector<string> a(n);
    for (int i = 0; i < n; ++i) {
        getline(cin, a[i]);
    }

    stringQuickSort(a, 0, n, 0);

    for (const string& item : a) {
        cout << item << '\n';
    }

    return 0;
}
