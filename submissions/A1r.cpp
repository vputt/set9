#include <iostream>
#include <string>
#include <vector>

using namespace std;

int codeAt(const string &s, int depth) {
  if (depth >= (int)s.size())
    return 0;
  return (unsigned char)s[depth] + 1;
}

void msdRadixSort(vector<string> &a, vector<string> &buffer, int left, int right,
                  int depth) {
  if (right - left <= 1)
    return;

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
    msdRadixSort(a, buffer, bucketLeft, bucketRight, depth + 1);
  }
}

int main() {
  ios::sync_with_stdio(false);
  cin.tie(nullptr);

  int n;
  std::cin >> n;

  string s;
  std::getline(std::cin, s);

  vector<string> a(n);
  for (int i = 0; i < n; ++i) {
    std::getline(std::cin, a[i]);
  }

  vector<string> buffer(n);
  msdRadixSort(a, buffer, 0, n, 0);

  for (const string &item : a) {
    std::cout << item << '\n';
  }

  return 0;
}
