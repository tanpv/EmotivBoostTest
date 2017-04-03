//--------------------------------------------------------------
/*Checklist:
- Login with correct EmotivID
- Download valid profile
- Download invalid profile
- Upload valid profile
- Upload invalid profile
*/
#define _WIN32_WINNT 0x0501
#define BOOST_TEST_MODULE EMOTIV_CLOUD_PROFILE
#define BOOST_TEST_NO_MAIN

#include <boost/test/auto_unit_test.hpp>
#include <boost/asio.hpp>
#include <boost/date_time/posix_time/posix_time.hpp>
#include <boost/thread/thread.hpp>

#include <iostream>
#include <conio.h>
#include <atomic>
#include <thread>
#include <fstream>
#include <stdio.h>

#include "Iedk.h"
#include "IedkErrorCode.h"
#include "EmotivLicense.h"
#include "EmotivCloudClient.h"

#ifdef _WIN32
	#include <conio.h>
	#include <windows.h>
#endif

#if __linux__ || __APPLE__
	#include <unistd.h>
#endif

//--------------------------------------------------------------
namespace utf = boost::unit_test;

int engineUserID = -1;
int userCloudID = -1;
int profileID = -1;

std::string const profileName = "test1";
std::string const localProfile = "./training_profile/upload/test1.emu";
std::string const cloudProfile = "./training_profile/download/test1.emu";

// username/pw for sdk 3.5 dev server
std::string const userName = "toannd";
std::string const password = "3Motiv8ed";

// username/pw for sdk Community product server 
//std::string userName = "min";
//std::string password = "Zxcvbnm1";

/************** FUNCTIONS *******************/

void listCloudProfile(int userCloudID) {
	int getNumberProfile = EC_GetAllProfileName(userCloudID);
	std::cout << "Number of profiles: " << getNumberProfile << "\n";

	// Print out all existing training profile on Emotiv Cloud
	for (int i = 0; i < getNumberProfile; i++) {
		std::cout << "Profile Name: " << EC_ProfileNameAtIndex(userCloudID, i) << ", ";
		std::cout << "Profile ID: " << EC_ProfileIDAtIndex(userCloudID, i) << ", ";
		std::cout << "Profile type: " << ((EC_ProfileTypeAtIndex(userCloudID, i) == profileFileType::TRAINING) ? "TRAINING" : "EMOKEY") << ", ";
		std::cout << EC_ProfileLastModifiedAtIndex(userCloudID, i) << ",\r\n";
	}
}

bool verifyProfile(const char * downloadFile, const char * uploadFile, long long * size) {
	std::ifstream downloadFS(downloadFile, std::ifstream::ate | std::ifstream::binary);
	long long downloadSize, uploadSize;
	downloadSize = downloadFS.is_open() ? downloadFS.tellg() : -1;
	downloadFS.close();

	std::ifstream uploadFS(downloadFile, std::ifstream::ate | std::ifstream::binary);
	uploadSize = uploadFS.is_open() ? uploadFS.tellg() : -1;
	uploadFS.close();

	*size = (downloadSize == uploadSize) ? downloadSize : -1;

	if (*size == -1)
		return false;
	return downloadSize == uploadSize;
}

/********************************************/

BOOST_AUTO_TEST_SUITE(EMOTIV_CLOUD_PROFILE)

BOOST_AUTO_TEST_CASE(PRO-265_GIVEN_correct_EmotivID_WHEN_access_to_Emotiv_cloud_THEN_user_fail_to_login) {
	int errorCode;
	BOOST_CHECK_MESSAGE(IEE_EngineConnect() == EDK_OK, "ERROR: IEE_EngineConnect failed");
	errorCode = EC_Connect();
	BOOST_CHECK_MESSAGE(EC_Connect() == EDK_OK, "ERROR: EC_Connect failed: " << std::hex << errorCode); // Require dongle plugged-in
	errorCode = EC_Login(userName.c_str(), password.c_str());
	BOOST_CHECK_MESSAGE(EC_Login(userName.c_str(), password.c_str()) == EDK_OK, "EC_Login failed " << std::hex << errorCode);
	BOOST_CHECK_MESSAGE(EC_GetUserDetail(&userCloudID) == EDK_OK, "EC_GetUserDetail failed");

	//listCloudProfile(userCloudID);
}

BOOST_AUTO_TEST_CASE(PRO-265_GIVEN_local_valid_profile_WHEN_accessed_to_Emotiv_cloud_THEN_user_able_to_upload) {
	int errorCode;
	errorCode = EC_UploadProfileFile(userCloudID, profileName.c_str(), localProfile.c_str(), TRAINING, true);
	BOOST_CHECK_MESSAGE(errorCode == EDK_OK, "EC_UploadProfileFile failed");

	//listCloudProfile(userCloudID);
}


BOOST_AUTO_TEST_CASE(PRO-265_GIVEN_invalid_profile_WHEN_accessed_to_Emotiv_cloud_THEN_user_fail_to_download) {
	std::string cloudProfile = "_invalid_profile_";
	BOOST_CHECK_MESSAGE(EC_GetProfileId(userCloudID, cloudProfile.c_str(), &profileID) != EDK_OK, "EC_GetProfileId error");
	BOOST_CHECK_MESSAGE(EC_DownloadProfileFile(userCloudID, profileID, localProfile.c_str()) != EDK_OK, "EC_DownloadProfileFile error");
}

BOOST_AUTO_TEST_CASE(PRO-265_GIVEN_valid_profile_WHEN_accessed_to_Emotiv_cloud_THEN_user_able_to_download) {
	int errorCode;
	std::string cloudProfileName = "test1";
	errorCode = EC_GetProfileId(userCloudID, cloudProfileName.c_str(), &profileID);
	BOOST_CHECK_MESSAGE(errorCode == EDK_OK, "EC_GetProfileId failed: " << std::hex << errorCode);
	errorCode = EC_DownloadProfileFile(userCloudID, profileID, cloudProfile.c_str());
	BOOST_CHECK_MESSAGE(errorCode == EDK_OK, "EC_DownloadProfileFile failed: " << errorCode);

	// Remove profile in download folder
	/*long long fileSize = 0;
	bool valid = verifyProfile(cloudProfile.c_str(), localProfile.c_str(), &fileSize);
	BOOST_CHECK_MESSAGE(valid == true, "File's not match");
	if (valid == true) {
		if (remove(cloudProfile.c_str()) != 0)
			std::cout << "Cannot delete downloaded profile " << cloudProfile << std::endl;
	}*/
}

BOOST_AUTO_TEST_SUITE_END()
