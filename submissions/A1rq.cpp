#include <iostream>
#include <string>
#include <utility>
#include <vector>

using namespace std;

int charAt(const string& s, int depth) {
    if (depth >= (int)s.size()) return -1;
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

int codeAt(const string& s, int depth) {
    if (depth >= (int)s.size()) return 0;
    return (unsigned char)s[depth] + 1;
}

void msdRadixQuickSort(vector<string>& a, vector<string>& buffer, int left, int right, int depth) {
    const int alphabetSize = 74;
    if (right - left <= 1) return;

    if (right - left < alphabetSize) {
        stringQuickSort(a, left, right, depth);
        return;
    }

    vector<int> count(257, 0);
    vector<int> codes(right - left);

    for (int i = left; i < right; ++i) {
        codes[i - left] = codeAt(a[i], depth);
        ++count[codes[i - left]];
    }

    vector<int> start(257, left);
    for (int code = 1; code < 257; ++code) {
        start[code] = start[code - 1] + count[code - 1];
    }

    vector<int> next = start;
    for (int i = left; i < right; ++i) {
        int code = codes[i - left];
        buffer[next[code]++] = a[i];
    }

    for (int i = left; i < right; ++i) {
        a[i] = buffer[i];
    }

    for (int code = 1; code < 257; ++code) {
        int bucketLeft = start[code];
        int bucketRight = bucketLeft + count[code];
        msdRadixQuickSort(a, buffer, bucketLeft, bucketRight, depth + 1);
    }
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

    vector<string> buffer(n);
    msdRadixQuickSort(a, buffer, 0, n, 0);

    for (const string& item : a) {
        cout << item << '\n';
    }

    return 0;
}
