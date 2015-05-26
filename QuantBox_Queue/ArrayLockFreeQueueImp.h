#ifndef _ARRAYLOCKFREEQUEUEIMP_H___
#define _ARRAYLOCKFREEQUEUEIMP_H___
#include "ArrayLockFreeQueue.h"

#include <assert.h>
#include "atom_opt.h"

template <typename ELEM_T, QUEUE_INT Q_SIZE>
ArrayLockFreeQueue<ELEM_T, Q_SIZE>::ArrayLockFreeQueue() :
	m_writeIndex(0),
	m_readIndex(0),
	m_maximumReadIndex(0)
{

	m_count = 0;

}

template <typename ELEM_T, QUEUE_INT Q_SIZE>
ArrayLockFreeQueue<ELEM_T, Q_SIZE>::~ArrayLockFreeQueue()
{

}

template <typename ELEM_T, QUEUE_INT Q_SIZE>
inline QUEUE_INT ArrayLockFreeQueue<ELEM_T, Q_SIZE>::countToIndex(QUEUE_INT a_count)
{
	return (a_count % Q_SIZE);
}

template <typename ELEM_T, QUEUE_INT Q_SIZE>
QUEUE_INT ArrayLockFreeQueue<ELEM_T, Q_SIZE>::size()
{
	QUEUE_INT currentWriteIndex = m_writeIndex;
	QUEUE_INT currentReadIndex = m_readIndex;

	if(currentWriteIndex>=currentReadIndex)
		return currentWriteIndex - currentReadIndex;
	else
		return Q_SIZE + currentWriteIndex - currentReadIndex;

}

template <typename ELEM_T, QUEUE_INT Q_SIZE>
bool ArrayLockFreeQueue<ELEM_T, Q_SIZE>::enqueue(const ELEM_T &a_data)
{
	QUEUE_INT currentWriteIndex;
	QUEUE_INT currentReadIndex;
	do
	{
		currentWriteIndex = m_writeIndex;
		currentReadIndex = m_readIndex;
		if(countToIndex(currentWriteIndex + 1) ==
			countToIndex(currentReadIndex))
		{
			return false;
		}
	} while(!CAS(&m_writeIndex, currentWriteIndex, (currentWriteIndex+1)));

	m_thequeue[countToIndex(currentWriteIndex)] = a_data;

	while(!CAS(&m_maximumReadIndex, currentWriteIndex, (currentWriteIndex + 1)))
	{
		sched_yield();
	}

	AtomicAdd(&m_count, 1);

	return true;

}

template <typename ELEM_T, QUEUE_INT Q_SIZE>
bool ArrayLockFreeQueue<ELEM_T, Q_SIZE>::try_dequeue(ELEM_T &a_data)
{
    return dequeue(a_data);
}

template <typename ELEM_T, QUEUE_INT Q_SIZE>
bool ArrayLockFreeQueue<ELEM_T, Q_SIZE>::dequeue(ELEM_T &a_data)
{
	QUEUE_INT currentMaximumReadIndex;
	QUEUE_INT currentReadIndex;

	do
	{
		currentReadIndex = m_readIndex;
		currentMaximumReadIndex = m_maximumReadIndex;

		if(countToIndex(currentReadIndex) ==
			countToIndex(currentMaximumReadIndex))
		{
			return false;
		}

		a_data = m_thequeue[countToIndex(currentReadIndex)];

		if(CAS(&m_readIndex, currentReadIndex, (currentReadIndex + 1)))
		{
			AtomicSub(&m_count, 1);
			return true;
		}
	} while(true);

	assert(0);

	return false;

}

#endif
