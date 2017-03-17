/*

SDK function covered :
IS_GetContactQuality
IS_GetWirelessSignalStatus
IS_GetBatteryChargeLevel

- Test Insight headset returns information about battery level, wireless strength, contact quality

*/
#define _WIN32_WINNT 0x0501
#define BOOST_TEST_MODULE Headset_Information_logger
#define BOOST_TEST_NO_MAIN

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
int const runtime = 10;
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
		//BOOST_CHECK_EQUAL(IEE_EngineConnect(), EDK_OK);

		std::string edfFile = "../../edf/test.edf";
		std::string motionFile = "../../edf/test.edf";
		//IEE_EngineLocalConnect(edfFile.c_str(), motionFile.c_str());
		//IEE_EdfStart();
		BOOST_CHECK_MESSAGE(IEE_EngineConnect() == EDK_OK, "ERROR: Engine Connect failed");

		BOOST_TEST_MESSAGE("Connecting headset");

		eEvent = IEE_EmoEngineEventCreate();
		eState = IEE_EmoStateCreate();
	}

	//run after every test case finish
	~Fixture() {
		//BOOST_TEST_MESSAGE("Disconnect headset \n Free memory referenced by an EmoState handle \n Free memory referenced by an event handle");
		IEE_EngineDisconnect();
		IEE_EmoStateFree(eState);
		IEE_EmoEngineEventFree(eEvent);
	}
	EmoEngineEventHandle    eEvent;
	EmoStateHandle          eState;

};

//define and bind suite to fixture at one macrco
BOOST_FIXTURE_TEST_SUITE(Headset_Information_logger, Fixture)

//test case
BOOST_AUTO_TEST_CASE(TC01_GIVEN_Insight_headset_WHEN_connected_THEN_return_headset_information)
{
	boost::asio::io_service io;
	boost::asio::deadline_timer timer(io, boost::posix_time::seconds(test_case_timer.count()));
	timer.async_wait(&on_timeout);
	std::thread asioThread([&] { return io.run(); });
	isTimeout = false; //reset expiration timer

	unsigned int userID = 0;
	int state = 0;
	IEE_EEG_ContactQuality_t* contactQuality = new IEE_EEG_ContactQuality_t;
	float systemUpTime = 0;
	int batteryLevel, maxBatteryLevel = 0;
	IEE_SignalStrength_t wirelessStrength;
	bool onStateChanged = false;

	while (true) {

		state = IEE_EngineGetNextEvent(eEvent);
		if (state == EDK_OK) {

			IEE_Event_t eventType = IEE_EmoEngineEventGetType(eEvent);
			IEE_EmoEngineEventGetUserId(eEvent, &userID);

			switch (eventType)
			{
			case IEE_UserAdded:
				BOOST_TEST_MESSAGE("User added");
				std::cout << "User added" << '\n';
				std::cout << "Uptime - wireless - battery - AF3 - T7 - PZ - T8 - AF4" << '\n';
				break;
			case IEE_UserRemoved:
				BOOST_TEST_MESSAGE("User removed");
				std::cout << "User removed" << '\n';
				break;
			case IEE_EmoStateUpdated:
				onStateChanged = true;
				IEE_EmoEngineEventGetEmoState(eEvent, eState);
				break;
			default:
				break;
			}
		}

		if (onStateChanged)
		{
			onStateChanged = false;
			systemUpTime = IS_GetTimeFromStart(eState);
			wirelessStrength = IS_GetWirelessSignalStatus(eState);
			
			if (wirelessStrength != NO_SIG)
			{
				//std::cout << "Time: " << systemUpTime << std::endl;
				IS_GetBatteryChargeLevel(eState, &batteryLevel, &maxBatteryLevel);
				BOOST_CHECK(systemUpTime > 0);
				BOOST_CHECK(wirelessStrength > 0 && wirelessStrength <= 4);
				BOOST_CHECK(batteryLevel >= 0 && batteryLevel <= 4);
				BOOST_CHECK((IS_GetContactQuality(eState, IEE_CHAN_AF3) >= 0) && (IS_GetContactQuality(eState, IEE_CHAN_AF3) <= 4));
				BOOST_CHECK((IS_GetContactQuality(eState, IEE_CHAN_T7) >= 0) && (IS_GetContactQuality(eState, IEE_CHAN_T7) <= 4));
				BOOST_CHECK((IS_GetContactQuality(eState, IEE_CHAN_Pz) >= 0) && (IS_GetContactQuality(eState, IEE_CHAN_Pz) <= 4));
				BOOST_CHECK((IS_GetContactQuality(eState, IEE_CHAN_T8) >= 0) && (IS_GetContactQuality(eState, IEE_CHAN_T8) <= 4));
				BOOST_CHECK((IS_GetContactQuality(eState, IEE_CHAN_AF4) >= 0) && (IS_GetContactQuality(eState, IEE_CHAN_AF4) <= 4));
				std::cout << '\r'<< std::fixed << std::setprecision(2) << systemUpTime << "         "
					<< wirelessStrength << "         "
					<< batteryLevel << "      "
					<< IS_GetContactQuality(eState, IEE_CHAN_AF3) << "     "
					<< IS_GetContactQuality(eState, IEE_CHAN_T7) <<	"    "
					<< IS_GetContactQuality(eState, IEE_CHAN_Pz) << "    "
					<< IS_GetContactQuality(eState, IEE_CHAN_T8) << "    "
					<< IS_GetContactQuality(eState, IEE_CHAN_AF4);
			}
		}

		if (isTimeout)
		{
			delete contactQuality;
			break;
		}
	}
	asioThread.join();
}

//end of suite
BOOST_AUTO_TEST_SUITE_END()
