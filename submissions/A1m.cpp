#include <iostream>
#include <string>
#include <utility>
#include <vector>

using namespace std;

pair<int, int> lcpCompare(const string &a, const string &b, int knownLcp = 0) {
  int i = knownLcp;
  while (i < (int)a.size() && i < (int)b.size() && a[i] == b[i]) {
    ++i;
  }

  if (i == (int)a.size() && i == (int)b.size())
    return {0, i};
  if (i == (int)a.size())
    return {-1, i};
  if (i == (int)b.size())
    return {1, i};

  if ((unsigned char)a[i] < (unsigned char)b[i])
    return {-1, i};
  return {1, i};
}

void mergeSort(vector<string> &a, int left, int right) {
  if (right - left <= 1)
    return;

  int mid = left + (right - left) / 2;
  mergeSort(a, left, mid);
  mergeSort(a, mid, right);

  vector<string> merged;
  merged.reserve(right - left);

  int i = left;
  int j = mid;
  while (i < mid && j < right) {
    if (lcpCompare(a[i], a[j]).first <= 0) {
      merged.push_back(a[i++]);
    } else {
      merged.push_back(a[j++]);
    }
  }

  while (i < mid)
    merged.push_back(a[i++]);
  while (j < right)
    merged.push_back(a[j++]);

  for (int k = 0; k < (int)merged.size(); ++k) {
    a[left + k] = merged[k];
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

  mergeSort(a, 0, n);

  for (const string &item : a) {
    cout << item << '\n';
  }

  return 0;
}
