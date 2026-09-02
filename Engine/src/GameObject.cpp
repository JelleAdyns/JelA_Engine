#include "GameObject.h"

#include "TransformComponent.h"
#include "Scene.h"
#include "Utils.h"

namespace jela
{
    GameObject& GameObject::Create(Scene& scene)
    {
        return scene.ConsumeGameObject(std::move(GameObject{}));
    }
    void GameObject::SetParent(GameObject& pParent, bool keepWorldPosition)
    {
        SetParent(&pParent, keepWorldPosition);
    }
    void GameObject::SetParent(GameObject* pParent, bool keepWorldPosition)
    {
        if (pParent == m_pParent || pParent == this || IsChild(pParent))
            throw std::invalid_argument("Trying to set an invalid parent!");

        const auto t = m_pTransform;

        if (pParent == nullptr) t->SetLocalPos(t->WorldPosition());
        else if (keepWorldPosition) t->SetLocalPos(t->WorldPosition() - pParent->m_pTransform->WorldPosition());
        else t->SetPosDirty();

        if (m_pParent) std::erase(m_pParent->m_pChildren, this);
        m_pParent = pParent;
        if (m_pParent) m_pParent->m_pChildren.emplace_back(this);
    }
    bool GameObject::IsChild(const GameObject& pGameObject) const
    {
        return IsChild(&pGameObject);
    }
    bool GameObject::IsChild(const GameObject* pGameObject) const
    {
        if (pGameObject == nullptr || pGameObject == m_pParent || pGameObject == this)
            return false;

        return std::ranges::find(m_pChildren, pGameObject) != m_pChildren.end();
    }
    GameObject::GameObject()
    {
        m_pTransform = AddComponent<TransformComponent>();
    }
    GameObject::~GameObject()
    {
        const auto mngr = ENGINE.ComponentMngr();

        mngr->RemoveComponents<std::vector>(m_Components | std::views::values | std::ranges::to<std::vector>());
        m_Components.clear();
    }
    GameObject::GameObject(GameObject&& other) noexcept:
        m_pParent{std::exchange(other.m_pParent, nullptr)},
        m_pChildren{std::exchange(other.m_pChildren, {})},
        m_pTransform{std::exchange(other.m_pTransform, nullptr)},
        m_Components{std::exchange(other.m_Components, {})}
    {
        if (m_pParent)
        {
            if (const auto it = std::ranges::find(m_pParent->m_pChildren, &other);
                it != m_pParent->m_pChildren.cend())
                (*it) = this;

            OutputDebugString(_T("GamObject object had a parent, but wasn't found amongst its children!"));
        }
        std::ranges::for_each(m_Components | std::views::values, [pOwner = this](Component* component)
        {
            component->SetOwner(ComponentOwnerKey{}, pOwner);
        });
    }
}
