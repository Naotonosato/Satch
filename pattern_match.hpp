#pragma once
#include <type_traits>
#include <variant>
#include <optional>

namespace satch
{
    template <class... Types>
    struct TypeList
    {
    };

    template <typename Types>
    struct typelist_to_variant;

    template <typename... Types>
    struct typelist_to_variant<TypeList<Types...>>
    {
        using type = std::variant<Types..., std::nullopt_t>;
    };

    template <class... Types>
    struct join
    {
        using type = TypeList<Types...>;
    };

    template <class... Types>
    struct join<TypeList<Types...>>
    {
        using type = TypeList<Types...>;
    };

    template <class... Types1, class... Types2>
    struct join<TypeList<Types1...>, Types2...>
    {
        using type = TypeList<Types1..., Types2...>;
    };

    template <class... Types1, class... Types2>
    struct join<TypeList<Types1...>, TypeList<Types2...>>
    {
        using type = TypeList<Types1..., Types2...>;
    };

    template <typename T, typename U, typename = std::void_t<>>
    struct is_comparable : std::false_type
    {
    };

    template <typename T, typename U>
    struct is_comparable<
        T, U, std::void_t<decltype((std::declval<T>() == std::declval<U>()))>>
        : std::true_type
    {
    };

    template <typename T, typename... Types>
    struct remove_first_tag
    {
        using variant_type_removed_first = std::variant<Types...>;
    };

    class _TypeBase{};

    class _Type{};

    template <typename T>
    class Type:public _TypeBase
    {
        public: 
        using match_type = T;
        using __type_id = _Type;
        Type() = default;
    };

    class _Value{};

    template <typename T>
    class Value
    {
        public: 
        using match_type = T;
        using __type_id = _Value;
        private:
        T pattern;

        public:
        Value()=delete;
        Value(T value) : pattern(value){}
        const T& get_pattern() const { return pattern; }
    };

    class Default
    {
        public:
        Default() = default;
    };

    template <typename... Types>
    constexpr size_t get_size_of_variant_tags(const std::variant<Types...>&)
    {
        return sizeof...(Types);
    }

    template <typename Type1, typename... Types>
    constexpr bool has_default_case()
    {
        // end of checking
        if constexpr (std::is_same<Default, Type1>::value){return true;}
        else 
        {
            //continue checking
            constexpr bool result = has_default_case<Types...>();
            return result;
        }
        return false;
    }

    template <typename VariantType>
    class Match
    {
        private:
        VariantType& variant;

        template <typename Pattern, typename Function, typename... Rest>
        auto get_result_type()
        {
            if constexpr (std::is_same<Pattern,Default>::value)
            {
                using result_type = decltype(std::declval<Function>()(std::declval<VariantType>()));
                return result_type();
            }
          
            else
            {
                using match_type = typename Pattern::match_type;
                using result_type = decltype(std::declval<Function>()(std::declval<match_type>()));
                return result_type();
            }
        }

        template <int Index, typename ResultType, typename CaseType,
                typename FunctionType, typename... Rest>
        ResultType match(CaseType& case_obj,
                FunctionType& function, Rest&... rest)
        {
            if constexpr (std::is_same<CaseType,Default>::value)
            {
                static_assert(sizeof...(Rest) == 0,"Default case should passed at the end of arguments.");
                return function(variant);
            }
            else
            {
                using match_type = typename std::remove_reference<CaseType>::type::match_type;

                if (std::holds_alternative<match_type>(variant))
                {
                    //if pattern has value
                    if constexpr(std::is_same<typename CaseType::__type_id,_Value>::value)
                    {              
                        static_assert(is_comparable<match_type, match_type>::value,"cannnot compare pattern and value due to its type");
                                
                        if (std::get<match_type>(variant) == case_obj.get_pattern())
                        {
                            return function(std::get<match_type>(variant));
                        }
                    }
                    else
                    {
                        return function(std::get<match_type>(variant));
                    }
                }
                if constexpr (Index == 0)
                {
                    if constexpr (has_default_case<CaseType,Rest...>())
                    {}//OK.
                    else
                    {
                        constexpr bool not_instantiated = []{return false;}();
                        static_assert(not_instantiated,"Number of patterns must be more than number of variant's tags. And there's no 'Default' case.");
                    }
                }

                if constexpr (sizeof...(Rest) != 0)
                {
                    return match<Index + 1,ResultType>(rest...);
                }
            }
        }

        public:
        Match(VariantType& variant) : variant(variant) {}

        template <typename... Args>
        auto operator()(Args&&... args)
        {
            using result_type = decltype(get_result_type<Args...>());
            return match<0,result_type>( args...);        
        }
    };
} 