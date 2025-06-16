#include <iostream>
#include <string>
#include <stdexcept> // For std::runtime_error

int parseExpression(const std::string& expr, int& pos);
int parseTerm(const std::string& expr, int& pos);
int parseFactor(const std::string& expr, int& pos);
int parseNumber(const std::string& expr, int& pos);

int parseNumber(const std::string& expr, int& pos) {
    int start_pos = pos;
    while (pos < expr.length() && std::isdigit(expr[pos])) {
        pos++;
    }
    if (pos == start_pos) {
        throw std::runtime_error("Expected a number.");
    }
    return std::stoi(expr.substr(start_pos, pos - start_pos));
}

int parseFactor(const std::string& expr, int& pos) {
    int result;
    if (pos < expr.length() && expr[pos] == '(') {
        pos++;
        result = parseExpression(expr, pos);
        if (pos >= expr.length() || expr[pos] != ')') {
            throw std::runtime_error("Mismatched parentheses: Expected ')'");
        }
        pos++;
    } else {
        result = parseNumber(expr, pos);
    }
    return result;
}

int parseTerm(const std::string& expr, int& pos) {
    int result = parseFactor(expr, pos);
    while (pos < expr.length()) {
        char op = expr[pos];
        if (op == '*' || op == '/') {
            pos++;
            int next_factor = parseFactor(expr, pos);
            if (op == '*') {
                result *= next_factor;
            } else {
                if (next_factor == 0) {
                    throw std::runtime_error("Division by zero!");
                }
                result /= next_factor;
            }
        } else {
            break;
        }
    }
    return result;
}

int parseExpression(const std::string& expr, int& pos) {
    int result = parseTerm(expr, pos);
    while (pos < expr.length()) {
        char op = expr[pos];
        if (op == '+' || op == '-') {
            pos++;
            int next_term = parseTerm(expr, pos);
            if (op == '+') {
                result += next_term;
            } else {
                result -= next_term;
            }
        } else {
            break;
        }
    }
    return result;
}

int evaluateExpression(const std::string& expr) {
    int pos = 0;
    try {
        int result = parseExpression(expr, pos);
        if (pos < expr.length()) {
            throw std::runtime_error("Unexpected characters at end of expression: '" + expr.substr(pos) + "'");
        }
        return result;
    } catch (const std::runtime_error& e) {
        std::cerr << "Error: " << e.what() << std::endl;
        return 0; // Return 0 on error for simplicity
    } catch (const std::out_of_range& e) {
        std::cerr << "Error (Number Conversion): " << e.what() << std::endl;
        return 0;
    }
}

int main() {
    std::cout << "--- Expression Evaluator Basic Tests ---" << std::endl << std::endl;

    // Test Case 1: Basic addition with precedence
    std::string expr1 = "1+2*3";
    std::cout << "Expression: \"" << expr1 << "\"" << std::endl;
    std::cout << "Result: " << evaluateExpression(expr1) << std::endl << std::endl;

    // Test Case 2: Parentheses overriding precedence
    std::string expr2 = "(1+2)*3";
    std::cout << "Expression: \"" << expr2 << "\"" << std::endl;
    std::cout << "Result: " << evaluateExpression(expr2) << std::endl << std::endl;

    // Test Case 3: Division and subtraction
    std::string expr3 = "10-4/2";
    std::cout << "Expression: \"" << expr3 << "\"" << std::endl;
    std::cout << "Result: " << evaluateExpression(expr3) << std::endl << std::endl;

    // Test Case 4: Error handling - Division by zero
    std::string expr4 = "10/0";
    std::cout << "Expression: \"" << expr4 << "\"" << std::endl;
    std::cout << "Result: " << evaluateExpression(expr4) << std::endl << std::endl;
    
    std::cout << "--- Tests Completed ---" << std::endl;

    return 0;
}