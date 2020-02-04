// Google Test UnitTestOptions tests
//
// This file tests classes and functions used internally by
// Google Test.  They are subject to change without notice.
//
// This file is #included from gtest.cc, to avoid changing build or
// make-files on Windows and other platforms. Do not #include this file
// anywhere else!

#include "gtest/gtest.h"


#include "gtest/Assertion_result.hin"
#include "gtest/Message.hin"

#include "gtest/internal/Unit_test_options.h"

#include "gtest/gtest-internal-inl.h"


#if GTEST_OS_WINDOWS_MOBILE
# include "internal/custom/temporary_windows_include.h"
#elif GTEST_OS_WINDOWS
# include <direct.h>
#endif  // GTEST_OS_WINDOWS_MOBILE


namespace testing {
namespace internal {
namespace {

// Turns the given relative path into an absolute path.
FilePath GetAbsolutePathOf(const FilePath& relative_path) {
  return FilePath::ConcatPaths(FilePath::GetCurrentDir(), relative_path);
}

// Testing UnitTestOptions::GetOutputFormat/GetOutputFile.

TEST(XmlOutputTest, GetOutputFormatDefault) {
  GTEST_FLAG(output) = "";
  EXPECT_STREQ("", ::jmsd::cutf::internal::UnitTestOptions::GetOutputFormat().c_str());
}

TEST(XmlOutputTest, GetOutputFormat) {
  GTEST_FLAG(output) = "xml:filename";
  EXPECT_STREQ("xml", ::jmsd::cutf::internal::UnitTestOptions::GetOutputFormat().c_str());
}

TEST(XmlOutputTest, GetOutputFileDefault) {
  GTEST_FLAG(output) = "";
  EXPECT_EQ(GetAbsolutePathOf(FilePath("test_detail.xml")).string(),
            ::jmsd::cutf::internal::UnitTestOptions::GetAbsolutePathToOutputFile());
}

TEST(XmlOutputTest, GetOutputFileSingleFile) {
  GTEST_FLAG(output) = "xml:filename.abc";
  EXPECT_EQ(GetAbsolutePathOf(FilePath("filename.abc")).string(),
            ::jmsd::cutf::internal::UnitTestOptions::GetAbsolutePathToOutputFile());
}

TEST(XmlOutputTest, GetOutputFileFromDirectoryPath) {
  GTEST_FLAG(output) = "xml:path" GTEST_PATH_SEP_;
  const std::string expected_output_file =
      GetAbsolutePathOf(
          FilePath(std::string("path") + GTEST_PATH_SEP_ +
                   GetCurrentExecutableName().string() + ".xml")).string();
  const std::string& output_file =
      ::jmsd::cutf::internal::UnitTestOptions::GetAbsolutePathToOutputFile();
#if GTEST_OS_WINDOWS
  EXPECT_STRCASEEQ(expected_output_file.c_str(), output_file.c_str());
#else
  EXPECT_EQ(expected_output_file, output_file.c_str());
#endif
}

TEST(OutputFileHelpersTest, DISABLED_GetCurrentExecutableName) {
  const std::string exe_str = GetCurrentExecutableName().string();
#if GTEST_OS_WINDOWS
  const bool success =
      _strcmpi("googletest-options-test", exe_str.c_str()) == 0 ||
      _strcmpi("gtest-options-ex_test", exe_str.c_str()) == 0 ||
      _strcmpi("gtest_all_test", exe_str.c_str()) == 0 ||
      _strcmpi("gtest_dll_test", exe_str.c_str()) == 0;
#elif GTEST_OS_OS2
  const bool success =
      strcasecmp("googletest-options-test", exe_str.c_str()) == 0 ||
      strcasecmp("gtest-options-ex_test", exe_str.c_str()) == 0 ||
      strcasecmp("gtest_all_test", exe_str.c_str()) == 0 ||
      strcasecmp("gtest_dll_test", exe_str.c_str()) == 0;
#elif GTEST_OS_FUCHSIA
  const bool success = exe_str == "app";
#else
  const bool success =
      exe_str == "googletest-options-test" ||
      exe_str == "gtest_all_test" ||
      exe_str == "lt-gtest_all_test" ||
      exe_str == "gtest_dll_test";
#endif  // GTEST_OS_WINDOWS
  if (!success)
    FAIL() << "GetCurrentExecutableName() returns " << exe_str;
}

#if !GTEST_OS_FUCHSIA

class XmlOutputChangeDirTest : public ::jmsd::cutf::Test {
 protected:
  void SetUp() override {
    original_working_dir_ = FilePath::GetCurrentDir();
    posix::ChDir("..");
    // This will make the test fail if run from the root directory.
    EXPECT_NE(original_working_dir_.string(),
              FilePath::GetCurrentDir().string());
  }

