#ifndef OBSERVINGOBJECT_H
#define OBSERVINGOBJECT_H

#include "Observer.h"

namespace jela
{
    template <typename T>
        concept cExchangable = std::is_move_constructible_v<T> && std::is_copy_constructible_v<T> && std::is_swappable_v<T>;

    template <typename T>
        concept cEasyConstruct = cExchangable<T> && std::is_default_constructible_v<T>;

    template <cExchangable T>
    struct ObjectObserved
    {
        template <typename ...Args>
        ObjectObserved(Args&&... args) :
            object{ args... }
        {}

        ~ObjectObserved() { pOnResourceDestroy.NotifyObservers(); }

        ObjectObserved(const ObjectObserved&) = default;
        ObjectObserved(ObjectObserved&&) noexcept = default;
        ObjectObserved& operator= (const ObjectObserved&) = default;
        ObjectObserved& operator= (ObjectObserved&&) noexcept = default;

        Subject<>* GetSubject() const {return &pOnResourceDestroy;}

        T object;
    private:
        mutable Subject<> pOnResourceDestroy{};
    };


    template <cEasyConstruct ObjectType, typename ...SubjectArgs>
    struct ObservingObject : public Observer<SubjectArgs...>
    {
        ~ObservingObject() override { RemoveFromSubject(); }

        ObservingObject(const ObservingObject& other);
        ObservingObject(ObservingObject&& other) noexcept;
        ObservingObject& operator= (const ObservingObject& other);
        ObservingObject& operator= (ObservingObject&& other) noexcept;

        void swap(ObservingObject& other) noexcept;
        friend void swap(ObservingObject& a, ObservingObject& b) noexcept { a.swap(b); }
    protected:

        // Constructors
        ObservingObject() = default;
        explicit ObservingObject(Subject<SubjectArgs...>* pSubject, const ObjectType& value);
        explicit ObservingObject(Subject<SubjectArgs...>* pSubject, ObjectType&& value);
        template <typename U>
        explicit ObservingObject(const ObjectObserved<U>& observedObject, const ObjectType& value);
        template <typename U>
        explicit ObservingObject(const ObjectObserved<U>& observedObject, ObjectType&& value);


        // Interface implementations
        void Notify(SubjectArgs...) override {};
        void OnSubjectDestroy(Subject<SubjectArgs...>* pSubject) override
        {
            if (pSubject == m_pSubject) ResetSubject();
        }
        void OnSubjectCopied(Subject<SubjectArgs...>* pCopiedSubject, Subject<SubjectArgs...>* pOverwrittenSubject ) override
        {
            if (pOverwrittenSubject == m_pSubject) m_pSubject = pCopiedSubject;
        }
        void OnSubjectMoved(Subject<SubjectArgs...>* pPrevSubject, Subject<SubjectArgs...>* pNewSubject ) override
        {
            if (pPrevSubject == m_pSubject) m_pSubject = pNewSubject;
        }

        // Observing Object Value
        void SetValue(const ObjectType& newValue) { m_ValueObservingTheObject = newValue; }
        void ResetValue() { m_ValueObservingTheObject = ObjectType{}; }
        const ObjectType& GetValue() const {return m_ValueObservingTheObject;}

        // Subject
        void AddToSubject() { if (m_pSubject) m_pSubject->AddObserver(this); }
        void RemoveFromSubject() { if (m_pSubject) m_pSubject->RemoveObserver(this); }
        void ResetSubject() { m_pSubject = nullptr; }
        const Subject<SubjectArgs...>* GetSubject() const {return m_pSubject;}

    private:

        ObjectType m_ValueObservingTheObject{};
        Subject<SubjectArgs...>* m_pSubject{};
    };

    template <typename ResourceType>
    struct ResourcePtr final : public ObservingObject<const ResourceType*>
    {

        using ObservingType = const ResourceType*;

        ResourcePtr() = default;
        explicit ResourcePtr(const ObjectObserved<ResourceType>& observerdObject):
            ObservingObject<ObservingType>{observerdObject, &observerdObject.object}
        {}

        ObservingType get() const { return GetValue(); }
        explicit operator bool() const { return GetValue(); };

        const ResourceType& operator*() const;
        ObservingType operator->() const;

    protected:
        using ObservingObject<ObservingType>::GetValue;
        using ObservingObject<ObservingType>::ResetValue;

        void Notify() override { ResetValue(); }

    };

    struct CompsChangedInfo;

    struct ComponentIndex final : public ObservingObject<std::optional<std::size_t>, const CompsChangedInfo&>
    {
        ComponentIndex() = default;
        ComponentIndex(Subject<const CompsChangedInfo&>* pSubject, std::size_t index):
            ObservingObject{pSubject, index}
        {}

