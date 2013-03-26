#include <mutex>
#include <stack>

template<class T>
class LockedStack
{
public:
    void Push(T* entry)
    {
        std::lock_guard<std::mutex> lock(m_mutex);
        m_stack.push(entry);
    }
    
    // For compatability with the LockFreeStack interface,
    // add an unused int parameter.
    //
    T* Pop(int)
    {
        std::lock_guard<std::mutex> lock(m_mutex);
        if(m_stack.empty())
        {
            return nullptr;
        }
        T* ret = m_stack.top();
        m_stack.pop();
        return ret;
    }
    
private:
    std::stack<T*> m_stack;
    std::mutex m_mutex;
};

