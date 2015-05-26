#ifndef _ATOM_OPT_H___
#define _ATOM_OPT_H___

#ifdef __GNUC__
	#define CAS(a_ptr, a_oldVal, a_newVal) __sync_bool_compare_and_swap(a_ptr, a_oldVal, a_newVal)
	#define AtomicAdd(a_ptr,a_count) __sync_fetch_and_add (a_ptr, a_count)
	#define AtomicSub(a_ptr,a_count) __sync_fetch_and_sub (a_ptr, a_count)
	#include <sched.h> // sched_yield()
#else

#include <Windows.h>
#ifdef _WIN64
	#define CAS(a_ptr, a_oldVal, a_newVal) (a_oldVal == InterlockedCompareExchange64(a_ptr, a_newVal, a_oldVal))
	#define sched_yield()	SwitchToThread()
	#define AtomicAdd(a_ptr, num)	InterlockedIncrement64(a_ptr)
	#define AtomicSub(a_ptr, num)	InterlockedDecrement64(a_ptr)
#else
	#define CAS(a_ptr, a_oldVal, a_newVal) (a_oldVal == InterlockedCompareExchange(a_ptr, a_newVal, a_oldVal))
	#define sched_yield()	SwitchToThread()
	#define AtomicAdd(a_ptr, num)	InterlockedIncrement(a_ptr)
	#define AtomicSub(a_ptr, num)	InterlockedDecrement(a_ptr)
#endif

#endif

#endif
