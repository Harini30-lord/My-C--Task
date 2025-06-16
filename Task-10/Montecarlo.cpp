/******************************************************************************

Welcome to GDB Online.
GDB online is an online compiler and debugger tool for C, C++, Python, Java, PHP, Ruby, Perl,
C#, OCaml, VB, Swift, Pascal, Fortran, Haskell, Objective-C, Assembly, HTML, CSS, JS, SQLite, Prolog.
Code, Compile, Run and Debug online from anywhere in world.

*******************************************************************************/

#include <iostream>
#include <vector>
#include <random>
#include <cmath>
#include <iomanip>


                                                                       // num_points: The number of random points to generate
double estimate_pi(long long num_points) {
    if (num_points <= 0) return 0.0;

                                                                          // High-quality random number generation setup
    std::random_device rd;
    std::mt19937 gen(rd());
    std::uniform_real_distribution<> distrib(0.0, 1.0);

    long long points_in_circle = 0;


    for (long long i = 0; i < num_points; ++i) {
        
        double x = distrib(gen);
        double y = distrib(gen);

                                                                           // Calculate the distance from the origin squared
        double distance_squared = x * x + y * y;

                                                                             // Check if the point is inside the unit circle (radius 1)
        if (distance_squared <= 1.0) {
            points_in_circle++;
        }
    }


    return 4.0 * points_in_circle / num_points;
}


int main() {
                                                                                           // Define a list of different sample sizes to test
    std::vector<long long> sample_sizes = {100, 1000, 10000, 100000, 1000000, 10000000};
    
                                                                              // Get the actual value of PI for error calculation
    const double PI_TRUE = M_PI;

    std::cout << "Estimating Pi using the Monte Carlo Method" << std::endl;
    std::cout << std::string(60, '-') << std::endl;
    std::cout << std::left << std::setw(15) << "Sample Size"
              << std::setw(20) << "Estimated Pi"
              << "Error" << std::endl;
    std::cout << std::string(60, '-') << std::endl;

    for (long long n : sample_sizes) {
        double estimate = estimate_pi(n);
        double error = std::abs(estimate - PI_TRUE);

        std::cout << std::left << std::setw(15) << n
                  << std::fixed << std::setprecision(8) << std::setw(20) << estimate
                  << error << std::endl;
    }
    std::cout << std::string(60, '-') << std::endl;
    std::cout << "True value of Pi: " << std::fixed << std::setprecision(8) << PI_TRUE << std::endl;

    return 0;
}
