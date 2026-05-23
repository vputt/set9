#pragma once

#include <algorithm>
#include <random>
#include <string>
#include <utility>
#include <vector>

struct SortMetrics {
    long long char_ops = 0;
};

inline int char_at(const std::string& s, int depth, SortMetrics& metrics) {
    if (depth >= (int)s.size()) return -1;
    ++metrics.char_ops;
    return (unsigned char)s[depth];
}

inline int code_at(const std::string& s, int depth, SortMetrics& metrics) {
    if (depth >= (int)s.size()) return 0;
    ++metrics.char_ops;
    return (unsigned char)s[depth] + 1;
}

inline int compare_strings(const std::string& a, const std::string& b, SortMetrics& metrics) {
    int limit = std::min((int)a.size(), (int)b.size());
    for (int i = 0; i < limit; ++i) {
        ++metrics.char_ops;
        if ((unsigned char)a[i] < (unsigned char)b[i]) return -1;
        if ((unsigned char)a[i] > (unsigned char)b[i]) return 1;
    }

    if (a.size() < b.size()) return -1;
    if (a.size() > b.size()) return 1;
    return 0;
}

inline std::pair<int, int> lcp_compare(
    const std::string& a,
    const std::string& b,
    int known_lcp,
    SortMetrics& metrics
) {
    int i = known_lcp;
    while (i < (int)a.size() && i < (int)b.size()) {
        ++metrics.char_ops;
        if (a[i] != b[i]) break;
        ++i;
    }

    if (i == (int)a.size() && i == (int)b.size()) return {0, i};
    if (i == (int)a.size()) return {-1, i};
    if (i == (int)b.size()) return {1, i};

    if ((unsigned char)a[i] < (unsigned char)b[i]) return {-1, i};
    return {1, i};
}

inline void quick_sort_impl(std::vector<std::string>& a, int left, int right, SortMetrics& metrics) {
    if (right - left <= 1) return;

    std::string pivot = a[left];
    int i = left;
    int j = right - 1;

    while (i <= j) {
        while (compare_strings(a[i], pivot, metrics) < 0) ++i;
        while (compare_strings(a[j], pivot, metrics) > 0) --j;
        if (i <= j) {
            std::swap(a[i], a[j]);
            ++i;
            --j;
        }
    }

    if (left < j) quick_sort_impl(a, left, j + 1, metrics);
    if (i < right) quick_sort_impl(a, i, right, metrics);
}

inline SortMetrics standard_quick_sort(std::vector<std::string>& a) {
    SortMetrics metrics;
    quick_sort_impl(a, 0, (int)a.size(), metrics);
    return metrics;
}

inline void merge_sort_impl(std::vector<std::string>& a, int left, int right, SortMetrics& metrics) {
    if (right - left <= 1) return;

    int mid = left + (right - left) / 2;
    merge_sort_impl(a, left, mid, metrics);
    merge_sort_impl(a, mid, right, metrics);

    std::vector<std::string> merged;
    int i = left;
    int j = mid;

    while (i < mid && j < right) {
        if (compare_strings(a[i], a[j], metrics) <= 0) {
            merged.push_back(a[i++]);
        } else {
            merged.push_back(a[j++]);
        }
    }

    while (i < mid) merged.push_back(a[i++]);
    while (j < right) merged.push_back(a[j++]);

    for (int k = 0; k < (int)merged.size(); ++k) {
        a[left + k] = merged[k];
    }
}

inline SortMetrics standard_merge_sort(std::vector<std::string>& a) {
    SortMetrics metrics;
    merge_sort_impl(a, 0, (int)a.size(), metrics);
    return metrics;
}

inline void string_quick_sort_impl(
    std::vector<std::string>& a,
    int left,
    int right,
    int depth,
    SortMetrics& metrics
) {
    if (right - left <= 1) return;

    int pivot = char_at(a[left], depth, metrics);
    int lt = left;
    int gt = right;
    int i = left + 1;

    while (i < gt) {
        int current = char_at(a[i], depth, metrics);
        if (current < pivot) {
            std::swap(a[lt++], a[i++]);
        } else if (current > pivot) {
            std::swap(a[i], a[--gt]);
        } else {
            ++i;
        }
    }

    string_quick_sort_impl(a, left, lt, depth, metrics);
    if (pivot != -1) {
        string_quick_sort_impl(a, lt, gt, depth + 1, metrics);
    }
    string_quick_sort_impl(a, gt, right, depth, metrics);
}

inline SortMetrics string_quick_sort(std::vector<std::string>& a) {
    SortMetrics metrics;
    string_quick_sort_impl(a, 0, (int)a.size(), 0, metrics);
    return metrics;
}

