// This file tests the internal cross-platform support utilities.

#include "gmock/internal/gmock-port.h"

#include "gtest/gtest.h"


// NOTE: if this file is left without tests for some reason, put a dummy
// test here to make references to symbols in the gtest library and avoid
// 'undefined symbol' linker errors in gmock_main:

TEST(DummyTest, Dummy) {}
