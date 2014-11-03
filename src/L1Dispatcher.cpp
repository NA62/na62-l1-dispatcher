/*
 * L1Dispatcher.cpp
 *
 *  Created on: Nov 3, 2014
 *      Author: root
 */

#include "L1Dispatcher.h"

#include <thread>
#include <sys/types.h>
#include <structs/Network.h>
#include <LKr/MRP.h>
#include <socket/NetworkHandler.h>
#include <zmq.hpp>
#include <glog/logging.h>
#include <utils/Utils.h>

#include "options/MyOptions.h"
#include <boost/timer/timer.hpp>

namespace na62 {

using namespace cream;

L1Dispatcher::L1Dispatcher() :
		sentMRPs_(0) {
	monitorThread_ = new std::thread([this]() {
		while(true) {
			LOG(INFO) <<"Queued MRPs:\t"<< requestQueue_.size();
			LOG(INFO) <<"Sent MRPs:\t"<< sentMRPs_;
			sleep(1);
		}
	});
}

L1Dispatcher::~L1Dispatcher() {
}

void L1Dispatcher::storeRequest(zmq::message_t* request) {
	requestQueue_.push(request);
}

void L1Dispatcher::thread() {
	const uint usecsBetweenSends = MyOptions::GetInt(
	OPTION_MIN_USEC_BETWEEN_L1_REQUESTS);
	while (true) {
		zmq::message_t* msg;
		requestQueue_.pop(msg);
		NetworkHandler::SendFrameConcurrently(0,
				reinterpret_cast<const unsigned char*>(msg->data()),
				msg->size());
		delete msg;
		sentMRPs_++;
		usleep(usecsBetweenSends);
	}
}

} /* namespace na62 */
