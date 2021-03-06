#pragma once
// Google C++ Testing and Mocking Framework definitions useful in production code.

// When you need to test the private or protected members of a class,
// use the FRIEND_TEST macro to declare your tests as friends of the
// class.  For example:
//
// class MyClass {
//  private:
//   void PrivateMethod();
//   FRIEND_TEST(MyClassTest, PrivateMethodWorks);
// };
//
// class MyClassTest : public testing::Test {
//   // ...
// };
//
// TEST_F(MyClassTest, PrivateMethodWorks) {
//   // Can call MyClass::PrivateMethod() here.
// }
//
// Note: The test class must be in the same namespace as the class being tested.
// For example, putting MyClassTest in an anonymous namespace will not work.

#define FRIEND_TEST(test_case_name, test_name) \
	friend class test_case_name##_##test_name##_Test
