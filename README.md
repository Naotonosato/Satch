# Satch
Satch a C++ Pattern Match Library.
## Usage
```cpp example.cpp
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
                std::cout << "variant contains int value 10" << val << std::endl;
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
```

### output

```shell
$ g++ -std=c++17 example.cpp; ./a.out
please input text: abc
variant contains string value: abc
matching returned: 1
```

## ToDo
+ make error message easier to understand
+ check return type
+ handle errors in passed lambda function