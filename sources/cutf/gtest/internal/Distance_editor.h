#pragma once

#include "Distance_editor.hxx"


#include <string>
#include <vector>


namespace jmsd {
namespace cutf {
namespace internal {


class Distance_editor {

	enum EditType {
		kMatch,
		kAdd,
		kRemove,
		kReplace };

public:
	// Returns the optimal edits to go from 'left' to 'right'.
	// All edits cost the same, with replace having lower priority than add/remove.
	// Simple implementation of the Wagner-Fischer algorithm.
	// See http://en.wikipedia.org/wiki/Wagner-Fischer_algorithm
	static ::std::vector< EditType > CalculateOptimalEdits(
		 ::std::vector< size_t > const &left,
		 ::std::vector< size_t > const &right );

	// Same as above, but the input is represented as strings.
	static ::std::vector< EditType > CalculateOptimalEdits(
		::std::vector< ::std::string > const &left,
		::std::vector< ::std::string > const &right);

	// Create a diff of the input strings in Unified diff format.
	static ::std::string CreateUnifiedDiff(
		::std::vector< ::std::string > const &left,
		::std::vector< ::std::string > const &right,
		size_t context = 2 );

// - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -
public:
//	enum EditType {
//		kMatch,
//		kAdd,
//		kRemove,
//		kReplace };

// = = = = = = = = = = = = = = = = = = = = = = = = = = = = = = = = = = = = = = = = = = = = = = = = = = = = = = = = = = = = = = =
private:
	virtual ~Distance_editor() noexcept = delete;

// - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -
private:
	Distance_editor() noexcept = delete;

// - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -
private:
	Distance_editor( Distance_editor const &another ) noexcept = delete;
	Distance_editor &operator =( Distance_editor const &another ) noexcept = delete;

// - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -
private:
	Distance_editor( Distance_editor &&another ) noexcept = delete;
	Distance_editor &operator =( Distance_editor &&another ) noexcept = delete;

// # # # # # # # # # # # # # # # # # # # # # # # # # # # # # # # # # # # # # # # # # # # # # # # # # # # # # # # # # # # # # # #
private:
	class InternalStrings;
	class Hunk;

// - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -
private:

// - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -
private:

};


} // namespace internal
} // namespace cutf
} // namespace jmsd
