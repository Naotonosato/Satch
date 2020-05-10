#include <type_traits>
#include <variant>
#include <iostream>
#include <functional>
#include <optional>

template <class... Types> struct TypeList 
{
};

template <typename Types, typename T>
struct append;

template <>
struct append<std::nullopt_t, std::nullopt_t> { using type = std::nullopt_t; };

template <typename T>
struct append<std::nullopt_t, T> { using type = TypeList<T, std::nullopt_t>; };


template <typename Head, typename Tail, typename T>
struct append<TypeList<Head, Tail>, T>
{ using type = TypeList<Head, typename append<Tail, T>::type>; };


template <typename ... Types> struct make;


template <typename T, typename ... Rest>
struct make<T, Rest...> { 
    using type = TypeList<T, typename make<Rest...>::type>;
};


template <>
struct make<> { using type = std::nullopt_t; };

template <typename T> class debug_t;




template<typename T, typename... Args>
struct variant_push;

template<typename T, typename... Args>
struct variant_push<T, std::variant<Args...>>
{
    using type = std::variant<Args..., T>;
};

template<typename Types>
struct typelist_to_variant;

template<typename Head, typename Tail>
struct typelist_to_variant<TypeList<Head, Tail>>
{
    using type = typename variant_push<Head, typename typelist_to_variant<Tail>::type>::type;
};

template<typename Head, typename... Others>
struct remove_first_type
{
    using type = std::variant<std::nullopt_t,Others...>;
};

template<typename Head>
struct typelist_to_variant<TypeList<Head, std::nullopt_t>>
{
    using type = typename remove_first_type<Head>::type;
};


template <typename VariantType>
class Match
{
    private:
    VariantType variant;
    
    
    template<typename FunctionType>
    auto exec(VariantType& variant,FunctionType function)->decltype(function(variant))
    {
        return function(variant);
    }
    

    void operator()(){}
    
    auto get_result_type()
    {
        return make<std::nullopt_t,std::nullopt_t>();
    }

    template<typename First,typename Second>
    auto get_result_type()
    {
        return make<decltype(
            std::declval<Second>() (std::declval<VariantType&>())
            ),
            std::nullopt_t>();
    }

    template<typename First,typename Second,typename Third,typename Fourth,typename... Rest>
    auto get_result_type(Rest... args)
    {
        using list1 = typename make<
        decltype(
            std::declval<Second>() (std::declval<VariantType&>())
            ),
        decltype(
            std::declval<Fourth>() (std::declval<VariantType&>())
            )
            >::type;
        
        using list2 = typename decltype(get_result_type(std::forward<Rest>(args)...))::type;
        return append<list1,list2>();
    }

    private:

    public:
    Match(VariantType& variant):variant(variant){}
    
    template<typename First,typename Second,typename... Rest>
    void operator()(First,Second second,Rest... rest)
    {
        using pattern = typename First::pattern;
        auto function = second;

        using result_type = typename typelist_to_variant<typename decltype(get_result_type<First,Second,Rest...>())::type>::type;


        result_type result(std::nullopt);
        //std::cout << debug_t<decltype(result)>() << std::endl;
        
        if (std::holds_alternative<pattern>(variant))
        {
            result = exec(variant,function);
        }
        
        
        operator()(std::forward<Rest>(rest)...);
        
    }
};

template<typename T>
class Case
{   
    public:
    using pattern = T;
};




int main()
{
    auto v = std::variant<int,std::string>(0);
    
    Match{v}
    (
        Case<std::string>(),[](auto&) {std::cout << "contains string" << std::endl; return 0; },
        Case<int>(),[](auto&) {std::cout << "contains int" << std::endl; return 0.1;}
    );
    

    
    return 0;
}
