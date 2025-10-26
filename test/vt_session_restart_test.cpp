//================================================================================================
/// @file vt_session_restart_test.cpp
///
/// @brief Unit tests for the VT server ETP session restart mechanism
/// @author Adrian Del Grosso
///
/// @copyright 2023 The Open-Agriculture Developers
//================================================================================================
#include <gtest/gtest.h>
#include "isobus/isobus/isobus_virtual_terminal_server.hpp"
#include "isobus/isobus/isobus_virtual_terminal_server_managed_working_set.hpp"

using namespace isobus;

// Test the ETP session restart mechanism
TEST(VTSessionRestartTests, TestSessionRestartMechanism)
{
	// Create a mock control function
	auto mockControlFunction = std::make_shared<isobus::ControlFunction>(isobus::NAME(0), 0x12, 0);
	
	// Create a managed working set
	auto workingSet = std::make_shared<VirtualTerminalServerManagedWorkingSet>(mockControlFunction);
	
	// Initially, there should be no failed transfers
	EXPECT_FALSE(workingSet->has_failed_object_pool_transfer());
	EXPECT_EQ(0u, workingSet->get_retry_count());
	
	// Track a failed object pool transfer
	std::uint32_t failedSize = 2048;
	workingSet->track_failed_object_pool_transfer(failedSize);
	
	// Verify the tracking
	EXPECT_TRUE(workingSet->has_failed_object_pool_transfer());
	EXPECT_EQ(failedSize, workingSet->get_failed_object_pool_size());
	EXPECT_EQ(0u, workingSet->get_retry_count());
	
	// Test incrementing retry count
	workingSet->increment_retry_count();
	EXPECT_EQ(1u, workingSet->get_retry_count());
	
	// Test that we can retry up to the maximum
	for (std::uint32_t i = 0; i < VirtualTerminalServerManagedWorkingSet::MAX_RETRY_COUNT - 1; i++)
	{
		workingSet->increment_retry_count();
	}
	EXPECT_EQ(VirtualTerminalServerManagedWorkingSet::MAX_RETRY_COUNT, workingSet->get_retry_count());
	
	// Test that we don't exceed the maximum
	workingSet->increment_retry_count();
	EXPECT_EQ(VirtualTerminalServerManagedWorkingSet::MAX_RETRY_COUNT, workingSet->get_retry_count());
	
	// Test clearing the failed transfer
	workingSet->clear_failed_object_pool_transfer();
	EXPECT_FALSE(workingSet->has_failed_object_pool_transfer());
	EXPECT_EQ(0u, workingSet->get_failed_object_pool_size());
	EXPECT_EQ(0u, workingSet->get_retry_count());
	
	// Test reset retry count
	workingSet->track_failed_object_pool_transfer(failedSize);
	workingSet->increment_retry_count();
	workingSet->increment_retry_count();
	EXPECT_EQ(2u, workingSet->get_retry_count());
	workingSet->reset_retry_count();
	EXPECT_EQ(0u, workingSet->get_retry_count());
}

// Test that the MAX_RETRY_COUNT constant is set correctly
TEST(VTSessionRestartTests, TestMaxRetryCount)
{
	EXPECT_EQ(5u, VirtualTerminalServerManagedWorkingSet::MAX_RETRY_COUNT);
}