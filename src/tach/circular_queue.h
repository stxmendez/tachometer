#ifndef CIRCULAR_QUEUE_H
#define CIRCULAR_QUEUE_H

class CircularQueue {
public:
	CircularQueue(int size);

	bool IsEmpty();
	bool IsFull();
	int Pop(unsigned long* entry);
	void Push(unsigned long entry);
	int size();

private:
	unsigned long* entries_;
	bool full_;
	int head_;
	int tail_;
	int max_;
};

#endif // CIRCULAR_QUEUE_H
