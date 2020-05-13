#include <iostream>
#include <variant>
#include <string>
#include "pattern_match.hpp"

using namespace satch;

template <typename T>
struct debug_t;

int main()
{
    auto v = std::variant<int,std::string,double,float>("aaa");
    
    auto res = Match{v}
    (
        Case<int>(0),[](auto&) {std::cout << "contains int value 0" << std::endl; return 0;},
        Case<std::string>("aaa"),[](auto&) {return; },
        Case<float>(),[](auto&) {std::cout << "contains string aaa" << std::endl; return 0; },
        Case<double>(),[](auto&) {std::cout << "contains string aaa" << std::endl; return 0; }
    );
    
    //debug_t<decltype(res)> d;

    std::cout << "result of metching: " << res.index() << std::endl;
    
    return 0;
}
