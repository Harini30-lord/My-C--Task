/******************************************************************************

Welcome to GDB Online.
GDB online is an online compiler and debugger tool for C, C++, Python, Java, PHP, Ruby, Perl,
C#, OCaml, VB, Swift, Pascal, Fortran, Haskell, Objective-C, Assembly, HTML, CSS, JS, SQLite, Prolog.
Code, Compile, Run and Debug online from anywhere in world.

*******************************************************************************/


#include <iostream>
#include <cmath>
#include <iomanip>

template <typename T>
class Vector3D {
public:
    T x, y, z;

                                                                            //default Constructor
                                                                        
    Vector3D(T x = 0, T y = 0, T z = 0) : x(x), y(y), z(z) {}

                                                                           // Arithmetic operations
    Vector3D<T> operator+(const Vector3D<T>& other) const {
        return Vector3D<T>(x + other.x, y + other.y, z + other.z);
    }

    Vector3D<T> operator-(const Vector3D<T>& other) const {
        return Vector3D<T>(x - other.x, y - other.y, z - other.z);
    }

                                                                                   // Scalar multiplication
    Vector3D<T> operator*(T scalar) const {
        return Vector3D<T>(x * scalar, y * scalar, z * scalar);
    }

                                                                                   // Scalar division
    Vector3D<T> operator/(T scalar) const {
        if (scalar == 0) throw std::runtime_error("Division by zero");
        return Vector3D<T>(x / scalar, y / scalar, z / scalar);
    }

    // --- Vector Operations ---

                                                                                // Magnitude (length) of the vector
    T magnitude() const {
        return static_cast<T>(sqrt(x*x + y*y + z*z));
    }

                                                               // Normalization: returns a unit vector (length 1) in the same direction
    Vector3D<T> normalize() const {
        T mag = magnitude();
        if (mag == 0) return Vector3D<T>(0, 0, 0); 
        return (*this) / mag;
    }
};


                                                                                      // Dot Product of two vectors
template <typename T>
T dot_product(const Vector3D<T>& a, const Vector3D<T>& b) {
    return a.x * b.x + a.y * b.y + a.z * b.z;
}

template <typename T>
Vector3D<T> cross_product(const Vector3D<T>& a, const Vector3D<T>& b) {
    T cross_x = a.y * b.z - a.z * b.y;
    T cross_y = a.z * b.x - a.x * b.z;
    T cross_z = a.x * b.y - a.y * b.x;
    return Vector3D<T>(cross_x, cross_y, cross_z);
}

template <typename T>
std::ostream& operator<<(std::ostream& os, const Vector3D<T>& v) {
    os << "(" << v.x << ", " << v.y << ", " << v.z << ")";
    return os;
}

                                                                              // --- Demonstration and Test Cases ---

int main() {
    std::cout << std::fixed << std::setprecision(2);

                                                                                        // === Test Case 1: Integer Vectors ===
    std::cout << "--- Test Case 1: Integer Vectors ---" << std::endl;
    Vector3D<int> v1(1, 2, 3);
    Vector3D<int> v2(4, 5, 6);

    std::cout << "v1 = " << v1 << std::endl;
    std::cout << "v2 = " << v2 << std::endl;
    std::cout << "v1 + v2 = " << (v1 + v2) << std::endl;
    std::cout << "v1 - v2 = " << (v1 - v2) << std::endl;
    std::cout << "v1 * 3 = " << (v1 * 3) << std::endl;
    std::cout << "Dot Product(v1, v2) = " << dot_product(v1, v2) << std::endl;
    std::cout << "Cross Product(v1, v2) = " << cross_product(v1, v2) << std::endl;
    
 
    std::cout << "Magnitude of v1 = " << v1.magnitude() << std::endl;
    std::cout << std::endl;

                                                                                      // === Test Case 2: Double Precision Vectors ===
    std::cout << "--- Test Case 2: Double Precision Vectors ---" << std::endl;
    Vector3D<double> a(1.0, 0.0, 0.0); // Unit vector i
    Vector3D<double> b(0.0, 1.0, 0.0); // Unit vector j
    Vector3D<double> c(3.0, 4.0, 0.0);

    std::cout << "a = " << a << std::endl;
    std::cout << "b = " << b << std::endl;
    std::cout << "c = " << c << std::endl;

    std::cout << "Dot Product(a, b) = " << dot_product(a, b) << std::endl;
    

    std::cout << "Cross Product(a, b) = " << cross_product(a, b) << std::endl;
    
                                                                                                     // Test magnitude and normalization
    std::cout << "Magnitude of c = " << c.magnitude() << std::endl;
    Vector3D<double> normalized_c = c.normalize();
    std::cout << "Normalized c = " << normalized_c << std::endl;
    std::cout << "Magnitude of normalized c = " << normalized_c.magnitude() << std::endl;

    return 0;
}