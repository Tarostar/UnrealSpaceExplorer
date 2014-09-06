#include "stdafx.h"
#include "CppUnitTest.h"
//#include "../Source/SpaceExplorer/Public/InventoryObject.h"

using namespace Microsoft::VisualStudio::CppUnitTestFramework;

namespace SpaceExplorerTest
{		
	TEST_CLASS(UnitTest1)
	{
	public:
		
		TEST_METHOD(TestMethod1)
		{
			//AInventoryObject inv(NULL);

			Assert::AreEqual("Hello", "Hello");

			Assert::AreEqual(3, 3);
		}

	};
}