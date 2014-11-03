//============================================================================
// Name        : NA62 online trigger PC farm framework
// Author      : Jonas Kunze (kunze.jonas@gmail.com)
//============================================================================

#include <boost/asio.hpp>
#include <boost/thread.hpp>
#include <utils/AExecutable.h>
#include <zmq.h>
#include <zmq.hpp>
#include <cstdbool>
#include <sstream>
#include <string>

#ifdef USE_GLOG
#include <glog/logging.h>
#endif
#include <socket/NetworkHandler.h>
#include <unistd.h>
#include <csignal>
#include <iostream>
#include <vector>
#include <socket/ZMQHandler.h>

#include "options/MyOptions.h"
#include "L1Dispatcher.h"

using namespace std;
using namespace na62;

void handle_stop(const boost::system::error_code& error, int signal_number) {
	std::cout << "Received signal " << signal_number << " - Shutting down"
			<< std::endl;

	usleep(100);
	if (!error) {
		ZMQHandler::Stop();
		AExecutable::InterruptAll();

		usleep(1000);
		std::cout << "Stopping ZMQ handler" << std::endl;
		ZMQHandler::shutdown();

		std::cout << "Cleanly shut down na62-farm" << std::endl;
		AExecutable::JoinAll();
	}
}

int main(int argc, char* argv[]) {
	/*
	 * Signals
	 */
	boost::asio::io_service signalService;
	boost::asio::signal_set signals(signalService, SIGINT, SIGTERM, SIGQUIT);
	signals.async_wait(handle_stop);
	boost::thread signalThread(
			boost::bind(&boost::asio::io_service::run, &signalService));

	/*
	 * Static Class initializations
	 */
	MyOptions::Load(argc, argv);

	ZMQHandler::Initialize(1);

	/*
	 * initialize NIC handler and start gratuitous ARP request sending thread
	 */
	NetworkHandler NetworkHandler(Options::GetString(OPTION_ETH_DEVICE_NAME));
//	NetworkHandler.startThread("ArpSender");

	zmq::socket_t* socket = ZMQHandler::GenerateSocket(ZMQ_PULL);
	std::stringstream address;
	address << "tcp://*:" << MyOptions::GetInt(OPTION_ZMQ_RECEIVER_PORT);
	socket->bind(address.str().c_str());

	L1Dispatcher dispatcher;

	dispatcher.startThread(0, "L1Dispatcher");

	while (true) {
		zmq::message_t* msg = new zmq::message_t();
		try {
			socket->recv(msg);
			dispatcher.storeRequest(msg);
		} catch (const zmq::error_t& ex) {
			if (ex.num() != EINTR) { // try again if EINTR (signal caught)
				ZMQHandler::DestroySocket(socket);
				break;
			}
			delete msg;
		}
	}

	/*
	 * Join all threads
	 */
	AExecutable::JoinAll();
	return 0;
}
