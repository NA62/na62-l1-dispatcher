/*
 * L1Dispatcher.h
 *
 *  Created on: Nov 3, 2014
 *      Author: root
 */

#ifndef L1DISPATCHER_H_
#define L1DISPATCHER_H_

#include <sys/types.h>
#include <tbb/concurrent_queue.h>
#include <utils/AExecutable.h>
#include <zmq.hpp>

namespace zmq {
class message_t;
} /* namespace zmq */

namespace std {
class thread;
} /* namespace std */

namespace na62 {

class L1Dispatcher: public AExecutable {
public:
	L1Dispatcher();
	virtual ~L1Dispatcher();
	void storeRequest(zmq::message_t* request);

private:
	tbb::concurrent_bounded_queue<zmq::message_t*> requestQueue_;
	std::thread* monitorThread_;
	uint sentMRPs_;

	void thread();


};

} /* namespace na62 */

#endif /* L1DISPATCHER_H_ */
