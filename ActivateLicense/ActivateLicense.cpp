#define _CRT_SECURE_NO_WARNINGS
#define _WIN32_WINNT 0x0501
/****************************************************************************
**
** Copyright 2015 by Emotiv. All rights reserved
** Example - ActivateLicense
** How to activate a license key and Get infomation of that license
**
****************************************************************************/

#define BOOST_TEST_MODULE ACTIVE_LICENSE
#define BOOST_TEST_NO_MAIN
#include <boost/test/auto_unit_test.hpp>
#include <boost/asio.hpp>
#include <boost/date_time/posix_time/posix_time.hpp>
#include <boost/thread/thread.hpp>

#include <iostream>
#include <conio.h>
#include <atomic>
#include <thread>

#include "Iedk.h"
#include "IedkErrorCode.h"
#include "EmotivLicense.h"
#include <EmotivCloudClient.h>

/**
* License keys:
*/

// EEG, PM 2 seat
std::string const validKey1 = "088413c1-1d87-4f59-81a6-9fbf8a206911";
std::string const validKey2 = "51e4c3ea-d8b4-4c61-b15c-8b1efb92bceb";
std::string const validKey3 = "57584465-3e44-454e-ac37-bfa014deb043";

namespace utf = boost::unit_test;

int errorCode;
int withdrawSession;

std::string const userName = "toannd";
std::string const password = "3Motiv8ed";
int userCloudID = -1;

std::string convertEpochToTime(time_t epochTime, std::string format) {
	if(format == "") {
		format = "%Y-%m-%d %H:%M:%S";
	}

	char timestamp[64] = { 0 };
	strftime(timestamp, sizeof(timestamp), format.c_str(), localtime(&epochTime));
	return timestamp;
}

IEE_LicenseType_t licenseType;

void printLicenseInformation(IEE_LicenseInfos_t& licenseInfos) {
	std::cout << std::endl;
	std::cout << "Date From  : " << convertEpochToTime(licenseInfos.date_from, "%Y-%m-%d %H:%M:%S") << std::endl;
	std::cout << "Date To    : " << convertEpochToTime(licenseInfos.date_to, "%Y-%m-%d %H:%M:%S") << std::endl;
	std::cout << std::endl;

	std::cout << "Seat number: " << licenseInfos.seat_count << std::endl;
	std::cout << std::endl;

	std::cout << "Total Quota: " << licenseInfos.quota << std::endl;
	std::cout << "Total quota used    : " << licenseInfos.usedQuota << std::endl;
	std::cout << std::endl;

	switch(licenseInfos.scopes) {
	case IEE_EEG:
		licenseType = IEE_LicenseType_t::IEE_EEG;

		std::cout << "License type : " << "EEG" << std::endl;
		std::cout << std::endl;
		break;

	case IEE_EEG_PM:
		licenseType = IEE_LicenseType_t::IEE_EEG_PM;

		std::cout << "License type : " << "EEG + PM" << std::endl;
		std::cout << std::endl;
		break;

	case IEE_PM:
		licenseType = IEE_LicenseType_t::IEE_PM;
		std::cout << "License type : " << "PM" << std::endl;
		std::cout << std::endl;
		break;

	default:
		std::cout << "License type : " << "No type" << std::endl;
		std::cout << std::endl;
		break;
	}

	std::cout << std::endl;
}

// Fixture define, leave blank for future implements
struct Fixture {
	//run before every test case start
	Fixture() {
		BOOST_CHECK_MESSAGE(IEE_EngineConnect() == EDK_OK, "ERROR: Engine Connect failed");
		BOOST_CHECK_MESSAGE(EC_Connect() == EDK_OK, "ERROR: EC_Connect failed");
		BOOST_CHECK_MESSAGE(EC_Login(userName.c_str(), password.c_str()) == EDK_OK, "EC_Login failed");
		EC_GetUserDetail(&userCloudID);

		std::cout << "FIXTURE CREATE - User Cloud ID = " << userCloudID << "\n";
		//Sleep(10000);
	}

