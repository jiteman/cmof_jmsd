// gtest_xml_outfile2_test_ writes some xml via TestProperty used by
// gtest_xml_outfiles_test.py

#include "gtest/gtest.h"

class PropertyTwo : public ::jmsd::cutf::Test {
 protected:
  void SetUp() override { RecordProperty("SetUpProp", 2); }
  void TearDown() override { RecordProperty("TearDownProp", 2); }
};

TEST_F(PropertyTwo, TestSomeProperties) {
  RecordProperty("TestSomeProperty", 2);
}
