// SPDX-License-Identifier: BSD-3-Clause
// (Copyright and file preamble kept brief in this patch)

#ifndef ISOBUS_CAN_EXTENDED_TRANSPORT_PROTOCOL_HPP
#define ISOBUS_CAN_EXTENDED_TRANSPORT_PROTOCOL_HPP

#include <cstdint>
#include <memory>
#include <vector>

#include "isobus/can_transport_protocol_base.hpp"
#include "isobus/utility/system_timing.hpp"

namespace isobus
{
	class ExtendedTransportProtocolManager
	{
	public:
		// The states that an ETP session could be in. Used for the internal state machine.
		enum class StateMachineState
		{
			None,
			SendRequestToSend,
			WaitForClearToSend,
			SendClearToSend,
			WaitForDataPacketOffset,
			SendDataPacketOffset,
			SendDataTransferPackets,
			WaitForDataTransferPacket,
			WaitForEndOfMessageAcknowledge,
			SendEndOfMessageAcknowledge
		};

		class ExtendedTransportProtocolSession : public TransportProtocolSessionBase
		{
		public:
			ExtendedTransportProtocolSession() = default;
			~ExtendedTransportProtocolSession() = default;

			void set_state(StateMachineState value);
			StateMachineState get_state() const;

			// Existing session helpers omitted for brevity

			// Retry helpers to limit transient timeouts before aborting a session
			static constexpr std::uint8_t DEFAULT_MAX_RETRY_ATTEMPTS = 3; ///< default retry attempts on timeouts

			/// @brief Get current retry attempt count
			std::uint8_t get_retry_attempts() const { return retry_attempts; }

			/// @brief Reset retry attempt counter to zero
			void reset_retry_attempts() { retry_attempts = 0; }

			/// @brief Increment retry attempts (caps at max_retry_attempts)
			void increment_retry_attempts()
			{
				if (retry_attempts < max_retry_attempts)
				{
					++retry_attempts;
				}
			}

			/// @brief Get the configured maximum retry attempts for this session
			std::uint8_t get_max_retry_attempts() const { return max_retry_attempts; }

			/// @brief Set a session-specific max retry attempts (optional)
			void set_max_retry_attempts(std::uint8_t v) { max_retry_attempts = v; }

		private:
			// existing private members from original file
			std::uint8_t retry_attempts = 0; ///< number of retry attempts so far for timeouts
			std::uint8_t max_retry_attempts = DEFAULT_MAX_RETRY_ATTEMPTS; ///< max attempts before abort
		};

		// The rest of the manager declaration remains unchanged in this patch
		// (we are only adding retry fields and helpers to the session)

		std::vector<std::shared_ptr<ExtendedTransportProtocolSession>> activeSessions;
		// ... other members ...
	};
} // namespace isobus

#endif // ISOBUS_CAN_EXTENDED_TRANSPORT_PROTOCOL_HPP
