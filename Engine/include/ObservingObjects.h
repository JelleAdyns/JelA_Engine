#ifndef OBSERVINGOBJECT_H
#define OBSERVINGOBJECT_H

#include "Observer.h"

namespace jela
{
    template <typename T>
    struct ObjectObserved
    {
        template <typename ...Args>
        ObjectObserved(Args&&... args) :
            object{ args... },
            pOnResourceDestroy{std::make_unique<Subject<>>()}
        {}

        ~ObjectObserved() { pOnResourceDestroy->NotifyObservers(); }

        ObjectObserved(const ObjectObserved&) = delete;
        ObjectObserved(ObjectObserved&&) noexcept = delete;
        ObjectObserved& operator= (const ObjectObserved&) = delete;
        ObjectObserved& operator= (ObjectObserved&&) noexcept = delete;

        Subject<>* GetSubject() const {return pOnResourceDestroy.get();}

        T object;
    private:
        std::unique_ptr<Subject<>> pOnResourceDestroy{};
    };

    template <typename T>
        concept cInterchangeable = std::is_move_constructible_v<T> && std::is_copy_constructible_v<T> && std::is_swappable_v<T>;

    template <cInterchangeable ObjectType, typename ...SubjectArgs>
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

        ObservingObject() = default;
        explicit ObservingObject(Subject<SubjectArgs...>* pSubject, const ObjectType& value);
        explicit ObservingObject(Subject<SubjectArgs...>* pSubject, ObjectType&& value);
        template <typename U>
        explicit ObservingObject(const ObjectObserved<U>& observedObject, const ObjectType& value);
        template <typename U>
        explicit ObservingObject(const ObjectObserved<U>& observedObject, ObjectType&& value);


        void Notify(SubjectArgs...) override {};
        void OnSubjectDestroy(Subject<SubjectArgs...>* pSubject) override { if (pSubject == m_pSubject) m_pSubject = nullptr; }

        void SetValue(const ObjectType& newValue) { m_ValueObservingTheObject = newValue; }
        void ResetValue() { m_ValueObservingTheObject = ObjectType{}; }
        const ObjectType& GetValue() const {return m_ValueObservingTheObject;}

        void SaveSubject(Subject<SubjectArgs...>* pSubject);

        void AddToSubject() { if (m_pSubject) m_pSubject->AddObserver(this); }
        void RemoveFromSubject() { if (m_pSubject) m_pSubject->RemoveObserver(this); }

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

    //----------------------------------------------------------------------------------------------------------------------
    // ObservingObject
    template <cInterchangeable ObjectType, typename ...SubjectArgs>
    ObservingObject<ObjectType, SubjectArgs...>::ObservingObject(const ObservingObject& other):
        m_ValueObservingTheObject{other.m_ValueObservingTheObject},
        m_pSubject{other.m_pSubject}
    {
        AddToSubject();
    }
    template <cInterchangeable ObjectType, typename ...SubjectArgs>
    ObservingObject<ObjectType, SubjectArgs...>::ObservingObject(ObservingObject&& other) noexcept:
        m_ValueObservingTheObject{std::exchange(other.m_ValueObservingTheObject, {})},
        m_pSubject{std::exchange(other.m_pSubject, nullptr)}
    {
        if (m_pSubject) m_pSubject->RemoveObserver(&other);
        AddToSubject();
    }
    template <cInterchangeable ObjectType, typename ...SubjectArgs>
    ObservingObject<ObjectType, SubjectArgs...>& ObservingObject<ObjectType, SubjectArgs...>::operator=(const ObservingObject& other)
    {
        ObservingObject{ other }.swap(*this);
        return *this;
    }
    template <cInterchangeable ObjectType, typename ...SubjectArgs>
    ObservingObject<ObjectType, SubjectArgs...>& ObservingObject<ObjectType, SubjectArgs...>::operator=(ObservingObject&& other) noexcept
    {
        ObservingObject{ std::move(other) }.swap(*this);
        return *this;
    }
    template <cInterchangeable ObjectType, typename ...SubjectArgs>
    void ObservingObject<ObjectType, SubjectArgs...>::swap(ObservingObject& other) noexcept
    {
        other.RemoveFromSubject();
        RemoveFromSubject();

        std::swap(m_ValueObservingTheObject, other.m_ValueObservingTheObject);
        std::swap(m_pSubject, other.m_pSubject);

        other.AddToSubject();
        AddToSubject();
    }
    template <cInterchangeable ObjectType, typename ... SubjectArgs>
    ObservingObject<ObjectType, SubjectArgs...>::ObservingObject(Subject<SubjectArgs...>* pSubject, const ObjectType& value):
        m_ValueObservingTheObject{value},
        m_pSubject{pSubject}
    {
        AddToSubject();
    }
    template <cInterchangeable ObjectType, typename ... SubjectArgs>
    ObservingObject<ObjectType, SubjectArgs...>::ObservingObject(Subject<SubjectArgs...>* pSubject, ObjectType&& value):
        m_ValueObservingTheObject{std::move(value)},
        m_pSubject{pSubject}
    {
        AddToSubject();
    }

    template <cInterchangeable ObjectType, typename ...SubjectArgs>
    template <typename U>
    ObservingObject<ObjectType, SubjectArgs...>::ObservingObject(const ObjectObserved<U>& observedObject, const ObjectType& value):
        ObservingObject{observedObject.GetSubject(), std::forward<ObjectType>(value)}
    {}
    template <cInterchangeable ObjectType, typename ...SubjectArgs>
    template <typename U>
    ObservingObject<ObjectType, SubjectArgs...>::ObservingObject(const ObjectObserved<U>& observedObject, ObjectType&& value):
        ObservingObject{observedObject.GetSubject(), std::forward<ObjectType>(value)}
    {}

    template <cInterchangeable ObjectType, typename ...SubjectArgs>
    void ObservingObject<ObjectType, SubjectArgs...>::SaveSubject(Subject<SubjectArgs...>* pSubject)
    {
        if (!pSubject) OutputDebugString(_T("Subject was nullptr when trying to save it to the ObservingObject Observer."));
        else m_pSubject = pSubject;
    }
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