	//run after every test case finish
	~Fixture() {
		BOOST_CHECK_MESSAGE(EC_Logout(userCloudID) == EDK_OK, "EC_Logout failed");
		BOOST_CHECK_MESSAGE(EC_Disconnect() == EDK_OK, "ERROR: EC_Disconnect failed");
		BOOST_CHECK_MESSAGE(IEE_EngineDisconnect() == EDK_OK, "ERROR: Engine Disconnect failed");

		std::cout << "FIXTURE DESTROY - User Cloud ID = " << userCloudID << "\n";
	}
};

BOOST_FIXTURE_TEST_SUITE(ACTIVE_LICENSE, Fixture)

// REQUIRE CASE
//BOOST_FIXTURE_TEST_CASE(TC0_CONNECT_TO_EMOTIV, Fixture) {
//  std::string const userName = "toannd";
//  std::string const password = "3Motiv8ed";
//
//  // Connect headset - required
//  BOOST_CHECK_MESSAGE(IEE_EngineConnect() == EDK_OK, "ERROR: Engine Connect failed");
//  // Connect Emotiv Cloud
//  BOOST_CHECK_MESSAGE(EC_Connect() == EDK_OK, "ERROR: EC_Connect failed");
//  // Login to Emotiv Cloud
//  BOOST_CHECK_MESSAGE(EC_Login(userName.c_str(), password.c_str()) == EDK_OK, "EC_Login failed");
//}

//BOOST_FIXTURE_TEST_CASE(TC1_GIVEN_a_valid_license_WHEN_input_invalid_grant_number_THEN_user_could_not_activate_license, Fixture) {
//  // Feed grant number from -100 to -1 to IEE_ActivateLicense()
//  for (auto i = -5; i < 0; i++) {
//      errorCode = IEE_ActivateLicense(validKeyEP2.c_str(), i);
//      BOOST_CHECK_MESSAGE(errorCode != EDK_OK, "ERROR: License activated with grant number = " << i << " - Error Code = " << std::hex << errorCode);
//  }
//}
//
//BOOST_FIXTURE_TEST_CASE(TC2_GIVEN_a_valid_license_WHEN_input_0_as_grant_number_THEN_user_could_activate_license_successfully, Fixture) {
//
//  errorCode = IEE_ActivateLicense(validKeyEP2.c_str(), 0);
//  BOOST_CHECK_MESSAGE(errorCode == EDK_OK, "ERROR: Unable to withdraw " << 0 << " session - Error Code = " << std::hex << errorCode);
//
//  IEE_LicenseInformation(&licenseInfos);
//  auto const cloudQuota = licenseInfos.remainingSession;
//  BOOST_TEST_MESSAGE("sessionRemaining = " << std::dec << cloudQuota);
//}

//This test case cannot verify total session got from cloud due to API's defect
BOOST_FIXTURE_TEST_CASE(TC3_GIVEN_a_valid_license_WHEN_input_1_as_grant_number_THEN_user_could_activate_license_successfully, Fixture) {
	// Grant 0 session just for activate license to refresh cloud quota
	//auto errorCode = IEE_ActivateLicense(validKeyEP2.c_str(), 0);
	//BOOST_CHECK_MESSAGE(errorCode == EDK_OK, "ERROR: License activated with grant number = " << 0 << " - Error Code = " << std::hex << errorCode);

	// Get infomation from activated license
	//errorCode = IEE_LicenseInformation(&licenseInfos);
	//auto const quotaBeforeWithdraw = licenseInfos.quota;
	//BOOST_TEST_MESSAGE("PC Quota = " << quotaBeforeWithdraw);
	//BOOST_CHECK_MESSAGE(errorCode == EDK_OK, "ERROR: Unable to get information from license - Error Code = " << std::hex << errorCode);
	//auto const withdrawSession = 1;
	// Withraw 1 session from cloud
	withdrawSession = 5;
	BOOST_TEST_MESSAGE("Getting " << withdrawSession << " 1 session from Cloud");

	errorCode = IEE_ActivateLicense(validKey3.c_str(), withdrawSession);
	BOOST_CHECK_MESSAGE(errorCode == EDK_OK, "ERROR: Unable to withraw session from cloud - Error Code = " << std::hex << errorCode);

	IEE_LicenseInfos_t licenseInfos;
	// Check quota after withdaw
	std::cout << validKey2 << std::endl;
	IEE_LicenseInformation(&licenseInfos);
	printLicenseInformation(licenseInfos);
	//auto const newQuota = licenseInfos.quota;
	//BOOST_CHECK_MESSAGE(newQuota == (quotaBeforeWithdraw + withdrawSession), "Quota number error mismatch");
}

