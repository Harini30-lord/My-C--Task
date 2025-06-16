C++ Numerical Integration Comparison:

This is a C++ program that demonstrates and compares two common numerical integration methods: the Trapezoidal Rule and Simpson's Rule.

The program calculates the definite integral (the area under the curve) of a sample function using these two approximation methods. It then compares their accuracy against the exact, true answer calculated analytically using calculus.

///Important Functions:

The program is built around three key functions that represent the different methods for finding the area under a curve.

1. trapezoidal_rule()
What it does: This function approximates the area by dividing the curve into many small, vertical slices and treating each slice as a simple trapezoid.
Accuracy: It's straightforward but generally less accurate because the straight tops of the trapezoids don't perfectly match the curve.

2. simpsons_rule()
What it does: This is a more advanced and accurate method. Instead of using straight lines, it approximates the curve using a series of parabolas (U-shaped curves).
Accuracy: Because a parabola can bend, it "hugs" the original function's curve much more closely, leading to a significantly more accurate result.

3. analytical_solution()
What it does: This function does not use an approximation. It calculates the exact answer using the rules of calculus.
Purpose: It serves as our "golden reference" or ground truth. We use this perfect result to measure the error of the other two methods.
\\\

///
you run the program, it performs the following steps:

<<It defines the problem: Integrate the function f(x) = x³ - 2x² + 5 from x = 0 to x = 10, using 1000 intervals.

<<It calls the trapezoidal_rule() function to get the first estimate.

<<It calls the simpsons_rule() function to get a second, more accurate estimate.

<<It calls the analytical_solution() function to get the true, exact answer.

<<Finally, it calculates the error for the two numerical methods (how far off they were from the exact answer) 

Output:

 This table clearly shows the result from each method and its accuracy.

Integrating f(x) = x^3 - 2x^2 + 5 from 0 to 10 with 1000 intervals.
------------------------------------------------------------
Method                   Result              Error
------------------------------------------------------------
Trapezoidal Rule         1883.33375000       0.00041667
Simpson's Rule           1883.33333333       0.00000000
Analytical Solution      1883.33333333       0.0 (Exact)
------------------------------------------------------------
By looking at the "Error" column, you can immediately see that Simpson's Rule is vastly more accurate than the Trapezoidal Rule for the same amount of work, as its error is effectively zero for this particular problem.

Compile:
 Run the online C++ compiler GDB.


