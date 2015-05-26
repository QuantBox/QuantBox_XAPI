#ifndef _ARRAYLOCKFREEQUEUE_H___
#define _ARRAYLOCKFREEQUEUE_H___

#include <stdint.h>

#ifdef _WIN64
#define QUEUE_INT LONG64
#else
#define QUEUE_INT unsigned long
#endif

#define ARRAY_LOCK_FREE_Q_DEFAULT_SIZE 65535 // 2^16

template <typename ELEM_T, QUEUE_INT Q_SIZE = ARRAY_LOCK_FREE_Q_DEFAULT_SIZE>
class ArrayLockFreeQueue
{
public:

	ArrayLockFreeQueue();
	virtual ~ArrayLockFreeQueue();

	QUEUE_INT size();

	bool enqueue(const ELEM_T &a_data);

	bool dequeue(ELEM_T &a_data);

    bool try_dequeue(ELEM_T &a_data);

private:

	ELEM_T m_thequeue[Q_SIZE];

	volatile QUEUE_INT m_count;
	volatile QUEUE_INT m_writeIndex;

	volatile QUEUE_INT m_readIndex;

	volatile QUEUE_INT m_maximumReadIndex;

	inline QUEUE_INT countToIndex(QUEUE_INT a_count);
};

#include "ArrayLockFreeQueueImp.h"

#endif
