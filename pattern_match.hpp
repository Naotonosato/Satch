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

    class _Type{};

    template <typename T>
    class Type
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

    class None
    {
        public:
        None() = default;
    };

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
        void match(ResultType& result_ref, CaseType& case_obj,
                FunctionType& function, Rest&... rest)
        {
            if constexpr (std::is_same<CaseType,Default>::value)
            {
                static_assert(sizeof...(Rest) == 0,"Default case should passed at the end of arguments.");
                constexpr bool is_returns_void =
                    std::is_same<void, decltype(function(variant))>::value;
                if constexpr (is_returns_void)
                {
                    function(variant);
                    return;
                }
                else
                {
                    result_ref = function(variant);
                }
            }
            else
            {
                using match_type = typename std::remove_reference<CaseType>::type::match_type;
                constexpr bool is_returns_void =
                    std::is_same<void, decltype(function(std::get<match_type>(variant)))>::value;

                if (std::holds_alternative<match_type>(variant))
                {
                    if constexpr(std::is_same<typename CaseType::__type_id,_Value>::value)
                    {
                        if constexpr (is_returns_void)
                        {
                            constexpr bool compiled_this_block = []{return true;}();
                            static_assert(compiled_this_block && is_comparable<match_type, match_type>::value,"cannnot compare pattern and value due to its type");
                            
                            
                            if (std::get<match_type>(variant) == case_obj.get_pattern())
                            {
                                function(std::get<match_type>(variant));
                                return;
                            }
                        }
                        else
                        {
                            constexpr bool compiled_this_block = []{return true;}();
                            static_assert(compiled_this_block && is_comparable<match_type, match_type>::value,"cannnot compare pattern and value due to its type");
                                        
                            if (std::get<match_type>(variant) == case_obj.get_pattern())
                            {
                                result_ref = function(std::get<match_type>(variant));
                            }
                            
                        }
                    }
                    else
                    {
                        if constexpr (is_returns_void)
                        {
                            function(std::get<match_type>(variant));
                            return;
                        }
                        else
                        {
                            if (std::holds_alternative<match_type>(variant))
                            {
                                result_ref = function(std::get<match_type>(variant));
                            }
                        }
                    }
                }

                if (result_ref.has_value())
                {
                    return;
                }

                if constexpr (sizeof...(Rest) != 0)
                {
                    if constexpr (!is_returns_void)
                    {
                        match<Index + 1>(result_ref, rest...);
                    }
                    else
                    {
                        match<Index>(result_ref, rest...);
                    }
                }
                return;
            }
        }

        public:
        Match(VariantType& variant) : variant(variant) {}

        template <typename... Args>
        auto operator()(Args&&... args)
        {
            if constexpr(std::is_same<decltype(get_result_type<Args...>()),void>::value)
            {
                std::optional<None> result;
                match<0>(result, args...);
                return result;
            }
            else
            {
                using result_type = decltype(get_result_type<Args...>());
                std::optional<result_type> result(std::nullopt);
                match<0>(result, args...);
                return result;
            }
        }
    };


    template <typename VariantType>
    class MatchReturnsMultiple
    {
        private:
        VariantType& variant;

        template <typename List, typename Pattern, typename Function, typename... Rest>
        auto get_result_type()
        {
            if constexpr (std::is_same<Pattern,Default>::value)
            {
                using result_type1 =
                    decltype(std::declval<Function>()(std::declval<VariantType>()));
                if constexpr (std::is_same<result_type1, void>::value)
                {
                    if constexpr (sizeof...(Rest) == 0)
                    {
                        return join<List>();
                    }
                    else
                    {
                        using result_type2 = decltype(get_result_type<List, Rest...>());
                        return result_type2();
                    }
                }
                else
                {
                    if constexpr (sizeof...(Rest) == 0)
                    {
                        return join<List, result_type1>();
                    }

                    else
                    {
                        using result_type2 = decltype(
                            get_result_type<typename join<List, result_type1>::type,
                                            Rest...>());
                        return result_type2();
                    }
                }
            }
            else
            {
                using match_type = typename Pattern::match_type;

                using result_type1 =
                    decltype(std::declval<Function>()(std::declval<match_type>()));

                if constexpr (std::is_same<result_type1, void>::value)
                {
                    if constexpr (sizeof...(Rest) == 0)
                    {
                        return join<List>();
                    }
                    else
                    {
                        using result_type2 = decltype(get_result_type<List, Rest...>());
                        return result_type2();
                    }
                }
                else
                {
                    if constexpr (sizeof...(Rest) == 0)
                    {
                        return join<List, result_type1>();
                    }

                    else
                    {
                        using result_type2 = decltype(
                            get_result_type<typename join<List, result_type1>::type,
                                            Rest...>());
                        return result_type2();
                    }
                }
            }
        }


        template <int Index, typename ResultType, typename CaseType,
                typename FunctionType, typename... Rest>
        void match(ResultType& result_ref, CaseType& case_obj,
                FunctionType& function, Rest&... rest)
        {
            if constexpr (std::is_same<CaseType,Default>::value)
            {
                static_assert(sizeof...(Rest) == 0,"Default case should passed at the end of arguments.");
                constexpr bool is_returns_void =
                    std::is_same<void, decltype(function(variant))>::value;
                if constexpr (is_returns_void)
                {
                    function(variant);
                    return;
                }
                else
                {
                    result_ref.template emplace<Index>(function(variant));
                }
            }
            else
            {
                auto& pattern = case_obj.get_pattern();
                using match_type =
                    typename std::remove_reference<decltype(pattern)>::type::value_type;
                constexpr bool is_returns_void =
                    std::is_same<void, decltype(function(std::get<match_type>(variant)))>::value;

                if (std::holds_alternative<match_type>(variant))
                {
                    if (pattern.has_value())
                    {
                        if constexpr (is_returns_void)
                        {
                            constexpr bool compiled_this_block = []{return true;}();
                            static_assert(compiled_this_block && is_comparable<match_type, match_type>::value,"cannnot compare pattern and value due to its type");
                            
                            if (std::get<match_type>(variant) == pattern.value())
                            {
                                function(std::get<match_type>(variant));
                                return;
                            }
                        }
                        else
                        {
                            constexpr bool compiled_this_block = []{return true;}();
                            static_assert(compiled_this_block && is_comparable<match_type, match_type>::value,"cannnot compare pattern and value due to its type");
                              
                            if (std::get<match_type>(variant) == pattern.value())
                            {
                                result_ref.template emplace<Index>(function(std::get<match_type>(variant)));
                            }
                            
                        }
                    }
                    else
                    {
                        if constexpr (is_returns_void)
                        {
                            function(std::get<match_type>(variant));
                            return;
                        }
                        else
                        {
                            if (std::holds_alternative<match_type>(variant))
                            {
                                result_ref.template emplace<Index>(function(std::get<match_type>(variant)));
                            }
                        }
                    }
                }

                if (!std::holds_alternative<std::nullopt_t>(result_ref))
                {
                    return;
                }

                if constexpr (sizeof...(Rest) != 0)
                {
                    if constexpr (!is_returns_void)
                    {
                        match<Index + 1>(result_ref, rest...);
                    }
                    else
                    {
                        match<Index>(result_ref, rest...);
                    }
                }
                return;
            }
        }

        public:
        MatchReturnsMultiple(VariantType& variant) : variant(variant) {}

        template <typename... Args>
        auto operator()(Args&&... args) -> typename typelist_to_variant<
            typename decltype(get_result_type<TypeList<>, Args...>())::type>::type
        {
            using result_type = typename typelist_to_variant<typename decltype(
                get_result_type<TypeList<>, Args...>())::type>::type;
            result_type result(std::nullopt);
            match<0>(result, args...);
            return result;
        }
    };
} 