        bool HasValue() const {return GetValue().has_value();}
        std::size_t Get() const { return GetValue().value();}

        void UnbindObserver() { RemoveFromSubject(); ResetSubject(); }
    protected:

        void Notify(const CompsChangedInfo& changeInfo) override;
    };

    //----------------------------------------------------------------------------------------------------------------------
    // ObservingObject
    template <cEasyConstruct ObjectType, typename ...SubjectArgs>
    ObservingObject<ObjectType, SubjectArgs...>::ObservingObject(const ObservingObject& other):
        m_ValueObservingTheObject{other.m_ValueObservingTheObject},
        m_pSubject{other.m_pSubject}
    {
        AddToSubject();
    }
    template <cEasyConstruct ObjectType, typename ...SubjectArgs>
    ObservingObject<ObjectType, SubjectArgs...>::ObservingObject(ObservingObject&& other) noexcept:
        m_ValueObservingTheObject{std::exchange(other.m_ValueObservingTheObject, {})},
        m_pSubject{std::exchange(other.m_pSubject, nullptr)}
    {
        if (m_pSubject) m_pSubject->RemoveObserver(&other);
        AddToSubject();
    }
    template <cEasyConstruct ObjectType, typename ...SubjectArgs>
    ObservingObject<ObjectType, SubjectArgs...>& ObservingObject<ObjectType, SubjectArgs...>::operator=(const ObservingObject& other)
    {
        ObservingObject{ other }.swap(*this);
        return *this;
    }
    template <cEasyConstruct ObjectType, typename ...SubjectArgs>
    ObservingObject<ObjectType, SubjectArgs...>& ObservingObject<ObjectType, SubjectArgs...>::operator=(ObservingObject&& other) noexcept
    {
        ObservingObject{ std::move(other) }.swap(*this);
        return *this;
    }
    template <cEasyConstruct ObjectType, typename ...SubjectArgs>
    void ObservingObject<ObjectType, SubjectArgs...>::swap(ObservingObject& other) noexcept
    {
        other.RemoveFromSubject();
        RemoveFromSubject();

        std::swap(m_ValueObservingTheObject, other.m_ValueObservingTheObject);
        std::swap(m_pSubject, other.m_pSubject);

        other.AddToSubject();
        AddToSubject();
    }
    template <cEasyConstruct ObjectType, typename ... SubjectArgs>
    ObservingObject<ObjectType, SubjectArgs...>::ObservingObject(Subject<SubjectArgs...>* pSubject, const ObjectType& value):
        m_ValueObservingTheObject{value},
        m_pSubject{pSubject}
    {
        AddToSubject();
    }
    template <cEasyConstruct ObjectType, typename ... SubjectArgs>
    ObservingObject<ObjectType, SubjectArgs...>::ObservingObject(Subject<SubjectArgs...>* pSubject, ObjectType&& value):
        m_ValueObservingTheObject{std::move(value)},
        m_pSubject{pSubject}
    {
        AddToSubject();
    }

    template <cEasyConstruct ObjectType, typename ...SubjectArgs>
    template <typename U>
    ObservingObject<ObjectType, SubjectArgs...>::ObservingObject(const ObjectObserved<U>& observedObject, const ObjectType& value):
        ObservingObject{observedObject.GetSubject(), std::forward<ObjectType>(value)}
    {}
    template <cEasyConstruct ObjectType, typename ...SubjectArgs>
    template <typename U>
    ObservingObject<ObjectType, SubjectArgs...>::ObservingObject(const ObjectObserved<U>& observedObject, ObjectType&& value):
        ObservingObject{observedObject.GetSubject(), std::forward<ObjectType>(value)}
    {}

    //----------------------------------------------------------------------------------------------------------------------

    //----------------------------------------------------------------------------------------------------------------------
    // ResourcePtr
    template <typename ResourceType>
    const ResourceType& ResourcePtr<ResourceType>::operator*() const
    {
        if (GetValue() == nullptr) throw std::runtime_error(
            "ObservingObject's value was nullptr when trying to dereference it using the '*' operator!");
        return *GetValue();
    }
    template <typename ResourceType>
    ResourcePtr<ResourceType>::ObservingType ResourcePtr<ResourceType>::operator->() const
    {
        if (GetValue() == nullptr) throw std::runtime_error(
            "ObservingObject's value was nullptr when trying to acces it using the '->' operator!");
        return GetValue();
    }
    //----------------------------------------------------------------------------------------------------------------------
}
#endif //OBSERVINGOBJECT_H