inline void string_merge_sort_impl(std::vector<std::string>& a, int left, int right, SortMetrics& metrics) {
    if (right - left <= 1) return;

    int mid = left + (right - left) / 2;
    string_merge_sort_impl(a, left, mid, metrics);
    string_merge_sort_impl(a, mid, right, metrics);

    std::vector<std::string> merged;
    int i = left;
    int j = mid;

    while (i < mid && j < right) {
        if (lcp_compare(a[i], a[j], 0, metrics).first <= 0) {
            merged.push_back(a[i++]);
        } else {
            merged.push_back(a[j++]);
        }
    }

    while (i < mid) merged.push_back(a[i++]);
    while (j < right) merged.push_back(a[j++]);

    for (int k = 0; k < (int)merged.size(); ++k) {
        a[left + k] = merged[k];
    }
}

inline SortMetrics string_merge_sort(std::vector<std::string>& a) {
    SortMetrics metrics;
    string_merge_sort_impl(a, 0, (int)a.size(), metrics);
    return metrics;
}

inline void msd_radix_sort_impl(
    std::vector<std::string>& a,
    std::vector<std::string>& buffer,
    int left,
    int right,
    int depth,
    bool use_quick_sort,
    SortMetrics& metrics
) {
    const int alphabet_size = 74;
    if (right - left <= 1) return;

    if (use_quick_sort && right - left < alphabet_size) {
        string_quick_sort_impl(a, left, right, depth, metrics);
        return;
    }

    std::vector<int> count(257, 0);
    std::vector<int> codes(right - left);

    for (int i = left; i < right; ++i) {
        codes[i - left] = code_at(a[i], depth, metrics);
        ++count[codes[i - left]];
    }

    std::vector<int> start(257, left);
    for (int code = 1; code < 257; ++code) {
        start[code] = start[code - 1] + count[code - 1];
    }

    std::vector<int> next = start;
    for (int i = left; i < right; ++i) {
        int code = codes[i - left];
        buffer[next[code]++] = a[i];
    }

    for (int i = left; i < right; ++i) {
        a[i] = buffer[i];
    }

    for (int code = 1; code < 257; ++code) {
        int bucket_left = start[code];
        int bucket_right = bucket_left + count[code];
        msd_radix_sort_impl(a, buffer, bucket_left, bucket_right, depth + 1, use_quick_sort, metrics);
    }
}

inline SortMetrics msd_radix_sort(std::vector<std::string>& a, bool use_quick_sort) {
    SortMetrics metrics;
    std::vector<std::string> buffer(a.size());
    msd_radix_sort_impl(a, buffer, 0, (int)a.size(), 0, use_quick_sort, metrics);
    return metrics;
}

class StringGenerator {
public:
    StringGenerator() : rng(42) {}

    std::vector<std::string> random_array(int n) {
        std::vector<std::string> a;
        for (int i = 0; i < n; ++i) {
            a.push_back(random_string(10, 200));
        }
        return a;
    }

    std::vector<std::string> reversed_array(std::vector<std::string> a) {
        std::sort(a.begin(), a.end());
        std::reverse(a.begin(), a.end());
        return a;
    }

    std::vector<std::string> almost_sorted_array(std::vector<std::string> a) {
        std::sort(a.begin(), a.end());
        if (a.size() < 2) return a;

        int swaps = std::max(1, (int)a.size() / 30);
        std::uniform_int_distribution<int> index_dist(0, (int)a.size() - 1);

        for (int i = 0; i < swaps; ++i) {
            int first = index_dist(rng);
            int second = index_dist(rng);
            std::swap(a[first], a[second]);
        }

        return a;
    }

    std::vector<std::string> common_prefix_array(int n) {
        std::string prefix = random_string(80, 80);
        std::vector<std::string> a;

        for (int i = 0; i < n; ++i) {
            a.push_back(prefix + random_string(10, 120));
        }

        return a;
    }

private:
    std::string random_string(int min_length, int max_length) {
        const std::string alphabet =
            "ABCDEFGHIJKLMNOPQRSTUVWXYZ"
            "abcdefghijklmnopqrstuvwxyz"
            "0123456789"
            "!@#%:;^&*()-";

        std::uniform_int_distribution<int> length_dist(min_length, max_length);
        std::uniform_int_distribution<int> char_dist(0, (int)alphabet.size() - 1);

        int length = length_dist(rng);
        std::string s;
        for (int i = 0; i < length; ++i) {
            s.push_back(alphabet[char_dist(rng)]);
        }

        return s;
    }

    std::mt19937 rng;
};
