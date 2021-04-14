#include <iostream>
#include <optional>
#include <variant>
#include <string>
#include "pattern_match.hpp"


int main()
{
    auto variant = std::variant<int, std::string, double, float>(10);
    std::string input;
    std::cout << "please input text: ";
    std::cin >> input;
    variant = input;

    auto result = satch::Match{variant}(
        satch::Type<std::string>(), [](auto&& str) 
            {
                std::cout << "variant contains string value: " << str << std::endl;
                return 1; 
            },
        satch::Value<int>(10), [](auto&& value)
            {
                std::cout << "variant contains int value 10" << std::endl;
                return 2;
            },
        satch::Default(),[](auto&& variant) 
            {
                std::cout << "variant.index() = " << variant.index() << std::endl;
                return 3;
            }
        );
    
    std::cout << "matching returned: " << result << std::endl;
    
    return 0;
}