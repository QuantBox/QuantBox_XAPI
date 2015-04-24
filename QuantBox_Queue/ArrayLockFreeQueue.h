#ifndef _ARRAYLOCKFREEQUEUE_H___
#define _ARRAYLOCKFREEQUEUE_H___

#include <stdint.h>

#define ARRAY_LOCK_FREE_Q_DEFAULT_SIZE 65535 // 2^16

template <typename ELEM_T, uint32_t Q_SIZE = ARRAY_LOCK_FREE_Q_DEFAULT_SIZE>
class ArrayLockFreeQueue
{
public:

	ArrayLockFreeQueue();
	virtual ~ArrayLockFreeQueue();

	uint32_t size();

	bool enqueue(const ELEM_T &a_data);

	bool dequeue(ELEM_T &a_data);

    bool try_dequeue(ELEM_T &a_data);

private:

	ELEM_T m_thequeue[Q_SIZE];

	volatile uint32_t m_count;
	volatile uint32_t m_writeIndex;

	volatile uint32_t m_readIndex;

	volatile uint32_t m_maximumReadIndex;

	inline uint32_t countToIndex(uint32_t a_count);
};

#include "ArrayLockFreeQueueImp.h"

#endif
