#ifndef OBSERVER_H
#define OBSERVER_H

#include <vector>
#include "Defines.h"

namespace jela
{

    template <typename... Args>
    class Observer;
    template <typename... Args>
    class Subject final
    {
    public:
        Subject() = default;
        ~Subject()
        {
            for (Observer< Args... >* pObserver : m_pVecObservers)
            {
                pObserver->OnSubjectDestroy(this);
            }
        }

        Subject(const Subject&) = default;
        Subject(Subject&&) noexcept = default;
        Subject& operator= (const Subject&) = default;
        Subject& operator= (Subject&&) noexcept = default;

        void AddObserver(Observer<Args... >* pObserver)
        {
            if (pObserver)
            {
                if (!HasObserver(pObserver)) m_pVecObservers.emplace_back(pObserver);
                else OutputDebugString(_T("Observer already subscribed to Subject."));
            }
            else OutputDebugString(_T("Tried to add a nullptr to the Subject."));
        }
        void RemoveObserver(Observer<Args... >* pObserver)
        {
            if (!m_pVecObservers.empty() && pObserver)
            {
                auto amountErased = std::erase(m_pVecObservers, pObserver);
                if (amountErased == 0) OutputDebugString(_T("Couldn't find Observer to remove in the vector. Continuing.\n"));
            }
        }
        bool HasObserver(Observer<Args... >* pObserver)
        {       
            return std::ranges::find(m_pVecObservers, pObserver) != m_pVecObservers.cend();
        }
        void NotifyObservers(Args...  args)
        {
            for (Observer<Args... >* pObserver : m_pVecObservers)
            {
                pObserver->Notify(args...);
            }
        }
    private:
        std::vector<Observer<Args... >*> m_pVecObservers;

    };



    template <typename... Args>
    class Observer
    {
    public:

        virtual ~Observer() = default;

        Observer(const Observer&) = default;
        Observer(Observer&&) noexcept = default;
        Observer& operator= (const Observer&) = default;
        Observer& operator= (Observer&&) noexcept = default;

        virtual void Notify(Args...  args) = 0;
        virtual void OnSubjectDestroy(Subject<Args...>* pSubject) = 0;

    protected:
        Observer() = default;
    };


}


#endif // !OBSERVER_H