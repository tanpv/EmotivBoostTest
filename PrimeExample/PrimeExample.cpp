/****************************************************************************
** Copyright 2017 by Emotiv. All rights reserved
** Test cases for Extra Prime Examples
** Require activated license
****************************************************************************/
#define _CRT_SECURE_NO_WARNINGS
#define _WIN32_WINNT 0x0501
#define BOOST_TEST_MODULE ACTIVE_EEG_ONLY
#define BOOST_TEST_NO_MAIN
#include <boost/test/auto_unit_test.hpp>
#include <iostream>
#include <ctime>
#include <cassert>
#include <chrono>
#include <thread>


#include "Iedk.h"
#include "IedkErrorCode.h"
#include "IEmoStateDLL.h"
#include "IEegData.h"
#include "EmotivCloudClient.h"
#include "IEmoStatePerformanceMetric.h"

IEE_DataChannel_t EpocChannelList[] = {
    IED_COUNTER,
    IED_INTERPOLATED,
    IED_AF3,
    IED_F7,
    IED_F3,
    IED_FC5,
    IED_T7,
    IED_P7,
    IED_O1,
    IED_O2,
    IED_P8,
    IED_T8,
    IED_FC6,
    IED_F4,
    IED_F8,
    IED_AF4,
    IED_RAW_CQ,
    IED_GYROX,
    IED_GYROY,
    IED_MARKER,
    IED_TIMESTAMP
};
IEE_DataChannel_t InsightChannelList[] = {
    IED_COUNTER,
    IED_INTERPOLATED,
    IED_RAW_CQ,
    IED_AF3,
    IED_T7,
    IED_Pz,
    IED_T8,
    IED_AF4,
    IED_TIMESTAMP,
    IED_MARKER,
    IED_SYNC_SIGNAL
};

void CaculateScale(double& rawScore, double& maxScale, double& minScale, double& scaledScore) {
	if(rawScore < minScale) {
		scaledScore = 0;
	} else if(rawScore > maxScale) {
		scaledScore = 1;
	} else {
		scaledScore = (rawScore - minScale) / (maxScale - minScale);
	}
}

void logPerformanceMetricScore(unsigned int userID, EmoStateHandle eState) {
	// Create the top header

	// Log the time stamp and user ID
	std::cout << IS_GetTimeFromStart(eState) << ",";
	std::cout << userID << ",";

	// PerformanceMetric results
	double rawScore = 0;
	double minScale = 0;
	double maxScale = 0;
	double scaledScore = 0;
	IS_PerformanceMetricGetStressModelParams(eState, &rawScore, &minScale, &maxScale);
	std::cout << rawScore << ",";
	std::cout << minScale << ",";
	std::cout << maxScale << ",";

	if(minScale == maxScale) {
		std::cout << "undefined" << ",";
	} else {
		CaculateScale(rawScore, maxScale, minScale, scaledScore);
		std::cout << scaledScore << ",";
	}

	IS_PerformanceMetricGetEngagementBoredomModelParams(eState, &rawScore,
		&minScale, &maxScale);
	std::cout << rawScore << ",";
	std::cout << minScale << ",";
	std::cout << maxScale << ",";

	if(minScale == maxScale) {
		std::cout << "undefined" << ",";
	} else {
		CaculateScale(rawScore, maxScale, minScale, scaledScore);
		std::cout << scaledScore << ",";
	}

	IS_PerformanceMetricGetRelaxationModelParams(eState, &rawScore,
		&minScale, &maxScale);
	std::cout << rawScore << ",";
	std::cout << minScale << ",";
	std::cout << maxScale << ",";

	if(minScale == maxScale) {
		std::cout << "undefined" << ",";
	} else {
		CaculateScale(rawScore, maxScale, minScale, scaledScore);
		std::cout << scaledScore << ",";
	}

	IS_PerformanceMetricGetInstantaneousExcitementModelParams(eState,
		&rawScore, &minScale, &maxScale);
	std::cout << rawScore << ",";
	std::cout << minScale << ",";
	std::cout << maxScale << ",";

	if(minScale == maxScale) {
		std::cout << "undefined" << ",";
	} else {
		CaculateScale(rawScore, maxScale, minScale, scaledScore);
		std::cout << scaledScore << ",";
	}

	IS_PerformanceMetricGetInterestModelParams(eState, &rawScore,
		&minScale, &maxScale);
	std::cout << rawScore << ",";
	std::cout << minScale << ",";
	std::cout << maxScale << ",";

	if(minScale == maxScale) {
		std::cout << "undefined" << ",";
	} else {
		CaculateScale(rawScore, maxScale, minScale, scaledScore);
		std::cout << scaledScore << ",";
	}

	std::cout << std::endl;
}



