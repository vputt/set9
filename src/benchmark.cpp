#include "string_sorts.hpp"

#include <algorithm>
#include <chrono>
#include <cstdlib>
#include <fstream>
#include <iomanip>
#include <iostream>
#include <string>
#include <vector>

struct TestCase {
    std::string name;
    std::vector<std::string> data;
};

struct AverageResult {
    std::string case_name;
    std::string algorithm_name;
    double time_us;
    double char_ops;
};

class StringSortTester {
public:
    void run(const std::vector<TestCase>& test_cases) {
        std::ofstream out("data/raw_results.csv");
        out << "case,n,algorithm,repeat,time_us,char_ops\n";

        std::cout << "String sort benchmark\n";
        std::cout << "sizes: 100..3000, step 100, repeats 7\n\n";

        for (const TestCase& test_case : test_cases) {
            std::cout << "Case: " << test_case.name << '\n';
            std::cout << "  n:";
            for (int n = 100; n <= 3000; n += 100) {
                std::vector<std::string> source(test_case.data.begin(), test_case.data.begin() + n);
                std::vector<std::string> correct = source;
                std::sort(correct.begin(), correct.end());

                AverageResult quick = test_algorithm(out, test_case.name, "std_quick_sort", source, correct);
                AverageResult merge = test_algorithm(out, test_case.name, "std_merge_sort", source, correct);
                AverageResult string_quick = test_algorithm(out, test_case.name, "string_quick_sort", source, correct);
                AverageResult string_merge = test_algorithm(out, test_case.name, "string_merge_sort", source, correct);
                AverageResult radix = test_algorithm(out, test_case.name, "msd_radix_sort", source, correct);
                AverageResult radix_quick = test_algorithm(out, test_case.name, "msd_radix_quick_sort", source, correct);

                if (n == 3000) {
                    final_results.push_back(quick);
                    final_results.push_back(merge);
                    final_results.push_back(string_quick);
                    final_results.push_back(string_merge);
                    final_results.push_back(radix);
                    final_results.push_back(radix_quick);
                }

                std::cout << ' ' << n;
            }
            std::cout << "\n\n";
        }

        print_final_table();
    }

private:
    AverageResult test_algorithm(
        std::ofstream& out,
        const std::string& case_name,
        const std::string& algorithm_name,
        const std::vector<std::string>& source,
        const std::vector<std::string>& correct
    ) {
        long long total_time = 0;
        long long total_char_ops = 0;

        for (int repeat = 1; repeat <= 7; ++repeat) {
            std::vector<std::string> data = source;

            auto start = std::chrono::steady_clock::now();
            SortMetrics metrics = run_algorithm(algorithm_name, data);
            auto finish = std::chrono::steady_clock::now();

            if (data != correct) {
                std::cerr << "Wrong answer: " << algorithm_name << " on " << case_name << '\n';
                std::exit(1);
            }

            long long time_us = std::chrono::duration_cast<std::chrono::microseconds>(finish - start).count();
            total_time += time_us;
            total_char_ops += metrics.char_ops;

            out << case_name << ','
                << source.size() << ','
                << algorithm_name << ','
                << repeat << ','
                << time_us << ','
                << metrics.char_ops << '\n';
        }

        return {
            case_name,
            algorithm_name,
            (double)total_time / 7.0,
            (double)total_char_ops / 7.0
        };
    }

    SortMetrics run_algorithm(const std::string& name, std::vector<std::string>& data) {
        if (name == "std_quick_sort") return standard_quick_sort(data);
        if (name == "std_merge_sort") return standard_merge_sort(data);
        if (name == "string_quick_sort") return string_quick_sort(data);
        if (name == "string_merge_sort") return string_merge_sort(data);
        if (name == "msd_radix_sort") return msd_radix_sort(data, false);
        return msd_radix_sort(data, true);
    }

    std::string pretty_name(const std::string& name) {
        if (name == "std_quick_sort") return "QuickSort";
        if (name == "std_merge_sort") return "MergeSort";
        if (name == "string_quick_sort") return "String QuickSort";
        if (name == "string_merge_sort") return "String MergeSort";
        if (name == "msd_radix_sort") return "MSD Radix";
        return "MSD Radix + Quick";
    }

    void print_final_table() {
        std::cout << "Summary for n = 3000\n";
        std::cout << std::left
                  << std::setw(16) << "case"
                  << std::setw(22) << "algorithm"
                  << std::right
                  << std::setw(12) << "time ms"
                  << std::setw(14) << "char ops"
                  << '\n';
        std::cout << std::string(64, '-') << '\n';

        for (const AverageResult& result : final_results) {
            std::cout << std::left
                      << std::setw(16) << result.case_name
                      << std::setw(22) << pretty_name(result.algorithm_name)
                      << std::right
                      << std::setw(12) << std::fixed << std::setprecision(3) << result.time_us / 1000.0
                      << std::setw(14) << (long long)(result.char_ops + 0.5)
                      << '\n';
        }

        std::cout << '\n';
        print_winners();
    }

    void print_winners() {
        std::vector<std::string> cases = {"random", "reversed", "almost_sorted", "common_prefix"};

        std::cout << "Fastest algorithms at n = 3000\n";
        for (const std::string& case_name : cases) {
            const AverageResult* best = nullptr;

            for (const AverageResult& result : final_results) {
                if (result.case_name != case_name) continue;
                if (best == nullptr || result.time_us < best->time_us) {
                    best = &result;
                }
            }

            if (best != nullptr) {
                std::cout << "  " << std::setw(14) << std::left << case_name
                          << " -> " << pretty_name(best->algorithm_name)
                          << " (" << std::fixed << std::setprecision(3)
                          << best->time_us / 1000.0 << " ms)\n";
            }
        }
    }

    std::vector<AverageResult> final_results;
};

int main() {
    StringGenerator generator;

    std::vector<std::string> random_data = generator.random_array(3000);

    std::vector<TestCase> test_cases;
    test_cases.push_back({"random", random_data});
    test_cases.push_back({"reversed", generator.reversed_array(random_data)});
    test_cases.push_back({"almost_sorted", generator.almost_sorted_array(random_data)});
    test_cases.push_back({"common_prefix", generator.common_prefix_array(3000)});

    StringSortTester tester;
    tester.run(test_cases);

    std::cout << "CSV written to data/raw_results.csv\n";
    return 0;
}
