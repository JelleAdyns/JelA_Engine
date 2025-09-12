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
                auto pos = std::find(m_pVecObservers.cbegin(), m_pVecObservers.cend(), pObserver);
                if (pos == m_pVecObservers.cend())
                {
                    m_pVecObservers.emplace_back(pObserver);
                }
                else OutputDebugString(_T("Observer already subscribed to Subject"));
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

        Observer(const Observer&) = default;
        Observer(Observer&&) noexcept = default;
        Observer& operator= (const Observer&) = default;
        Observer& operator= (Observer&&) noexcept = default;

        virtual void Notify(Args...  args) = 0;
        virtual void OnSubjectDestroy(Subject<Args...>* pSubject) = 0;

    protected:
        Observer() = default;
    };




    template <typename... Args>
    class TrackSubjectsObserver : public Observer<Args...>
    {
    public:

        virtual ~TrackSubjectsObserver()
        {
            for (auto& pSubject : m_pVecSubjects)
            {
                if(pSubject) pSubject->RemoveObserver(this);
            }
        }

        TrackSubjectsObserver(const TrackSubjectsObserver& other)
            : m_pVecSubjects{ other.m_pVecSubjects }
        {
            for (auto& pSubject : m_pVecSubjects)
            {
                if (pSubject) pSubject->AddObserver(this);
            }
        }
        TrackSubjectsObserver(TrackSubjectsObserver&& other) noexcept
            : m_pVecSubjects{ std::move(other.m_pVecSubjects)}
        {
            for (auto& pSubject : m_pVecSubjects)
            {
                if (pSubject)
                {
                    pSubject->RemoveObserver(&other);
                    pSubject->AddObserver(this);
                }
            }

            other.m_pVecSubjects.clear();
        }
        TrackSubjectsObserver& operator= (const TrackSubjectsObserver& other)
        {
            m_pVecSubjects = other.m_pVecSubjects;

            for (auto& pSubject : m_pVecSubjects)
            {
                if (pSubject) pSubject->AddObserver(this);
            }
        }
        TrackSubjectsObserver& operator= (TrackSubjectsObserver&& other) noexcept
        {
            m_pVecSubjects = std::move(other.m_pVecSubjects);
            
            for (auto& pSubject : m_pVecSubjects)
            {
                if (pSubject)
                {
                    pSubject->RemoveObserver(&other);
                    pSubject->AddObserver(this);
                }
            }

            other.m_pVecSubjects.clear();
        }

        virtual void Notify(Args...  args) = 0;
        virtual void OnSubjectDestroy(Subject<Args...>* pSubject) = 0;
        virtual void SaveSubject(Subject<Args...>* pSubject)
        {
            if (!pSubject)
            {
                OutputDebugString(_T("Subject was nullptr when trying to save it to a TrackSubjectsObserver."));
                return;
            }

            auto itSubjectPtr = std::ranges::find(m_pVecSubjects, pSubject);
            if (itSubjectPtr == m_pVecSubjects.cend())
                m_pVecSubjects.emplace_back(pSubject);
            else OutputDebugString(_T("Subject pointer already added to the TrackSubjectsObserver."));
        }

    protected:
        TrackSubjectsObserver() : Observer<Args...>{} {};
    
        std::vector<Subject<Args...>*> m_pVecSubjects{};
    };




    template <typename... Args>
    class SingleSubjectObserver : public Observer<Args...>
    {
    public:

        virtual ~SingleSubjectObserver()
        {
            if (m_pSubject) m_pSubject->RemoveObserver(this);
        }

        SingleSubjectObserver(const SingleSubjectObserver& other)
            : m_pSubject{ other.m_pSubject }
        {
            if (m_pSubject) m_pSubject->AddObserver(this); 
        }

        SingleSubjectObserver(SingleSubjectObserver&& other) noexcept
            : m_pSubject{ std::move(other.m_pSubject) }
        {
            if (m_pSubject)
            {
                m_pSubject->RemoveObserver(&other);
                m_pSubject->AddObserver(this);
            }

            other.m_pSubject = nullptr;
        }

        SingleSubjectObserver& operator= (const SingleSubjectObserver& other)
        {
            m_pSubject = other.m_pSubject;
            if (m_pSubject) m_pSubject->AddObserver(this);
        }

        SingleSubjectObserver& operator= (SingleSubjectObserver&& other) noexcept
        {
            m_pSubject = std::move(other.m_pSubject);

            if (m_pSubject)
            {
                m_pSubject->RemoveObserver(&other);
                m_pSubject->AddObserver(this);
            }

            other.m_pSubject = nullptr;
        }

        virtual void Notify(Args...  args) = 0;
        virtual void OnSubjectDestroy(Subject<Args...>* pSubject) = 0;
        virtual void SaveSubject(Subject<Args...>* pSubject)
        {
            if (!pSubject) OutputDebugString(_T("Subject was nullptr when trying to save it to a SingleSubjectsObserver."));
            else m_pSubject = pSubject;
        }

    protected:
        SingleSubjectObserver() : Observer<Args...>{} {};

        Subject<Args...>* m_pSubject{};
    };
}


#endif // !OBSERVER_H