// This test case cannot verify total session got from cloud due to API's defect
// BOOST_FIXTURE_TEST_CASE(TC4_GIVEN_a_valid_license_WHEN_input_10_as_grant_number_THEN_user_could_activate_license, Fixture) {
// //    // Grant 0 session just for activate license
// //    //auto errorCode = IEE_ActivateLicense(validKeyEP2.c_str(), 0);
// //    //BOOST_CHECK_MESSAGE(errorCode == EDK_OK, "ERROR: License activated with grant number = " << 0 << " - Error Code = " << std::hex << errorCode);
// //    //Sleep(10000);
// //
// //    // Get infomation from activated license
// //    errorCode = IEE_LicenseInformation(&licenseInfos);
// //    auto quotaBeforeWithdraw = licenseInfos.quota;
// //    BOOST_CHECK_MESSAGE(errorCode == EDK_OK, "ERROR: Unable to get information from license - Error Code = " << std::hex << errorCode);
// //
// //
// //    auto withdrawSession = 10;
// //    // Withraw 10 session from cloud
// //    errorCode = IEE_ActivateLicense(validKeyEP2.c_str(), withdrawSession);
// //    auto cloudQuota = licenseInfos.sessionRemaining;
// //
// //
// //    if (withdrawSession <= cloudQuota) {
// //        BOOST_CHECK_MESSAGE(errorCode == EDK_OK, "ERROR: Unable to withraw session from cloud - Error Code = " << std::hex << errorCode);
// //        // Check quota after withdaw
// //        IEE_LicenseInformation(&licenseInfos);
// //        auto newQuota = licenseInfos.quota;
// //        BOOST_CHECK_MESSAGE(newQuota == (quotaBeforeWithdraw + withdrawSession), "Quota number error mismatch");
// //    }
// //    else {
// //        BOOST_CHECK_MESSAGE(errorCode != EDK_OK, "ERROR: Able to withdraw invalid quota - Error Code = " << std::hex << errorCode);
// //    }
//  BOOST_TEST_MESSAGE("Getting 1 session from Cloud");
//  withdrawSession = 1;
//  errorCode = IEE_ActivateLicense(validKey1.c_str(), withdrawSession);

//  BOOST_CHECK_MESSAGE(errorCode == EDK_OK, "ERROR: Unable to withraw " << withdrawSession << " session from cloud - Error Code = " << std::hex << errorCode);
// }
//
//BOOST_FIXTURE_TEST_CASE(TC5_GIVEN_a_valid_license_WHEN_input_grant_number_more_than_cloud_remain_THEN_user_could_not_activate_license, Fixture) {
//  // Grant 0 session just for activate license
//  //auto errorCode = IEE_ActivateLicense(validKeyEP2.c_str(), 0);
//  //BOOST_CHECK_MESSAGE(errorCode == EDK_OK, "ERROR: License activated with grant number = " << 0 << " - Error Code = " << std::hex << errorCode);
//  Sleep(10000);
//
//  // Get infomation from activated license
//  IEE_LicenseInfos_t licenseInfos;
//  errorCode = IEE_LicenseInformation(&licenseInfos);
//  BOOST_CHECK_MESSAGE(errorCode == EDK_OK, "ERROR: Unable to get information from license - Error Code = " << std::hex << errorCode);
//
//  auto cloudQuota = licenseInfos.remainingSession;
//  //for (auto i = 0; i < 5; i++) {
//  //  errorCode = IEE_ActivateLicense(validKeyEP2.c_str(), cloudQuota + i);
//  //  BOOST_CHECK_MESSAGE(errorCode != EDK_OK, "ERROR: Able to withraw more available session");
//  //}
//  withdrawSession = cloudQuota + 1;
//  errorCode = IEE_ActivateLicense(validKey1.c_str(), withdrawSession);
//  BOOST_CHECK_MESSAGE(errorCode != EDK_OK, "ERROR: Able to withraw more available session from CLOUD");
//}

BOOST_AUTO_TEST_SUITE_END()