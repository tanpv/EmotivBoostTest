/*
SDK function covered :
	IEE_ActivateLicense
	IEE_LicenseInformation
*/
#define _WIN32_WINNT 0x0501
#define BOOST_TEST_MODULE MultiDongleConnection
#define BOOST_TEST_NO_MAIN
#include <boost/asio.hpp>
#include <boost/test/auto_unit_test.hpp>
#include <boost/date_time/posix_time/posix_time.hpp>
#include <iostream>
#include <atomic>
#include <chrono>
#include <thread>
#include "Iedk.h"
#include "IedkErrorCode.h"

using namespace std;

// handle timer expire
std::atomic<bool> isTimeout(false);
void on_timeout(const boost::system::error_code& /*e*/)
{
	isTimeout = true;
	BOOST_TEST_MESSAGE("Timer canceled or expired \n");
}
// set timeout value here
const int TIME_OUT_AFTER = 120;
constexpr std::chrono::seconds test_case_timer(TIME_OUT_AFTER);


struct EmoEngine_Fixture {

	EmoEngine_Fixture() {
		BOOST_CHECK_EQUAL(IEE_EngineConnect(), EDK_OK);
		eEvent = IEE_EmoEngineEventCreate();
		eState = IEE_EmoStateCreate();
	}

	~EmoEngine_Fixture() {
		IEE_EngineDisconnect();
		IEE_EmoStateFree(eState);
		IEE_EmoEngineEventFree(eEvent);
	}

	EmoEngineEventHandle    eEvent;
	EmoStateHandle          eState;
};


BOOST_FIXTURE_TEST_SUITE(MultiDongleConnection, EmoEngine_Fixture)

BOOST_AUTO_TEST_CASE(TC01_Given_has_one_dongle_When_dongle_plugedin_or_remove_Then_engine_should_be_notified)
{
	unsigned int userID = -1;
	list<int> listUser;
	int state = 0;

	cout << "-----------------------------------------------------------------------" << endl;
	cout << "   This test check the ability to connect to two headsets at the same time" << endl;
	cout << "   It captures event when you plug or unplug dongle" << endl;
	cout << "   Every time you plug or unplug dongle, there is a notice that dongle ID" << endl;
	cout << "   Please prepare atlease 2 dongle to do this test" << endl; 
	cout << endl;
	cout << "   You have 2 mins to do test, Please insert dongle and then remove for check" << endl;
	cout << "-----------------------------------------------------------------------" << endl;

	// handle timer
	boost::asio::io_service io;
	boost::asio::deadline_timer timer(io, boost::posix_time::seconds(test_case_timer.count()));
	timer.async_wait(&on_timeout);
	std::thread asioThread([&] { return io.run(); });
	isTimeout = false; //reset expiration timer

	while (1)
	{
		// get next event
		int state = IEE_EngineGetNextEvent(eEvent);

		// check return state is in one of 2 state
		BOOST_TEST((state == EDK_OK || state == EDK_NO_EVENT));

		if (state == EDK_OK)
		{
			IEE_Event_t eventType = IEE_EmoEngineEventGetType(eEvent);

			//check event type is in one of three types
			BOOST_TEST(((eventType == IEE_EmoStateUpdated) ||
						(eventType == IEE_UserRemoved) ||
						(eventType = IEE_UserAdded)));
				
			//check get user id end ok
			BOOST_TEST(IEE_EmoEngineEventGetUserId(eEvent, &userID) == EDK_OK);

			if (userID == -1)
				continue;

			if (eventType == IEE_EmoStateUpdated)
			{
				IEE_EmoEngineEventGetEmoState(eEvent, eState);
				//cout << "User " << userID << ": " << IS_GetTimeFromStart(eState);
			}
			
			// userremoved event
			else if (eventType == IEE_UserRemoved)
			{
				cout << "user ID: " << userID << " have removed" << endl;
				listUser.remove(userID);
			}
			
			// useradded event 
			else if (eventType == IEE_UserAdded)
			{
				listUser.push_back(userID);
				cout << "user ID: " << userID << " have added" << endl;
			}

			userID = -1;
		}

		if (isTimeout)
		{
			break;
		}
	}

	asioThread.join();
}


BOOST_AUTO_TEST_SUITE_END()