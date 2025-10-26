#include <gtest/gtest.h>
#include "isobus/isobus/can_extended_transport_protocol.hpp"
#include "isobus/isobus/can_internal_control_function.hpp"
#include "isobus/isobus/can_partnered_control_function.hpp"
#include "isobus/isobus/can_message.hpp"
#include "isobus/isobus/can_network_configuration.hpp"

// Mock CAN send function
bool mockCANSendFunction(std::uint32_t parameterGroupNumber,
                         const isobus::CANDataSpan &data,
                         std::shared_ptr<isobus::InternalControlFunction> source,
                         std::shared_ptr<isobus::ControlFunction> destination,
                         isobus::CANIdentifier::CANPriority priority)
{
	// Always succeed for testing
	return true;
}

// Mock CAN message received callback
void mockCANMessageReceivedCallback(const isobus::CANMessage &message)
{
	// Do nothing for testing
}

TEST(ETPRetryTest, TestRetryCountMethods)
{
	// Create mock control functions
	auto internalECU = std::make_shared<isobus::InternalControlFunction>(nullptr, 0, 0);
	auto externalECU = std::make_shared<isobus::PartneredControlFunction>(0, nullptr);

	// Create a session
	auto session = std::make_shared<isobus::ExtendedTransportProtocolManager::ExtendedTransportProtocolSession>(
		isobus::ExtendedTransportProtocolSession::Direction::Transmit,
		nullptr,
		0x12345,
		2000,
		internalECU,
		externalECU,
		nullptr,
		nullptr
	);

	// Test initial retry count
	EXPECT_EQ(session->get_retry_count(), 0);

	// Test incrementing retry count
	session->increment_retry_count();
	EXPECT_EQ(session->get_retry_count(), 1);

	// Test resetting retry count
	session->reset_retry_count();
	EXPECT_EQ(session->get_retry_count(), 0);
	
	// Test incrementing multiple times
	for (int i = 0; i < 5; i++) {
		session->increment_retry_count();
	}
	EXPECT_EQ(session->get_retry_count(), 5);
	
	// Test reset works after multiple increments
	session->reset_retry_count();
	EXPECT_EQ(session->get_retry_count(), 0);
}

TEST(ETPRetryTest, TestETPManagerCreation)
{
	// Create ETP manager
	isobus::CANNetworkConfiguration config;
	isobus::ExtendedTransportProtocolManager etpManager(mockCANSendFunction, mockCANMessageReceivedCallback, &config);

	// Test that the manager was created successfully
	EXPECT_TRUE(true); // If we get here, the manager was created without crashing
}