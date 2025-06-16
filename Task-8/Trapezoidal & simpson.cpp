/******************************************************************************

Welcome to GDB Online.
GDB online is an online compiler and debugger tool for C, C++, Python, Java, PHP, Ruby, Perl,
C#, OCaml, VB, Swift, Pascal, Fortran, Haskell, Objective-C, Assembly, HTML, CSS, JS, SQLite, Prolog.
Code, Compile, Run and Debug online from anywhere in world.

*******************************************************************************/

#include <iostream>
#include <vector>
#include <cmath>
#include <iomanip>
#include <functional>

                                                                                       // The function: f(x) = x^3 - 2x^2 + 5
double func_to_integrate(double x) {
    return x*x*x - 2*x*x + 5;
}

                                                                                                 // --- 1. Trapezoidal Rule Implementation --- 
double trapezoidal_rule(const std::function<double(double)>& func, double a, double b, int n) {
    double h = (b - a) / n;
    double sum = 0.5 * (func(a) + func(b));

    for (int i = 1; i < n; ++i) {
        sum += func(a + i * h);
    }

    return h * sum;
}

                                                                                                // --- 2. Simpson's 1/3 Rule Implementation ---
double simpsons_rule(const std::function<double(double)>& func, double a, double b, int n) {
    
    if (n % 2 != 0) {
        n++;                                                                                            // n to be even
    }
    
    double h = (b - a) / n;
    double sum = func(a) + func(b);

                                                                                             // Sum of terms with odd indices
    for (int i = 1; i < n; i += 2) {
        sum += 4 * func(a + i * h);
    }
    
                                                                                             // Sum of terms with even indices
    for (int i = 2; i < n - 1; i += 2) {
        sum += 2 * func(a + i * h);
    }

    return (h / 3.0) * sum;
}

                                                                                                  // --- 3. Analytical (Exact) Solution ---
                                                                                               // The definite integral of x^3 - 2x^2 + 5 is (x^4)/4 - (2x^3)/3 + 5x
double analytical_solution(double a, double b) {
    auto integral_func = [](double x) {
        return (pow(x, 4) / 4.0) - (2.0 * pow(x, 3) / 3.0) + (5.0 * x);
    };
    return integral_func(b) - integral_func(a);
}

// --- Main ---
int main() {
    double a = 0.0;    // Lower bound of integration
    double b = 10.0;   // Upper bound of integration
    int n = 1000;      // Number of intervals

    std::cout << "Integrating f(x) = x^3 - 2x^2 + 5 from " << a << " to " << b << " with " << n << " intervals." << std::endl;
    std::cout << std::fixed << std::setprecision(8);
    std::cout << std::string(60, '-') << std::endl;

                                                                                                                      // Calculate results from all three methods
    double trapezoidal_result = trapezoidal_rule(func_to_integrate, a, b, n);
    double simpsons_result = simpsons_rule(func_to_integrate, a, b, n);
    double exact_result = analytical_solution(a, b);

                                                                                                                   // Calculate the error for each numerical method
    double trapezoidal_error = std::abs(trapezoidal_result - exact_result);
    double simpsons_error = std::abs(simpsons_result - exact_result);

                                                                                                                       // Print the comparison table
    std::cout << std::left << std::setw(25) << "Method" << std::setw(20) << "Result" << "Error" << std::endl;
    std::cout << std::string(60, '-') << std::endl;
    std::cout << std::left << std::setw(25) << "Trapezoidal Rule" << std::setw(20) << trapezoidal_result << trapezoidal_error << std::endl;
    std::cout << std::left << std::setw(25) << "Simpson's Rule" << std::setw(20) << simpsons_result << simpsons_error << std::endl;
    std::cout << std::left << std::setw(25) << "Analytical Solution" << std::setw(20) << exact_result << "0.0 (Exact)" << std::endl;
    std::cout << std::string(60, '-') << std::endl;

    return 0;
}