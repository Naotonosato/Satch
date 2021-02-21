# Satch
Satch a C++ Pattern Match Library.
## Usage
```cpp example.cpp
#include <iostream>
#include <variant>
#include <string>
#include "pattern_match.hpp"
using namespace satch;

int main()
{
    auto v = std::variant<int, std::string, double, float>("aaa");

    auto res = Match{v}(
        Case<int>(0),
        [](auto&&) {
            std::cout << "contains int value 0" << std::endl;
            return 0;
        },
        Case<std::string>("aaa"), [](auto&& str) {std::cout << str << std::endl;return 10; },
        Case<float>(),
        [](auto&& va) {
            std::cout << "contains float " << va << std::endl;
            return 0;
        },
        Case<double>(),
        [](auto&&) {
            std::cout << "contains double" << std::endl;
            return 0;
        });

    std::cout << "result of matching: " << std::get<1>(res)
              << std::endl;

    return 0;
}

```

output: aaa\n result of matching: 10

## ToDo
Add "Default" case support.