#include <type_traits>
#include <variant>
#include <optional>


namespace satch
{
    template <class... Types> struct TypeList 
    {};


    template<typename Types>
    struct typelist_to_variant;


    template<typename... Types>
    struct typelist_to_variant<TypeList<Types...>>
    {
        using type = std::variant<Types...,std::nullopt_t>;
    };


    template<class... Types>
    struct join
    {
        using type = TypeList<Types...>;
    };

    template<class... Types>
    struct join<TypeList<Types...>>
    {
        using type = TypeList<Types...>;
    };

    template<class... Types1, class... Types2>
    struct join<TypeList<Types1...>, Types2...>
    {
        using type = TypeList<Types1...,Types2...>;
    };

    template<class... Types1, class... Types2>
    struct join< TypeList<Types1...>, TypeList<Types2...> >
    {
        using type = TypeList<Types1...,Types2...>;
    };

    template<typename T>
    class Case
    {   
        private:
        std::optional<T> pattern;
        public:
        Case():pattern(std::nullopt){}
        Case(T value):pattern(value){}
        std::optional<T> const get_pattern() const{return pattern;}
    };


    template <typename VariantType>
    class Match
    {
        private:
        VariantType& variant;



        template<typename List,typename First,typename Second,typename... Rest>
        auto get_result_type()
        {

            using result_type1 = decltype( std::declval<Second>() (std::declval<VariantType&>()) );
            
            if constexpr(std::is_same<result_type1,void>::value)
            {
                using result_type2 = decltype(get_result_type<List,Rest...>());
                return result_type2();
            }
            else
            {
                if constexpr(sizeof...(Rest) == 0)
                {
                    return join<List,result_type1>();
                }

                else
                {
                    using result_type2 = decltype(get_result_type<typename join<List,result_type1>::type,Rest...>());
                    return result_type2();
                }
            }
        }

        private:

        public:
        Match(VariantType& variant):variant(variant){}


        template<typename... Args>
        auto operator()(Args... args) -> typename typelist_to_variant<typename decltype(get_result_type<TypeList<>,Args...>())::type >::type
        {
            using result_type = typename typelist_to_variant<typename decltype(get_result_type<TypeList<>,Args...>())::type >::type;
            result_type result(std::nullopt);
            match<0>(result,args...);
            return result;
        }

        template<int Index,typename ResultType,typename CaseType,typename FunctionType,typename... Rest>
        void match(ResultType& result_ref,CaseType case_obj,FunctionType function,Rest... rest)
        {
            auto pattern = case_obj.get_pattern();
            using match_type = typename decltype(pattern)::value_type;
            constexpr bool is_returns_void = std::is_same<void, decltype(function(variant))>::value;


            if (std::holds_alternative<match_type>(variant))
            {
                if (pattern.has_value())
                {
                    if constexpr(is_returns_void)
                    {
                        return;
                    }
                    else
                    {
                        if (std::get<match_type>(variant) == pattern.value())
                        {result_ref.template emplace<Index>(function(variant));}
                    }
                }
                else
                {
                    if constexpr(is_returns_void)
                    {
                        return;
                    }
                    else
                    {
                        if (std::holds_alternative<match_type>(variant))
                        {result_ref.template emplace<Index>(function(variant));}
                    }
                }
            }

            if (!std::holds_alternative<std::nullopt_t>(result_ref))
            {return;}

            if constexpr(sizeof...(Rest) != 0)
            {
                if constexpr(!is_returns_void) {match<Index+1>(result_ref,std::forward<Rest>(rest)...);}
                else {match<Index>(result_ref,std::forward<Rest>(rest)...);}
            }
            return;
        }   
    };
} // namespace satch