inline void getEEGData(EmoEngineEventHandle eEvent, DataHandle hData) {
	int result = IEE_DataUpdateHandle(0, hData);

	unsigned int nSamplesTaken = 0;
	IEE_DataGetNumberOfSample(hData, &nSamplesTaken);
	if(nSamplesTaken != 0) {
		double* data = new double[nSamplesTaken];

		for(int sampleIdx = 0; sampleIdx < (int) nSamplesTaken; ++sampleIdx) {
			for(int i = 0; i < sizeof(EpocChannelList) / sizeof(IEE_DataChannel_t); i++) {
				IEE_DataGet(hData, EpocChannelList[i], data, nSamplesTaken);
				std::cout << data[sampleIdx] << ",";
			}
			std::cout << std::endl;
		}
		delete[] data;
	}
	std::this_thread::sleep_for(std::chrono::milliseconds(10));
}

inline void getPMData(EmoEngineEventHandle eEvent, EmoStateHandle sState, int userID) {
	IEE_EmoEngineEventGetEmoState(eEvent, sState);
	logPerformanceMetricScore(userID, sState);
}

int emotivEngine(std::string primeType) {
	// Get license info
	IEE_LicenseInfos_t licenseInfos;
	IEE_LicenseInformation(&licenseInfos);
	int sessionNumbBefore = licenseInfos.usedQuota;


	// Call prime functions
	float secs{1};
	DataHandle hData = IEE_DataCreate();
	IEE_DataSetBufferSizeInSec(secs);
	float headsetTime = 0;
	int userAdded{-1};
	EmoStateHandle eState = IEE_EmoStateCreate();
	EmoEngineEventHandle eEvent = IEE_EmoEngineEventCreate();
	int state;
	unsigned int userID = 0;
	while(1) {
		state = IEE_EngineGetNextEvent(eEvent);
		if(state == EDK_OK) {
			IEE_Event_t eventType = IEE_EmoEngineEventGetType(eEvent);
			IEE_EmoEngineEventGetUserId(eEvent, &userID);
			if(eventType == IEE_UserAdded && userAdded != 0) {
				// This function would active session counting
				userAdded = IEE_DataAcquisitionEnable(userID, true);
				std::this_thread::sleep_for(std::chrono::seconds(10));
			}
			if(userAdded == 0 && eventType == IEE_EmoStateUpdated) {
				headsetTime = IS_GetTimeFromStart(eEvent);
				
				if(primeType == "EEG")
					getEEGData(eEvent, hData);
				else
					getPMData(eEvent, eState, userID);
				// Connect for 70 seconds to take at lease 1 session
				if(headsetTime > 70) {
					break;
				}
			}
		}
	}
	IEE_EmoStateFree(eState);
	IEE_EmoEngineEventFree(eEvent);
	// Check remaining quota
	IEE_LicenseInformation(&licenseInfos);
	int sessionNumbAfter = licenseInfos.usedQuota;
	std::cout << "Quota before used " << sessionNumbBefore << " - after: " << sessionNumbAfter << '\n';
	if(sessionNumbAfter > sessionNumbBefore)
		return 0;
	return 1;
}

const char header[] = "IED_COUNTER, IED_INTERPOLATED, IED_AF3, IED_F7, IED_F3, IED_FC5, IED_T7, IED_P7, IED_O1, IED_O2, IED_P8, IED_T8, IED_FC6, IED_F4, IED_F8, IED_AF4, IED_RAW_CQ, IED_GYROX, IED_GYROY, IED_MARKER, IED_TIMESTAMP ";
int userCloudID {0};

