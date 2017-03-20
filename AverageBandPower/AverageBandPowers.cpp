#define _WIN32_WINNT 0x0501

#define BOOST_TEST_MODULE Average_Band_Powers
#define BOOST_TEST_NO_MAIN

/*

SDK function covered :
	IEE_FFTSetWindowingType
	IEE_GetAverageBandPowers

*/

#include <iostream>
#include <fstream>
#include <conio.h>

#include "Iedk.h"
#include "IedkErrorCode.h"
#include "IEmoStateDLL.h"
#include <iomanip>

#include <boost/test/auto_unit_test.hpp>
#include <boost/asio.hpp>
#include <atomic>
#include <chrono>
#include <thread>
#include <string>
#include <boost/date_time/posix_time/posix_time.hpp>
#include <atomic>
#include <EdfData.h>

namespace utf = boost::unit_test;
std::atomic<bool> isTimeout(false);

// Set timeout
int const runtime = 5;
constexpr std::chrono::seconds test_case_timer(runtime);
void on_timeout(const boost::system::error_code& /*e*/)
{
	isTimeout = true;
	BOOST_TEST_MESSAGE("Timer canceled or expired \n");
}

// Fixture define
struct Fixture {

	//run before every test case start
	Fixture() {
		
		std::cout << "Run time: " << runtime << "s" << '\n';

		//std::string edfFile = "C:\\sdk_auto\\SdkAutoTest\\AverageBandPowers\\avaragebandpowers.edf";
		//std::string motionFile = "C:\\sdk_auto\\SdkAutoTest\\AverageBandPowers\\avaragebandpowers.md.edf";

		std::string edfFile = "avaragebandpowers.edf";
		std::string motionFile = "avaragebandpowers.md.edf";

		//BOOST_CHECK_EQUAL(IEE_EngineLocalConnect(edfFile.c_str(), motionFile.c_str()), EDK_OK);
		//BOOST_CHECK_EQUAL(IEE_EdfStart(), EDK_OK);
		BOOST_CHECK_MESSAGE(IEE_EngineConnect() == EDK_OK, "ERROR: Engine Connect failed");

		BOOST_TEST_MESSAGE("Connecting headset");

		eEvent = IEE_EmoEngineEventCreate();
		eState = IEE_EmoStateCreate();
	}

	//run after every test case finish
	~Fixture() {

		IEE_EngineDisconnect();
		IEE_EmoStateFree(eState);
		IEE_EmoEngineEventFree(eEvent);
	
	}
	EmoEngineEventHandle    eEvent;
	EmoStateHandle          eState;
		
};

//define and bind suite to fixture at one macrco
BOOST_FIXTURE_TEST_SUITE(Average_Band_Powers, Fixture)

//test case
BOOST_AUTO_TEST_CASE(TC01_GIVEN_headset_connected_WHEN_get_average_band_powers_THEN_return_data_successfully)
{
	boost::asio::io_service io;
	boost::asio::deadline_timer timer(io, boost::posix_time::seconds(test_case_timer.count()));
	timer.async_wait(&on_timeout);
	std::thread asioThread([&] { return io.run(); });
	isTimeout = false; //reset expiration timer

	unsigned int engineUserID = -1;
	bool ready = false;
	int state = 0;
	const char header[] = "Theta, Alpha, Low_beta, High_beta, Gamma";
	IEE_DataChannel_t channelList[] = { IED_AF3, IED_AF4, IED_T7, IED_T8, IED_Pz };

	
	while (true)
	{
		state = IEE_EngineGetNextEvent(eEvent);

		if (state == EDK_OK)
		{
			IEE_Event_t eventType = IEE_EmoEngineEventGetType(eEvent);
			IEE_EmoEngineEventGetUserId(eEvent, &engineUserID);

			if (eventType == IEE_UserAdded) {
				std::cout << "User added" << std::endl;
				engineUserID = 0;

				int set_window_type_result = IEE_FFTSetWindowingType(engineUserID, IEE_HAMMING);

				BOOST_CHECK_EQUAL(set_window_type_result, EDK_OK);

				std::cout << header << std::endl;
				ready = true;
			}
		}

		if (ready)
		{
			double alpha, low_beta, high_beta, gamma, theta;
			alpha = low_beta = high_beta = gamma = theta = 0;

			for (int i = 0; i< sizeof(channelList) / sizeof(channelList[0]); ++i)
			{

				int result = IEE_GetAverageBandPowers(engineUserID, channelList[i], &theta, &alpha,
					&low_beta, &high_beta, &gamma);

				BOOST_CHECK_EQUAL(result, EDK_OK);

				if (result == EDK_OK) {					
					std::cout << theta << "," << alpha << "," << low_beta << ",";
					std::cout << high_beta << "," << gamma << std::endl;
				}
			}
		}

		Sleep(1);
	}

	asioThread.join();
}

//end of suite
BOOST_AUTO_TEST_SUITE_END()