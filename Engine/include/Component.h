#ifndef COMPONENT_H
#define COMPONENT_H
#include <assert.h>

#include "Audio.h"

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
        template <cDerivedComponent T>
        static std::size_t GetAmount()
        {
            if  constexpr (ComponentHasMaxAmount<T>::value)
            {
                static_assert(std::is_same_v<const std::size_t, decltype(T::MAX_AMOUNT)>);
                return T::MAX_AMOUNT;
            }
            else return DEFAULT_MAX_AMOUNT;
        }

        static void SetDefaultMaxAmount(std::size_t maxAmount)
        {
            assert(maxAmount > 0);
            DEFAULT_MAX_AMOUNT = maxAmount;
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
        template<typename T> class ComponentHasMaxAmount {
            template<typename> static std::false_type test(...);
            template<typename U> static decltype(U::MAX_AMOUNT, std::true_type()) test(int);
        public:
            static constexpr bool value = std::is_same_v<decltype(test<T>(0)), std::true_type>;
        };
        static inline std::size_t DEFAULT_MAX_AMOUNT { 20 };

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
