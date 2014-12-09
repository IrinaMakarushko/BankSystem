#pragma once

#ifdef EXPORT_TEST_FUNCTIONS

#define MY_CPP_UNITTESTAPP_EXPORT __declspec(dllexport)
#else
#define MY_CPP_UNITTESTAPP_EXPORT
#endif

MY_CPP_UNITTESTAPP_EXPORT int addMoney(int & balance1, int totalTransaction1, int currentBalance1, int & currentTransaction1, int monthlyQuota1);
MY_CPP_UNITTESTAPP_EXPORT int withdrawMoney(int & balance1, int totalTransaction1, int currentBalance1, int & currentTransaction1, int monthlyQuota1);
