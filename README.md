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
    auto v = std::variant<int,std::string,double,float>("aaa");
    
    auto res = Match{v}
    (
        Case<int>(0),[](auto&) {std::cout << "contains int value 0" << std::endl; return 0;},
        Case<std::string>("aaa"),[](auto&) {return std::string("aaa->bbb"); },
        Case<float>(),[](auto& va) {std::cout << "contains float " << std::get<float>(va) << std::endl; return 0; },
        Case<double>(),[](auto&) {std::cout << "contains double" << std::endl; return 0; }
    );

    std::cout << "result of matching: " << std::get<std::string>(res) << std::endl;
    
    return 0;
}

```

output: bbb

## ToDo
Add "Default" case support.