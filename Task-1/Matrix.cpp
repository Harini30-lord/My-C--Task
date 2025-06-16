/******************************************************************************
https://www.onlinegdb.com/online_c++_compiler#editor_1
                              Online C++ Compiler.
               Code, Compile, Run and Debug C++ program online.
Write your code in this editor and press "Run" button to compile and execute it.

*******************************************************************************/

#include <iostream>
#include <vector>
#include <iomanip> //  formatting o/p

 
class Matrix {
public:
    int rows;
    int cols;
    
    std::vector<std::vector<double>> data;


    Matrix(int r, int c) : rows(r), cols(c) {
        data.resize(rows, std::vector<double>(cols, 0.0));
    }


    void print() {
        for (int i = 0; i < rows; ++i) {
            for (int j = 0; j < cols; ++j) {
                
                std::cout << std::setw(8) << data[i][j] << " ";
            }
            std::cout << std::endl;
        }
        std::cout << std::endl; 
    }
};



// Function to add two matrices
Matrix add(const Matrix& A, const Matrix& B) {

    if (A.rows != B.rows || A.cols != B.cols) {
        std::cerr << "Error: Matrices must have the same dimensions for addition." << std::endl;
        return Matrix(0, 0); 
    }

    Matrix result(A.rows, A.cols);
    for (int i = 0; i < A.rows; ++i) {
        for (int j = 0; j < A.cols; ++j) {
            result.data[i][j] = A.data[i][j] + B.data[i][j];
        }
    }
    return result;
}

// Function to subtract two matrices
Matrix subtract(const Matrix& A, const Matrix& B) {
    
    if (A.rows != B.rows || A.cols != B.cols) {
        std::cerr << "Error: Matrices must have the same dimensions for subtraction." << std::endl;
        return Matrix(0, 0); 
    }

    Matrix result(A.rows, A.cols);
    for (int i = 0; i < A.rows; ++i) {
        for (int j = 0; j < A.cols; ++j) {
            result.data[i][j] = A.data[i][j] - B.data[i][j];
        }
    }
    return result;
}

// Function to multiply two matrices
Matrix multiply(const Matrix& A, const Matrix& B) {
    
    if (A.cols != B.rows) {
        std::cerr << "Error: Incompatible dimensions for multiplication." << std::endl;
        return Matrix(0, 0); 
    }

    Matrix result(A.rows, B.cols);
    for (int i = 0; i < result.rows; ++i) {
        for (int j = 0; j < result.cols; ++j) {
            for (int k = 0; k < A.cols; ++k) {
                result.data[i][j] += A.data[i][k] * B.data[k][j];
            }
        }
    }
    return result;
}


int main() {

    Matrix A(2, 3);
    A.data = {{1, 2, 3}, {4, 5, 6}};


    Matrix B(2, 3);
    B.data = {{10, 11, 12}, {1, 2, 3}};
    

    Matrix C(3, 2);
    C.data = {{7, 8}, {9, 10}, {11, 12}};

    std::cout << "--- Matrix A (2x3) ---" << std::endl;
    A.print();
    std::cout << "--- Matrix B (2x3) ---" << std::endl;
    B.print();
    std::cout << "--- Matrix C (3x2) ---" << std::endl;
    C.print();

  

    // Addition
    std::cout << "--- Addition (A + B) ---" << std::endl;
    Matrix sum = add(A, B);
    sum.print();

    // Subtraction
    std::cout << "--- Subtraction (A - B) ---" << std::endl;
    Matrix diff = subtract(A, B);
    diff.print();

    // Multiplication
    std::cout << "--- Multiplication (A * C) ---" << std::endl;
    Matrix prod = multiply(A, C);
    prod.print();


    std::cout << "--- Invalid Multiplication (A * B) ---" << std::endl;
    Matrix invalid_prod = multiply(A, B); 

    return 0;
}