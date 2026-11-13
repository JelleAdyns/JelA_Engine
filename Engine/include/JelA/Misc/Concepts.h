#ifndef CONCEPTS_H
#define CONCEPTS_H

#include <type_traits>

namespace jela
{
    template <typename T, typename... Types>
        concept cAnyOf = std::disjunction_v<std::is_same<T, Types>...>;

    template <typename T>
        concept cExchangable = std::is_move_constructible_v<T> && std::is_copy_constructible_v<T> && std::is_swappable_v<T>;

    template <typename T>
        concept cEasyConstruct = cExchangable<T> && std::is_default_constructible_v<T>;

    template <typename T>
        concept cFloatArithmetic =
            std::is_arithmetic_v<T> &&
                std::is_signed_v<T> &&
                    std::numeric_limits<T>::max() <= std::numeric_limits<float>::max() &&
                        std::numeric_limits<T>::min() >= std::numeric_limits<float>::min();

}

#endif //CONCEPTS_H
