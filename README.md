# Satch
Satch a C++ Pattern Match Library.
## Usage
```cpp example.cpp
#include <iostream>
#include <optional>
#include <variant>
#include <string>
#include "pattern_match.hpp"
using namespace satch;

int main()
{
    auto variant = std::variant<int, std::string, double, float>(0);
    std::string input;
    std::cout << "please input text: ";
    std::cin >> input;
    variant = input;

    auto result = Match{variant}(
        Case<int>(0), [](auto&&) 
            {
                std::cout << "variant contains int value 0" << std::endl;
                return 0;
            },
        Case<std::string>(), [](auto&& str) 
            {
                std::cout << "variant contains string value: " << str << std::endl;
                return 10; 
            },
        Default(), [](auto&& variant_)
            {
                std::cout << "variant doesn't contains int(0) or string value" << std::endl;
                std::cout << "variant index: " << variant_.index() << std::endl;
                return 0;
            }
        );

    std::cout << "matching returned: " << result.value() << std::endl;

    return 0;
}
```

#### output

```shell
$ g++ -std=c++17 example.cpp; ./a.out
please input text: ABC
variant contains string value: ABC
matching returned: 10
```

## ToDo
+ make error message easier to understand
+ check return type
+ handle errors in passed lambda function