/******************************************************************************

Welcome to GDB Online.
GDB online is an online compiler and debugger tool for C, C++, Python, Java, PHP, Ruby, Perl,
C#, OCaml, VB, Swift, Pascal, Fortran, Haskell, Objective-C, Assembly, HTML, CSS, JS, SQLite, Prolog.
Code, Compile, Run and Debug online from anywhere in world.

*******************************************************************************/

#include <iostream>
#include <vector>
#include <numeric>      // For std::accumulate
#include <algorithm>    // For std::sort
#include <cmath>        // For std::sqrt
#include <iomanip>      
#include <tuple>        // For std::tuple

std::tuple<double, double, double, double> calculate_statistics(std::vector<double>& data) {
    if (data.empty()) {
        return {0.0, 0.0, 0.0, 0.0};
    }

                                                                                   // 1. Mean
    double sum = std::accumulate(data.begin(), data.end(), 0.0);
    double mean = sum / data.size();

                                                                                  // 2. Median
    
    std::sort(data.begin(), data.end());
    double median;
    size_t size = data.size();
    if (size % 2 == 0) {

        median = (data[size / 2 - 1] + data[size / 2]) / 2.0;
    } else {

        median = data[size / 2];
    }

                                                                                    // 3. Variance
    double variance_sum = 0.0;
    for (double value : data) {
        variance_sum += (value - mean) * (value - mean);
    }
    double variance = variance_sum / data.size(); 

                                                                                    // 4. Standard Deviation
    double std_dev = std::sqrt(variance);

    return {mean, median, variance, std_dev};
}

// --- Validation ---

int main() {

    std::vector<double> my_data = {2, 4, 4, 4, 5, 5, 7, 9};

    std::cout << "Data set: {2, 4, 4, 4, 5, 5, 7, 9}" << std::endl;
    std::cout << std::string(35, '-') << std::endl;

                                                                                                   // Binding to unpack the tuple
    auto [mean, median, variance, std_dev] = calculate_statistics(my_data);
    


    std::cout << std::fixed << std::setprecision(4);
    std::cout << "Mean               : " << mean << std::endl;
    std::cout << "Median             : " << median << std::endl;
    std::cout << "Variance           : " << variance << std::endl;
    std::cout << "Standard Deviation : " << std_dev << std::endl;
    
    return 0;
}

