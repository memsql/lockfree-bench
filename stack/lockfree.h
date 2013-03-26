
#include "atomic.h"

class LockFreeStack
{
public:
    // The elements we wish to store should inherit Node
    //
    struct Node
    {
        boost::atomic<Node*> next;
    };
    
    // Unfortunately, there is no platform independent way to
    // define this class.  The following definition works in
    // gcc on x86_64 architectures
    //
    class TaggedPointer
    {
    public:
        TaggedPointer(): m_node(nullptr), m_counter(0) {}
        
        Node* GetNode()
        {
            return m_node.load(boost::memory_order_acquire);
        }
        
        uint64_t GetCounter()
        {
            return m_counter.load(boost::memory_order_acquire);
        }
        
        bool CompareAndSwap(Node* oldNode, uint64_t oldCounter, Node* newNode, uint64_t newCounter)
        {
            bool cas_result;
            __asm__ __volatile__
            (
                "lock cmpxchg16b %0;"  // cmpxchg16b sets ZF on success
                "setz       %3;"  // if ZF set, set cas_result to 1

                : "+m" (*this), "+a" (oldNode), "+d" (oldCounter), "=q" (cas_result)
                : "b" (newNode), "c" (newCounter)                  
                : "cc", "memory"
            );
            return cas_result;
        }
    private:
        boost::atomic<Node*> m_node;
        boost::atomic<uint64_t> m_counter;
    }
    // 16-byte alignment is required for double-width
    // compare and swap
    //
    __attribute__((aligned(16)));
    
    bool TryPushStack(Node* entry)
    {
        Node* oldHead;
        uint64_t oldCounter;

        oldHead = m_head.GetNode();
        oldCounter = m_head.GetCounter();
        entry->next.store(oldHead, boost::memory_order_relaxed);
        return m_head.CompareAndSwap(oldHead, oldCounter, entry, oldCounter + 1);
    }
    
    bool TryPopStack(Node*& oldHead, int threadId)
    {
        oldHead = m_head.GetNode();
        uint64_t oldCounter = m_head.GetCounter();
        if(oldHead == nullptr)
        {
            return true;
        }
        m_hazard[threadId*8].store(oldHead, boost::memory_order_seq_cst);
        if(m_head.GetNode() != oldHead)
        {
            return false;
        }
        return m_head.CompareAndSwap(oldHead, oldCounter, oldHead->next.load(boost::memory_order_acquire), oldCounter + 1);
    }
    
    void Push(Node* entry)
    {
        while(true)
        {
            if(TryPushStack(entry))
            {
                return;
            }
            usleep(250);
        }
    }
    
    Node* Pop(int threadId)
    {
        Node* res;
        while(true)
        {
            if(TryPopStack(res, threadId))
            {
                return res;
            }
            usleep(250);
        }
    }
    
private:
    TaggedPointer m_head;
    boost::atomic<Node*> m_hazard[MAX_THREADS*8];
};

