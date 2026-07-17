#ifndef COMPONENT_H
#define COMPONENT_H
#include <cassert>
#include <optional>
#include "Defines.h"
#include "FixedSizeAllocators.h"

namespace jela
{
    class Component;

    template <typename DerivedType>
    concept cDerivedComponent =
        std::is_base_of_v<Component, DerivedType> &&
            (!std::is_same_v<DerivedType,Component>);

    class Component
    {
    public:
        static constexpr std::size_t DEFAULT_MAX_AMOUNT { 20 };

        template <cDerivedComponent T>
        static constexpr std::size_t GetMaxAmount()
        {
            if  constexpr (ComponentHasMaxAmount<T>::value)
            {
                static_assert(std::is_same_v<const std::size_t, decltype(T::MAX_AMOUNT)>);
                return T::MAX_AMOUNT;
            }
            else if (std::is_constant_evaluated())
            {
                return DEFAULT_MAX_AMOUNT;
            }
            else
            {
                if (BASE_MAX_AMOUNT.has_value())
                    return BASE_MAX_AMOUNT.value();
                return DEFAULT_MAX_AMOUNT;
            }
        }

        static void SetMaxAmount(std::size_t maxAmount)
        {
            assert(maxAmount > 0);
            BASE_MAX_AMOUNT = maxAmount;
        }

        virtual ~Component()
        {
            OutputDebugString(_T("Component Destructor!\n"));
        }
        Component()
        {
            OutputDebugString(_T("Component Constructor!\n"));
        };
    private:
        template<typename T> class ComponentHasMaxAmount
        {
            // ReSharper disable once CppFunctionIsNotImplemented
            template<typename> static std::false_type test(...);
            // ReSharper disable once CppFunctionIsNotImplemented
            template<typename U> static decltype(U::MAX_AMOUNT, std::true_type()) test(int);

        public:
            static constexpr bool value = std::is_same_v<decltype(test<T>(0)), std::true_type>;
        };
        static inline std::optional<std::size_t> BASE_MAX_AMOUNT {};

    };

    class ComponentAllocator final: public FixedSizeAllocator
    {
    public:
        template <cDerivedComponent T>
        explicit ComponentAllocator(std::type_identity<T> t, MemoryAllocator& alloc):
            FixedSizeAllocator{t, Component::GetMaxAmount<T>(), alloc}
        {}

        template <cDerivedComponent T>
        explicit ComponentAllocator(std::type_identity<T> t):
            FixedSizeAllocator{t, Component::GetMaxAmount<T>()}
        {}
    };

    class derived: public Component
    {
    public:
        ~derived() override
        {
            OutputDebugString(_T("derived Destructor!\n"));
        }
        derived()
        {
            OutputDebugString(_T("derived Constructor!\n"));
        };
        int kaas{};
    };
    class derived2: public Component
    {
    public:
        ~derived2() override
        {
            OutputDebugString(_T("derived2 Destructor!\n"));
        }
        derived2()
        {
            OutputDebugString(_T("derived2 Constructor!\n"));
        };
        static constexpr std::size_t MAX_AMOUNT { 30 };
    };

}
#endif //COMPONENT_H
