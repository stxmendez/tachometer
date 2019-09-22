#include "circular_queue.h"

CircularQueue::CircularQueue(int size) {
	entries_ = new unsigned long[size];
	full_ = false;
	head_ = tail_ = 0;
	max_ = size;
}

bool CircularQueue::IsEmpty() {
	return size() == 0;
}

bool CircularQueue::IsFull() {
	return head_ == tail_ && full_;
}

int CircularQueue::Pop(unsigned long* entry) {
	if (head_ == tail_ && !full_) {
		// queue is empty
		return -1;
	}
	
	*entry = entries_[tail_];
	full_ = false;
	tail_ = (tail_ + 1) % max_;
	return 0;
	
}

void CircularQueue::Push(unsigned long entry) {
	entries_[head_] = entry;
	
	if (full_) {
		tail_ = (tail_ + 1) % max_;
	}
	
	head_ = (head_ + 1) % max_;
	full_ = head_ == tail_;
}

int CircularQueue::size() {
	if (full_) {
		return max_;
	}
	
	return (head_ + max_ - tail_) % max_;
}
