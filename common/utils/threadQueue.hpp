/*
 * threadQueue.hpp
 *
 *  Created on: Jan 26, 2018
 *      Author: luguanglong
 */

#ifndef UTILS_THREADQUEUE_HPP_
#define UTILS_THREADQUEUE_HPP_

#include <pthread.h>

#include <memory>
#include <list>

#include "utils/exceptions.hpp"

namespace shark{

	template <class Type>
	class ThreadQueue{
	public:
		ThreadQueue(){
			int ret = 0;
			ret = pthread_cond_init(&newElementCond, NULL);
			if(ret < 0){
				throw SharkException("pthread cond init fail");
			}
		};

		~ThreadQueue(){
			pthread_cond_destroy(&newElementCond);
		};

		int put(Type element){
			clist.push_back(element);
			pthread_cond_signal(&newElementCond);
			return 0;
		};

		Type get(){
			pthread_mutex_lock(&elementMutex);
			if(clist.empty() == true){
				pthread_cond_wait(&newElementCond, &elementMutex);
			}

			pthread_mutex_unlock(&elementMutex);
			auto element = clist.begin();
			clist.pop_front();
			return *element;
		};

	private:
		std::list<Type> clist;
		pthread_cond_t newElementCond = PTHREAD_COND_INITIALIZER;
		pthread_mutex_t elementMutex = PTHREAD_MUTEX_INITIALIZER;
	};
}

#endif /* UTILS_THREADQUEUE_HPP_ */
