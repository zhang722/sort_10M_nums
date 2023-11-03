#include <iostream>
#include <cmath>
#include <iomanip>
#include <fstream>
#include <sstream>

#include "fmt.h"

double frexp10(double arg, int * exp)
{
   *exp = (arg == 0) ? 0 : (int)std::floor(std::log10(std::fabs(arg) ) );
   return arg * std::pow(10 , -(*exp));    
}

std::string double2str(double number, int exp_width) {
    std::stringstream buffer;
    int exponent;

    double mantissa = frexp10(number, &exponent);

    buffer << std::showpos << std::scientific << std::setprecision(9) << number;
    std::string buffer_str = buffer.str();
    if ((exponent < 100 && exponent > 0) || (exponent < 0 && exponent > -100)) {
        buffer_str.erase(buffer_str.length() - 2);

        std::string str = std::to_string(std::abs(exponent));

        if (str.length() < exp_width) {
            str = std::string(exp_width - str.length(), '0') + str;
        }
        buffer_str.append(str);
    }
    return buffer_str;
}