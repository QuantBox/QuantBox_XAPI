#ifndef _QUEUE_STRUCT_H_
#define _QUEUE_STRUCT_H_

struct ResponeItem
{
	char		type;	// (RequestType)

	void*		pApi1;	// 执行者，不能delete
	void*		pApi2;	// 源头，不能delete

	double		double1;
	double		double2;

	void*		ptr1;	// 需要delete
	int			size1;
	void*		ptr2;	// 需要delete
	int			size2;
	void*		ptr3;	// 需要delete
	int			size3;

	bool		bNeedDelete;
};

#endif//end of _QUEUE_HEADER_H_