  void TearDown() override {
    posix::ChDir(original_working_dir_.string().c_str());
  }

  FilePath original_working_dir_;
};

TEST_F(XmlOutputChangeDirTest, PreserveOriginalWorkingDirWithDefault) {
  GTEST_FLAG(output) = "";
  EXPECT_EQ(FilePath::ConcatPaths(original_working_dir_,
                                  FilePath("test_detail.xml")).string(),
            ::jmsd::cutf::internal::UnitTestOptions::GetAbsolutePathToOutputFile());
}

TEST_F(XmlOutputChangeDirTest, PreserveOriginalWorkingDirWithDefaultXML) {
  GTEST_FLAG(output) = "xml";
  EXPECT_EQ(FilePath::ConcatPaths(original_working_dir_,
                                  FilePath("test_detail.xml")).string(),
            ::jmsd::cutf::internal::UnitTestOptions::GetAbsolutePathToOutputFile());
}

TEST_F(XmlOutputChangeDirTest, PreserveOriginalWorkingDirWithRelativeFile) {
  GTEST_FLAG(output) = "xml:filename.abc";
  EXPECT_EQ(FilePath::ConcatPaths(original_working_dir_,
                                  FilePath("filename.abc")).string(),
            ::jmsd::cutf::internal::UnitTestOptions::GetAbsolutePathToOutputFile());
}

TEST_F(XmlOutputChangeDirTest, PreserveOriginalWorkingDirWithRelativePath) {
  GTEST_FLAG(output) = "xml:path" GTEST_PATH_SEP_;
  const std::string expected_output_file =
      FilePath::ConcatPaths(
          original_working_dir_,
          FilePath(std::string("path") + GTEST_PATH_SEP_ +
                   GetCurrentExecutableName().string() + ".xml")).string();
  const std::string& output_file =
      ::jmsd::cutf::internal::UnitTestOptions::GetAbsolutePathToOutputFile();
#if GTEST_OS_WINDOWS
  EXPECT_STRCASEEQ(expected_output_file.c_str(), output_file.c_str());
#else
  EXPECT_EQ(expected_output_file, output_file.c_str());
#endif
}

TEST_F(XmlOutputChangeDirTest, PreserveOriginalWorkingDirWithAbsoluteFile) {
#if GTEST_OS_WINDOWS
  GTEST_FLAG(output) = "xml:c:\\tmp\\filename.abc";
  EXPECT_EQ(FilePath("c:\\tmp\\filename.abc").string(),
            ::jmsd::cutf::internal::UnitTestOptions::GetAbsolutePathToOutputFile());
#else
  GTEST_FLAG(output) ="xml:/tmp/filename.abc";
  EXPECT_EQ(FilePath("/tmp/filename.abc").string(),
            ::jmsd::cutf::internal::UnitTestOptions::GetAbsolutePathToOutputFile());
#endif
}

TEST_F(XmlOutputChangeDirTest, PreserveOriginalWorkingDirWithAbsolutePath) {
#if GTEST_OS_WINDOWS
  const std::string path = "c:\\tmp\\";
#else
  const std::string path = "/tmp/";
#endif

  GTEST_FLAG(output) = "xml:" + path;
  const std::string expected_output_file =
      path + GetCurrentExecutableName().string() + ".xml";
  const std::string& output_file =
      ::jmsd::cutf::internal::UnitTestOptions::GetAbsolutePathToOutputFile();

#if GTEST_OS_WINDOWS
  EXPECT_STRCASEEQ(expected_output_file.c_str(), output_file.c_str());
#else
  EXPECT_EQ(expected_output_file, output_file.c_str());
#endif
}

#endif  // !GTEST_OS_FUCHSIA

}  // namespace
}  // namespace internal
}  // namespace testing
