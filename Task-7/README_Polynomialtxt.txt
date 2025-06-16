"C++ Polynomial Root Finder":

This is a C++ program that demonstrates a simple Polynomial class and uses the Newton-Raphson method to find its roots numerically.

///
The project is split into two main parts:

A Polynomial class that can store a polynomial's coefficients, evaluate its value, and calculate its derivative.
A standalone newton_raphson_find_root function that takes a Polynomial object and finds one of its roots.
\\\

function:

The program is designed to find a root for a pre-defined polynomial, P(x) = x² - 9.

Setup: The main function creates a Polynomial object representing P(x) = x² - 9.

Execution: It calls the newton_raphson_find_root function, giving it the polynomial and an initial guess of 5.0.

Iteration: The function then iteratively applies the Newton-Raphson formula (x 
n+1
​
 =x 
n
​
 − 
f 
′
 (x 
n
​
 )
f(x 
n
​
 )
​
 ) to get closer to the root with each step. The progress of each iteration is printed to the console.

Result: Once the guess is accurate enough (converges), the function returns the final root. The main function then prints this root and verifies the result by plugging it back into the original polynomial.


OUTPUT:

When you run the program, it will show the step-by-step process of the Newton-Raphson method converging on the correct root. The final output confirms that the found root is correct because the value of the polynomial at that root is zero.

Finding root for P(x) = x^2 - 9
Starting Newton-Raphson with guess x0 = 5
Iteration 1: x = 3.4000000
Iteration 2: x = 3.0235294
Iteration 3: x = 3.0000916
Iteration 4: x = 3.0000000
Success: Converged to a root.

----------------------------------------
Final root found: 3.0000000
P(3.0000000) = 0.0000000



 Running the code  and  Getting the Output:

1.Execution Starts: When you run ./root_finder, the main() function is executed.

2.Object Creation: A Polynomial object named poly_to_solve is created to represent x² - 9.

3.Function Call: main then calls newton_raphson_find_root.

4.Iterative Process: Inside this function, a for loop begins. In each iteration:
The current guess x is used to evaluate the polynomial and its derivative.

A new, more accurate guess is calculated using the formula x_new = x - f(x)/f'(x).

The line std::cout << "Iteration ..." prints the progress of each new guess. This is how you see the step-by-step convergence in the output.

5.Convergence: The loop continues until the difference between the old guess and the new guess is extremely small (less than the tolerance). At this point, the function prints "Success: Converged to a root." and returns the final value.

6.Final Verification: The execution flow returns to main. The final root (which is 3.0) is printed. The line poly_to_solve.evaluate(root) is called, which calculates 3² - 9 = 0. This final 0 is printed, providing a definitive check that the entire process was successful.

Compiler:
 Run the code in the Online C++ compiler GDB.







