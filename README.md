# Satch
Satch C++ Pattern Match Library.
## Usage
```cpp example.cpp
#include <iostream>
#include "pattern_match.hpp"

using namespace satch;

int main()
{
    auto v = std::variant<int,std::string,double,float>("aaa");
    
    auto res = Match{v}
    (
        Case<int>(0),[](auto&) {std::cout << "contains int value 0" << std::endl; return 0.1;},
        Case<std::string>("aaa"),[](auto&) {return std::string("bbb"); },
        Case<float>(),[](auto&) {std::cout << "contains string aaa" << std::endl; return 0; },
        Case<double>(),[](auto&) {std::cout << "contains string aaa" << std::endl; return 4LL; }
    );
    
    std::cout << "result of metching: " << std::get<std::string>(res) << std::endl;
    
    return 0;
}
```

output: bbb

##Isuue
Now,cannnot return value typed same to other in Match().It will improved.