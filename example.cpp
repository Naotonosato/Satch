#include <iostream>
#include <optional>
#include <variant>
#include <string>
#include "pattern_match.hpp"
using namespace satch;

int main()
{
    auto variant = std::variant<int, std::string, double, float>("ABC");

    auto result = Match{variant}(
        Case<int>(0), [](auto&&) 
            {
                std::cout << "variant contains int value 0" << std::endl;
                return 0;
            },
        Case<std::string>("ABC"), [](auto&& str) 
            {
                std::cout << str << std::endl;
                return 10; 
            },
        Case<float>(),[](auto&& val) 
            {
                std::cout << "variant contains float " << val << std::endl;
                return 0;
            },
        Case<double>(), [](auto&&) 
            {
                std::cout << "variant contains double" << std::endl;
                return 0;
            }
        );

    std::cout << "result of matching: " << result.value() << std::endl;

    return 0;
}
