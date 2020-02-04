// gtest_xml_outfile1_test_ writes some xml via TestProperty used by
// gtest_xml_outfiles_test.py

#include "gtest/gtest.h"

class PropertyOne : public ::jmsd::cutf::Test {
 protected:
  void SetUp() override { RecordProperty("SetUpProp", 1); }
  void TearDown() override { RecordProperty("TearDownProp", 1); }
};

TEST_F(PropertyOne, TestSomeProperties) {
  RecordProperty("TestSomeProperty", 1);
}
