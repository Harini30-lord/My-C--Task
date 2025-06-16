Monte Carlo Pi Estimation:

"This is a C++ program that estimates the value of the mathematical constant Pi (π) using the Monte Carlo method."

The program demonstrates the power of using randomness to approximate a deterministic value. The core idea is based on the "dartboard" analogy: by randomly dropping a huge number of points over a square area that contains a circle, the ratio of points that land inside the circle can be used to calculate π.

The Code Works:

///
The program's logic is based on simulating "dart throws" onto a 1x1 square that has a quarter-circle of radius 1 inside it.
\\\

--Generate Random Points: 
          The program generates a large number of random points, where each point has an x and a y coordinate between 0.0 and 1.0. This is like a dart landing somewhere inside the 1x1 square.

--Check if Point is in Circle: 
       For each point, it calculates its distance from the origin. If x² + y² is less than or equal to 1, the point is inside the inscribed quarter-circle.

--Count the Hits: 
       The program keeps a count of how many points fall inside the circle.

***
Calculate Pi: 
      The ratio of (points inside the circle) / (total points) approximates the ratio of the areas, which is π/4. Therefore, 
the code calculates the final estimate using the formula:

Pi ≈ 4 * (points inside the circle) / (total points)


**Output:

Estimating Pi using the Monte Carlo Method
------------------------------------------------------------
Sample Size    Estimated Pi        Error
------------------------------------------------------------
100            3.28000000          0.13840735
1000           3.14800000          0.00640735
10000          3.13800000          0.00359265
100000         3.14184000          0.00024735
1000000        3.14170400          0.00011135
10000000       3.14157400          0.00001865
------------------------------------------------------------
True value of Pi: 3.14159265

It shows how close our random estimation is to the actual, true value of Pi. This column is the most important part of the demonstration. You should notice that as the Sample Size gets larger, the Error gets smaller.

///
This is the Law of Large Numbers in action: with more random samples, the result will get closer and closer to the true answer. The output clearly shows that the more "darts" we throw, the more accurate our estimation of π becomes.
\\\


RUN & COMPILER:
 run in the online compiler GDB C++ version.
save into as. cpp file



