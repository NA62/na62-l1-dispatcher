/*
 * MyOptions.h
 *
 *  Created on: Apr 11, 2014
 \*      Author: Jonas Kunze (kunze.jonas@gmail.com)
 */

#pragma once
#ifndef MYOPTIONS_H_
#define MYOPTIONS_H_

#include <options/Options.h>
#include <string>
#include <boost/thread.hpp>

/*
 * Listening Ports
 */
#define OPTION_ZMQ_RECEIVER_PORT (char*)"zmqReceivePort"
#define OPTION_ETH_DEVICE_NAME (char*)"ethDeviceName"

/*
 * MRP rate
 */
#define OPTION_MIN_USEC_BETWEEN_L1_REQUESTS (char*)"minUsecsBetweenL1Requests"

namespace na62 {
class MyOptions: public Options {
public:
	MyOptions();
	virtual ~MyOptions();

	static void Load(int argc, char* argv[]) {
		desc.add_options()

		(OPTION_CONFIG_FILE,
				po::value<std::string>()->default_value(
						"/etc/na62-l1-dispatcher.cfg"),
				"Config file for the options shown here")

		(OPTION_ETH_DEVICE_NAME,
				po::value<std::string>()->default_value("dna0"),
				"Name of the device to be used for sending MRPs")

		(OPTION_ZMQ_RECEIVER_PORT, po::value<int>()->default_value(131816),
				"ZMQ-TCP-Port used to receive L1 requests to be dispatched")

		(OPTION_MIN_USEC_BETWEEN_L1_REQUESTS,
				po::value<int>()->default_value(1000),
				"Minimum time between two MRPs sent to the CREAMs")

				;

		Options::Initialize(argc, argv, desc);
	}
};

} /* namespace na62 */

#endif /* MYOPTIONS_H_ */
