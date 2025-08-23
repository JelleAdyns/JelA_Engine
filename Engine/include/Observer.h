#ifndef OBSERVER_H
#define OBSERVER_H

#include <vector>
#include <stdexcept>

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

        Subject(const Subject&) = delete;
        Subject(Subject&&) noexcept = delete;
        Subject& operator= (const Subject&) = delete;
        Subject& operator= (Subject&&) noexcept = delete;

        void AddObserver(Observer<Args... >* pObserver)
        {
            if (pObserver)
            {
                auto pos = std::find(m_pVecObservers.cbegin(), m_pVecObservers.cend(), pObserver);
                if (pos == m_pVecObservers.cend())
                {
                    m_pVecObservers.push_back(pObserver);
                }
                else throw std::runtime_error("Observer already subscribed to Subject");
            }
        }
        void RemoveObserver(Observer<Args... >* pObserver)
        {
            if (m_pVecObservers.size() > 0)
            {
                auto amountErased = std::erase(m_pVecObservers, pObserver);
                if (amountErased == 0) OutputDebugString(_T("Couldn't find Observer to remove in the vector. Continuing.\n"));
            }
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

        Observer(Observer&) = delete;
        Observer(Observer&&) noexcept = delete;
        Observer& operator= (Observer&) = delete;
        Observer& operator= (Observer&&) noexcept = delete;

        virtual void Notify(Args...  args) = 0;
        virtual void OnSubjectDestroy(Subject<Args...>* pSubject) = 0;

    protected:
        Observer() = default;
    };
}


#endif // !OBSERVER_H