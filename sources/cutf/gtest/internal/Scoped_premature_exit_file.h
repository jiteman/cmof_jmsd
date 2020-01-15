#pragma once

#include "Scoped_premature_exit_file.hxx"


#include <string>


namespace jmsd {
namespace cutf {
namespace internal {


// A helper class that creates the premature-exit file in its
// constructor and deletes the file in its destructor.
class ScopedPrematureExitFile {
 public:
  explicit ScopedPrematureExitFile(const char* premature_exit_filepath);

  ~ScopedPrematureExitFile();

private:
	::std::string const premature_exit_filepath_;

private:
	ScopedPrematureExitFile( ScopedPrematureExitFile const & ) = delete;
	ScopedPrematureExitFile &operator ==( ScopedPrematureExitFile const & ) = delete;

};


} // namespace internal
} // namespace cutf
} // namespace jmsd


namespace testing {


} // namespace testing