struct Fixture {
    //run before every test case start
    Fixture() {
		IEE_EngineConnect();
	}

    //run after every test case finish
    ~Fixture() {
		IEE_EngineDisconnect();
	}
};

BOOST_FIXTURE_TEST_SUITE (ACTIVE_EEG_ONLY, Fixture)


// Login to Emotiv Cloud
//BOOST_FIXTURE_TEST_CASE(TC01_GIVEN_internet_available_WHEN_user_login_THEN_login_successfuly, Fixture) {
//    std::string const userName {"toannd"};
//    std::string const password {"3Motiv8ed"};
//    BOOST_CHECK_MESSAGE(IEE_EngineConnect() == EDK_OK, "ERROR: Engine Connect failed");
//    BOOST_CHECK_MESSAGE(EC_Connect() == EDK_OK, "ERROR: EC_Connect failed");
//    BOOST_CHECK_MESSAGE(EC_Login(userName.c_str(), password.c_str()) == EDK_OK, "EC_Login failed");
//    BOOST_CHECK_MESSAGE(EC_GetUserDetail(&userCloudID) == EDK_OK, "EC_GetUserDetail failed");
//    assert(userCloudID != 0); // Stop test if userCloudID is invalid
//}

//BOOST_FIXTURE_TEST_CASE(TC02_GIVEN_invalid_license_WHEN_activate_THEN_fail, Fixture) {
//    // TODO: Insert invalid license here
//    std::string const EEGLicense {"088413c1-1d87-4f59-81a6-9fbf8a2069123"};
//    assert(EEGLicense != ""); // Stop test if EEGLicense is empty
//    //BOOST_CHECK_MESSAGE(IEE_ActivateLicense(EEGLicense.c_str(), 0) == EDK_LICENSE_ERROR, "IEE_ActivateLicense failed");
//    BOOST_CHECK_MESSAGE(IEE_ActivateLicense(EEGLicense.c_str(), 0) != EDK_OK, "IEE_ActivateLicense failed");
//}

//BOOST_FIXTURE_TEST_CASE(TC03_GIVEN_no_license_activated_WHEN_call_EEG_function_THEN_user_fail_to_get_data, Fixture) {
//	
//}
//
//BOOST_FIXTURE_TEST_CASE(TC04_GIVEN_no_license_activated_WHEN_call_PM_function_THEN_data_update_per_10_seconds, Fixture) {}

BOOST_FIXTURE_TEST_CASE(TC05_GIVEN_no_license_WHEN_not_activated_THEN_data_update_per_10_seconds, Fixture) {
    // TODO: Insert valid EEG only license here
 //   std::string EEGLicense {"6bbee6b4-b1ba-4ac4-8f27-bc49dcf8cfc1"};
 //   assert(EEGLicense != ""); // Stop test if EEGLicense is empty
	//int errorCode = IEE_ActivateLicense(EEGLicense.c_str(), 1);
 //   BOOST_CHECK_MESSAGE(errorCode == EDK_OK, "IEE_ActivateLicense failed 0x" << std::hex << errorCode);

	// Test one session
	int res = emotivEngine("PM");
	BOOST_CHECK_MESSAGE(res == 0, "emotivEngine");
}

//BOOST_FIXTURE_TEST_CASE(TC06_GIVEN_activated_EEG_only_license_WHEN_call_PM_function_THEN_data_update_per_10_seconds, Fixture) {}
//
//BOOST_FIXTURE_TEST_CASE(TC07_GIVEN_activated_EEG_only_license_WHEN_call_EEG_function_THEN_data_update_per_0.5_seconds, Fixture) {}
//
//BOOST_FIXTURE_TEST_CASE(TC08_GIVEN_activated_EEG_PM_license_WHEN_call_PM_function_THEN_data_update_per_0.5_seconds, Fixture) {}
//
//BOOST_FIXTURE_TEST_CASE(TC09_GIVEN_activated_EEG_only_license_WHEN_call_EEG_function_THEN_data_update_per_0.5_seconds, Fixture) {}

BOOST_AUTO_TEST_SUITE_END()