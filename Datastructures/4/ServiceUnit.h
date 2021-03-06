//
// Created by ed on 31.10.16.
//

#ifndef QUEUE_SERVICEUNIT_H
#define QUEUE_SERVICEUNIT_H

#include <cassert>
#include <exception>
#include <type_traits>

#include "Queue.h"
#include "RequestUnit.h"
#include "QueueArray.h"
#include "RandomGenerator.h"

template<template<typename> class QUEUE>
class ServiceUnit : public Queue<RequestUnit> {
	private:
		QUEUE<RequestUnit> requests;

		double curr_time = 0;
		double idle_time = 0;

		double min_proc_time = 0;
		double max_proc_time = 0;

		double sum_proc_time = 0;
		double sum_queuing_time = 0;
		int sum_size = 0;

		int proc_count = 0;

	public:
		class NotSupportedQueueTypeException : public std::exception {};

		ServiceUnit(double min_proc_time, double max_proc_time);

		bool SynchronizeTime(double time);

		template<template<typename> class QUEUE_T>
		bool SynchronizeTime(const ServiceUnit<QUEUE_T>&);

		virtual void ForEach(const std::function<void(RequestUnit&)>& function) override;
		virtual void ForEach(const std::function<void(const RequestUnit&)>& function) const override;

		double get_curr_time() const;
		double get_idle_time() const;

		double get_min_proc_time() const;
		double get_max_proc_time() const;

		int get_proc_count() const;
		double get_sum_proc_time() const;

		double get_average_proc_time() const;
		double get_average_size() const;
		double get_average_queueing_time() const;

		void set_min_proc_time(double min_proc_time);
		void set_max_proc_time(double max_proc_time);

		virtual size_t get_memory_size() const override;
	protected:
		virtual void _enqueue(const RequestUnit& request) override;
		virtual RequestUnit _dequeue() override;
		virtual const RequestUnit& _first() const override;
		virtual const RequestUnit& _last() const override;

};

template<template<typename> class QUEUE>
ServiceUnit<QUEUE>::ServiceUnit(double min_proc_time, double max_proc_time)
	: min_proc_time(min_proc_time), max_proc_time(max_proc_time) {}

template<template<typename> class QUEUE>
double ServiceUnit<QUEUE>::get_curr_time() const {
	return curr_time;
}

template<template<typename> class QUEUE>
double ServiceUnit<QUEUE>::get_idle_time() const {
	return idle_time;
}

template<template<typename> class QUEUE>
double ServiceUnit<QUEUE>::get_min_proc_time() const {
	return min_proc_time;
}

template<template<typename> class QUEUE>
double ServiceUnit<QUEUE>::get_max_proc_time() const {
	return max_proc_time;
}

template<template<typename> class QUEUE>
int ServiceUnit<QUEUE>::get_proc_count() const {
	return proc_count;
}

template<template<typename> class QUEUE>
double ServiceUnit<QUEUE>::get_average_proc_time() const {
	return (double) sum_proc_time / proc_count;
}

template<template<typename> class QUEUE>
double ServiceUnit<QUEUE>::get_average_size() const {
	return (double) sum_size / proc_count;
}

template<template<typename> class QUEUE>
double ServiceUnit<QUEUE>::get_average_queueing_time() const {
	return (double) sum_queuing_time / proc_count;
}

template<template<typename> class QUEUE>
bool ServiceUnit<QUEUE>::SynchronizeTime(double time) {
	if (time < curr_time) return false;

	idle_time += time - curr_time;
	curr_time = time;
	return true;
}

template<template<typename> class QUEUE>
template<template<typename> class QUEUE_T>
bool ServiceUnit<QUEUE>::SynchronizeTime(const ServiceUnit<QUEUE_T>& queue) {
	return SynchronizeTime(queue.get_curr_time());
}

template<template<typename> class QUEUE>
void ServiceUnit<QUEUE>::ForEach(const std::function<void(RequestUnit&)>& function) {
	return requests.ForEach(function);
}

template<template<typename> class QUEUE>
void ServiceUnit<QUEUE>::ForEach(const std::function<void(const RequestUnit&)>& function) const {
	return requests.ForEach(function);
}

template<template<typename> class QUEUE>
void ServiceUnit<QUEUE>::set_min_proc_time(double min_proc_time) {
	ServiceUnit<QUEUE>::min_proc_time = min_proc_time;
}

template<template<typename> class QUEUE>
void ServiceUnit<QUEUE>::set_max_proc_time(double max_proc_time) {
	ServiceUnit<QUEUE>::max_proc_time = max_proc_time;
}

template<template<typename> class QUEUE>
void ServiceUnit<QUEUE>::_enqueue(const RequestUnit& t) {
	RequestUnit request;
	request.proc_count = t.proc_count;

	requests.Enqueue(request);
}

template<template<typename> class QUEUE>
RequestUnit ServiceUnit<QUEUE>::_dequeue() {
	RequestUnit request = requests.Dequeue();

	double proc_time = request.service_time =
		                   min_proc_time + (max_proc_time - min_proc_time) * RandomGenerator::GetRandomDouble01();

	requests.ForEach([&proc_time](RequestUnit& a) {
		a.queuing_time += proc_time;
	});

	sum_queuing_time += request.queuing_time;
	sum_proc_time += proc_time;

	curr_time += proc_time;
	sum_size += get_size();

	++proc_count;

	++request.proc_count;

	return request;
}

template<template<typename> class QUEUE>
const RequestUnit& ServiceUnit<QUEUE>::_first() const {
	return requests.First();
}

template<template<typename> class QUEUE>
const RequestUnit& ServiceUnit<QUEUE>::_last() const {
	return requests.Last();
}

template<template<typename> class QUEUE>
double ServiceUnit<QUEUE>::get_sum_proc_time() const {
	return sum_proc_time;
}

template<template<typename> class QUEUE>
size_t ServiceUnit<QUEUE>::get_memory_size() const {
	return sizeof(*this) + requests.get_memory_size();
}

#endif //QUEUE_SERVICEUNIT_H
