#ifndef OBSERVER_H
#define OBSERVER_H

#include <vector>

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
                pObserver->OnSubjectDestroy();
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
                auto pos = std::find(m_pVecObservers.cbegin(), m_pVecObservers.cend(), pObserver);
                if (pos != m_pVecObservers.cend()) m_pVecObservers.erase(pos);
#ifndef NDEBUG
                else OutputDebugString(_T("Couldn't find Observer to remove in the vector. Continuing.\n"));
#endif // !NDEBUG
            }
        }

        void NotifyObservers(Args...  pSubjectOwner)
        {
            for (Observer<Args... >* pObserver : m_pVecObservers)
            {
                pObserver->Notify(pSubjectOwner...);
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
        virtual void OnSubjectDestroy() = 0;

    protected:
        Observer() = default;
    };
}


#endif // !OBSERVER_H