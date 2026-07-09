#ifndef COMPONENT_H
#define COMPONENT_H
#include <set>
#include <typeindex>

namespace jela
{
    class Component
    {
    public:
        static size_t GetMaxAmount() {return 20;}
        static size_t GetAmountOfComponentTypes() {return m_AllComponentTypes.size();}
        virtual ~Component() = default;
    protected:
        template <typename DerivedType>
        static std::type_index AddType()
        {
            m_AllComponentTypes.insert(typeid(DerivedType));
            return typeid(DerivedType);
        }
    private:
        static inline std::set<std::type_index> m_AllComponentTypes{};
    };

    class derived: public Component
    {
        static inline auto typeID = AddType<derived>();
    public:
        int kaas{};
    };
    class derived2: public Component
    {
        static inline auto typeID = AddType<derived2>();
        static size_t GetMaxAmount()
        {
            return 30;
        }
    };

    template <typename DerivedType>
    concept cDerivedComponent = std::is_base_of_v<Component, DerivedType> && (!std::is_same_v<DerivedType,Component>) && requires (){ DerivedType::GetMaxAmount(); };
}
#endif //COMPONENT_H
