#pragma once
#include <iostream>
#include <atomic>
#define SAFE
namespace q {
	template<typename T>
	class ring_queue {
	protected:
		std::atomic<T*> _test; 
		std::atomic<int> _max_size;
		std::atomic<int> _push_pos = 0;
		std::atomic<int> _pop_pos = 0;
		bool safe(int push_pos, int pop_pos) {
			bool safe = true;
			if (pop_pos > push_pos) {
				safe = false;
			}
			else if (push_pos - pop_pos > _max_size) {
				safe = false;
			}
			return safe;
		}
	public:
		int push(T test) {
			// queue is full
			if (_push_pos - _pop_pos >= _max_size) {
				return -1;
			}
			_test[_push_pos % _max_size] = test;
			_push_pos++;
			return 0;
		}
		T pop() {
			// check if queue is empty
			if (_pop_pos == _push_pos) {
				return NULL;
			}
			T value = _test[_pop_pos % _max_size];
			_pop_pos++;
			return value;

		}
		T front() {
			return _test[_pop_pos % _max_size];
		}
		bool empty() {
			if (_pop_pos != _push_pos) {
				return false;
			}
			else {
				return true;
			}
		}
		bool full() {
			if (_push_pos - _pop_pos >= _max_size) {
				return true;
			}
			else {
				return false;
			}
		}
		void print() {
			bool empty = this->empty();
			bool full = this->full();
			std::cout << "push pos: " << _push_pos << "\n";
			std::cout << "pop pos: " << _pop_pos << "\n";
			std::cout << "queue is empty?: " << empty << "\n";
			std::cout << "queue is full?: " << full << "\n";
			for (auto i = 0; i < _max_size; i++) {
				std::cout << _test[i];
				if (i == _push_pos % _max_size && i == _pop_pos % _max_size) { std::cout << " <- push and pop are here"; }
				else if (i == _push_pos % _max_size) { std::cout << " <- push is here"; }
				else if (i == _pop_pos % _max_size) { std::cout << " <- pop is here"; }
				std::cout << "\n";
			}
			std::cout << "\n";
		}
	};
	template <typename T, int max_size>
	class s_rqueue : public ring_queue<T> {
	private:
		T _static_array_queue[max_size];
	public:
		s_rqueue() {
			for (int i = 0; i < max_size; i++) {
				_static_array_queue[i] = NULL;
			}
			this->_test = _static_array_queue;
			this->_max_size = max_size;
		}
	};
	template <typename T>
	class d_rqueue : public ring_queue<T> {
	private:
		T* _dynamic_array_queue;
	public:
		d_rqueue() {
			this->_test = NULL;
			this->_max_size = 0;
		}
		d_rqueue(int max_size) {
			_dynamic_array_queue = new T[max_size]();
			this->_test = _dynamic_array_queue;
			this->_max_size = max_size;
		}
		int resize(int max_size) {
			delete[] _dynamic_array_queue;
			_dynamic_array_queue = new T[max_size]();
			this->_test = _dynamic_array_queue;
			this->_max_size = max_size;
			return 0;
		}
		~d_rqueue() {
			delete[] _dynamic_array_queue;
		}
	};
}