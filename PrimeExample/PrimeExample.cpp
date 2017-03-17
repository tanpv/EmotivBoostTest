/****************************************************************************
** Copyright 2017 by Emotiv. All rights reserved
** Test cases for Extra Prime Examples
** Require activated license
****************************************************************************/

#define _CRT_SECURE_NO_WARNINGS
#define _WIN32_WINNT 0x0501
#define BOOST_TEST_MODULE ACTIVE_LICENSE
#define BOOST_TEST_NO_MAIN
#include <boost/test/auto_unit_test.hpp>
#include <iostream>
#include <ctime>
#include <cassert>

#include "Iedk.h"
#include "IedkErrorCode.h"
#include "IEmoStateDLL.h"
#include "Iedk.h"
#include "EmotivCloudClient.h"

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

const char header[] = "IED_COUNTER, IED_INTERPOLATED, IED_AF3, IED_F7, IED_F3, IED_FC5, IED_T7, IED_P7, IED_O1, IED_O2, IED_P8, IED_T8, IED_FC6, IED_F4, IED_F8, IED_AF4, IED_RAW_CQ, IED_GYROX, IED_GYROY, IED_MARKER, IED_TIMESTAMP ";
int userCloudID {0};



struct Fixture {
    //run before every test case start
    Fixture() {

    }

    //run after every test case finish
    ~Fixture() {
    }
};

BOOST_FIXTURE_TEST_SUITE(ACTIVE_EEG_ONLY, Fixture)

BOOST_FIXTURE_TEST_CASE(TC01_GIVEN_internet_available_WHEN_user_login_THEN_login_successfuly, Fixture) {
    std::string const userName {"min"};
    std::string const password {"Zxcvbnm1"};
    BOOST_CHECK_MESSAGE(IEE_EngineConnect() == EDK_OK, "ERROR: Engine Connect failed");
    BOOST_CHECK_MESSAGE(EC_Connect() == EDK_OK, "ERROR: EC_Connect failed");
    BOOST_CHECK_MESSAGE(EC_Login(userName.c_str(), password.c_str()) == EDK_OK, "EC_Login failed");
    BOOST_CHECK_MESSAGE(EC_GetUserDetail(&userCloudID) == EDK_OK, "EC_GetUserDetail failed");
    assert(userCloudID != 0); // Stop test if userCloudID is invalid
}

BOOST_FIXTURE_TEST_CASE(TC02_GIVEN_invalid_license_WHEN_activate_THEN_fail, Fixture) {
    // TODO: Insert invalid license here
    std::string const EEGLicense {""};
    assert(EEGLicense != ""); // Stop test if EEGLicense is empty
    BOOST_CHECK_MESSAGE(IEE_ActivateLicense(EEGLicense.c_str(), 0) == EDK_LICENSE_ERROR, "IEE_ActivateLicense failed");
    BOOST_CHECK_MESSAGE(IEE_ActivateLicense(EEGLicense.c_str(), 0) != EDK_OK, "IEE_ActivateLicense failed");
}

BOOST_FIXTURE_TEST_CASE(TC03_GIVEN_no_license_activated_WHEN_call_EEG_function_THEN_data_update_per_10_seconds, Fixture) {

}

BOOST_FIXTURE_TEST_CASE(TC04_GIVEN_no_license_activated_WHEN_call_PM_function_THEN_data_update_per_10_seconds, Fixture) {

}

BOOST_FIXTURE_TEST_CASE(TC05_GIVEN_valid_EEG_license_WHEN_activate_THEN_successful, Fixture) {
    // TODO: Insert valid EEG only license here
    std::string const EEGLicense {""};
    assert(EEGLicense != ""); // Stop test if EEGLicense is empty
    BOOST_CHECK_MESSAGE(IEE_ActivateLicense(EEGLicense.c_str(), 0) == EDK_OK, "IEE_ActivateLicense failed");
}

BOOST_FIXTURE_TEST_CASE(TC06_GIVEN_activated_EEG_only_license_WHEN_call_PM_function_THEN_data_update_per_10_seconds, Fixture) {
    
}

BOOST_FIXTURE_TEST_CASE(TC07_GIVEN_activated_EEG_only_license_WHEN_call_EEG_function_THEN_data_update_per_2_seconds, Fixture) {

}

BOOST_FIXTURE_TEST_CASE(TC08_GIVEN_activated_EEG_PM_license_WHEN_call_PM_function_THEN_data_update_per_2_seconds, Fixture) {

}

BOOST_FIXTURE_TEST_CASE(TC09_GIVEN_activated_EEG_only_license_WHEN_call_EEG_function_THEN_data_update_per_2_seconds, Fixture) {

}

BOOST_AUTO_TEST_SUITE_END()