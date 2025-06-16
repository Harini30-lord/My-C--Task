/******************************************************************************

Welcome to GDB Online.
GDB online is an online compiler and debugger tool for C, C++, Python, Java, PHP, Ruby, Perl,
C#, OCaml, VB, Swift, Pascal, Fortran, Haskell, Objective-C, Assembly, HTML, CSS, JS, SQLite, Prolog.
Code, Compile, Run and Debug online from anywhere in world.

*******************************************************************************/
#include <iostream>
#include <vector>
#include <cmath>   // For std::abs (absolute value)
#include <iomanip> // For std::setprecision

// ===================================================================
// PART 1: The Polynomial Class
                                                             // Its only job is to store coefficients and evaluate itself and its derivative.

class Polynomial {
public:

                                                             //Example: For 3x^2 + 2x + 5, the vector is {3, 2, 5}
    std::vector<double> coeffs;

                                                              // Constructor to create a Polynomial object from a vector of coefficients
    Polynomial(const std::vector<double>& c) : coeffs(c) {}

                                                                       // Evaluates the polynomial for a given value of x.
                                                                      // For P(x) = 3x^2 + 2x + 5, evaluate(10) returns 3*10^2 + 2*10 + 5 = 325.
    double evaluate(double x) const {
        double result = 0.0;
 
        for (double coeff : coeffs) {
            result = result * x + coeff;
        }
        return result;
    }

                                                                      // Calculates the derivative of this polynomial and returns it as a new Polynomial object.
                                                                      // For P(x) = 3x^2 + 2x + 5, the derivative is P'(x) = 6x + 2.
    Polynomial derivative() const {
                                                                       // If the polynomial is just a constant (e.g., "5"), its derivative is 0.
        if (coeffs.size() <= 1) {
            return Polynomial({0});
        }

        std::vector<double> deriv_coeffs;
        int degree = coeffs.size() - 1;
        for (size_t i = 0; i < coeffs.size() - 1; ++i) {
            // The derivative of ax^n is (a*n)x^(n-1)
            deriv_coeffs.push_back(coeffs[i] * (degree - i));
        }
        return Polynomial(deriv_coeffs);
    }
};

// ===================================================================
// PART 2: The Newton-Raphson Function
// This function is separate from the class. It takes a Polynomial
// and uses it to find a root.
// ===================================================================
// Parameters:
// p: The polynomial we want to find a root of.
// initial_guess: The starting value for x. A good guess is important!
// tolerance: How close to zero we need to be to consider it a root.
// max_iterations: A safety limit to prevent the loop from running forever.
double newton_raphson_find_root(const Polynomial& p, double initial_guess, double tolerance = 1e-7, int max_iterations = 100) {
    
                                                 
                                                // Get the derivative polynomial, which we'll need in every step.
    Polynomial p_derivative = p.derivative();
    
                                                //Start with our initial guess.    
    double x_current = initial_guess;

    std::cout << "Starting Newton-Raphson with guess x0 = " << x_current << std::endl;

    for (int i = 0; i < max_iterations; ++i) {
                                                  // Calculate f(x_n)
        double y = p.evaluate(x_current);
        
                                                      // Calculate f'(x_n)
        double y_derivative = p_derivative.evaluate(x_current);


        if (std::abs(y_derivative) < 1e-12) {
            std::cerr << "Error: Derivative is zero. Newton's method cannot continue." << std::endl;
            return NAN; 
        }

                                                            //the core Newton-Raphson formula: x_new = x_current - f(x)/f'(x)
        double x_next = x_current - y / y_derivative;

                                                                                //  progress of each iteration
        std::cout << "Iteration " << i + 1 << ": x = " << std::fixed << std::setprecision(7) << x_next << std::endl;

                                                                      // Stopping Condition: If the new guess is very close to the old guess,

        if (std::abs(x_next - x_current) < tolerance) {
            std::cout << "Success: Converged to a root." << std::endl;
            return x_next;
        }

                                                                        // Update our current guess for the next loop iteration.
        x_current = x_next;
    }

    std::cerr << "Warning: Failed to converge after " << max_iterations << " iterations." << std::endl;
    return NAN; 
}

// --- Main function ---
int main() {
                                                                           // Let's find a root for the polynomial P(x) = x^2 - 9
                                                                           // The coefficients for x^2 + 0x - 9 are {1, 0, -9}
    Polynomial poly_to_solve({1, 0, -9});

    std::cout << "Finding root for P(x) = x^2 - 9" << std::endl;
    
                                                                            // Call the separate Newton-Raphson function with an initial guess of 5.0
    double root = newton_raphson_find_root(poly_to_solve, 5.0);

    std::cout << "\n----------------------------------------\n";
    if (!std::isnan(root)) {
        std::cout << "Final root found: " << root << std::endl;

        std::cout << "P(" << root << ") = " << poly_to_solve.evaluate(root) << std::endl;
    } else {
        std::cout << "Could not find a root." << std::endl;
    }

    return 0;
}