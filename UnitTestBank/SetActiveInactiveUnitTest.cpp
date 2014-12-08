#include "stdafx.h"
#include "CppUnitTest.h"
#include <string>
#include "../BankSystem/Header.h"

#pragma comment(lib, "../Debug/BankSystem.lib")

using namespace Microsoft::VisualStudio::CppUnitTestFramework;

namespace UnitTestBank
{		
	TEST_CLASS(UnitTest1)
	{
	public:
		
		TEST_METHOD(TestMethodAddMoneyPassed)
		{
			int balance = 5;
			int currentTransaction = 3;
			Assert::AreEqual(0, addMoney(balance, 20, 500, currentTransaction, 3), L"Error", LINE_INFO());
		}

		TEST_METHOD(TestMethodAddMoneyFailed)
		{
			int balance = 5;
			int currentTransaction = 3;
			Assert::AreEqual(2, addMoney(balance, 20, 500, currentTransaction, 3), L"Error", LINE_INFO());
		}


	};
}