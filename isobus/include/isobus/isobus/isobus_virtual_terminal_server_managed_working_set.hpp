//================================================================================================
/// @file isobus_virtual_terminal_server_managed_working_set.hpp
///
/// @brief Defines a class that manages a VT server's active working sets.
/// @author Adrian Del Grosso
///
/// @copyright 2023 Adrian Del Grosso
//================================================================================================
#ifndef ISOBUS_VIRTUAL_TERMINAL_SERVER_MANAGED_WORKING_SET_HPP
#define ISOBUS_VIRTUAL_TERMINAL_SERVER_MANAGED_WORKING_SET_HPP

#include "isobus/isobus/can_badge.hpp"
#include "isobus/isobus/can_control_function.hpp"
#include "isobus/isobus/isobus_virtual_terminal_objects.hpp"
#include "isobus/utility/event_dispatcher.hpp"

#include <atomic>
#include <list>
#include <memory>
#include <mutex>
#include <thread>
#include <vector>

namespace isobus
{
	class VirtualTerminalServer;

	//================================================================================================
	/// @class VirtualTerminalServerManagedWorkingSet
	///
	/// @brief This class manages a single client's working set on a VT server.
	//================================================================================================
	class VirtualTerminalServerManagedWorkingSet
	{
	public:
		/// @brief Enumerates the possible states of the object pool processing thread
		enum class ObjectPoolProcessingThreadState
		{
			None, ///< No processing thread is running
			Running, ///< The processing thread is running
			Success, ///< The processing thread completed successfully
			Fail, ///< The processing thread failed
			Joined ///< The processing thread has been joined
		};

		/// @brief Constructor for a managed working set with no associated control function
		VirtualTerminalServerManagedWorkingSet();

		/// @brief Constructor for a managed working set
		/// @param[in] associatedControlFunction The control function associated with this working set
		explicit VirtualTerminalServerManagedWorkingSet(std::shared_ptr<ControlFunction> associatedControlFunction);

		/// @brief Destructor for a managed working set
		~VirtualTerminalServerManagedWorkingSet();

		/// @brief Starts the object pool parsing thread
		void start_parsing_thread();

		/// @brief Joins the object pool parsing thread if it exists
		void join_parsing_thread();

		/// @brief Returns if any object pools have been added to this working set
		/// @returns true if any object pools have been added to this working set
		bool get_any_object_pools() const;

		/// @brief Returns the current state of the object pool processing thread
		/// @returns The current state of the object pool processing thread
		ObjectPoolProcessingThreadState get_object_pool_processing_state();

		/// @brief Returns the control function associated with this working set
		/// @returns The control function associated with this working set
		std::shared_ptr<ControlFunction> get_control_function() const;

		/// @brief Returns the timestamp of the last working set maintenance message
		/// @returns The timestamp of the last working set maintenance message
		std::uint32_t get_working_set_maintenance_message_timestamp_ms() const;

		/// @brief Sets the timestamp of the last working set maintenance message
		/// @param[in] value The timestamp of the last working set maintenance message
		void set_working_set_maintenance_message_timestamp_ms(std::uint32_t value);

		/// @brief Saves a callback handle for later removal
		/// @param[in] callbackHandle The callback handle to save
		void save_callback_handle(EventCallbackHandle callbackHandle);

		/// @brief Clears all saved callback handles
		void clear_callback_handles();

		/// @brief Returns if the object pool was loaded from non-volatile memory
		/// @returns true if the object pool was loaded from non-volatile memory
		bool get_was_object_pool_loaded_from_non_volatile_memory() const;

		/// @brief Sets if the object pool was loaded from non-volatile memory
		/// @param[in] value true if the object pool was loaded from non-volatile memory
		void set_was_object_pool_loaded_from_non_volatile_memory(bool value, CANLibBadge<VirtualTerminalServer>);

		/// @brief Sets the object focus
		/// @param[in] objectID The object ID to focus
		void set_object_focus(std::uint16_t objectID);

		/// @brief Returns the object focus
		/// @returns The object focus
		std::uint16_t get_object_focus() const;

		/// @brief Sets the auxiliary input maintenance timestamp
		/// @param[in] value The auxiliary input maintenance timestamp
		void set_auxiliary_input_maintenance_timestamp_ms(std::uint32_t value);

		/// @brief Returns the auxiliary input maintenance timestamp
		/// @returns The auxiliary input maintenance timestamp
		std::uint32_t get_auxiliary_input_maintenance_timestamp_ms() const;

		/// @brief Requests deletion of this working set
		void request_deletion();

		/// @brief Returns if deletion has been requested
		/// @returns true if deletion has been requested
		bool is_deletion_requested() const;

		/// @brief Sets the IOP size
		/// @param[in] newIopSize The new IOP size
		void set_iop_size(std::uint32_t newIopSize);

		/// @brief Returns the IOP load percentage
		/// @returns The IOP load percentage
		float iop_load_percentage() const;

		/// @brief Adds IOP raw data to the working set
		/// @param[in] data The IOP raw data to add
		void add_iop_raw_data(const std::vector<std::uint8_t> &data);

		/// @brief Returns if an object pool transfer is in progress
		/// @returns true if an object pool transfer is in progress
		bool is_object_pool_transfer_in_progress() const;

		/// @brief Sets the object pool processing state
		/// @param[in] value The object pool processing state
		void set_object_pool_processing_state(ObjectPoolProcessingThreadState value);

		/// @brief Returns the list of objects in this working set
		/// @returns The list of objects in this working set
		const std::list<std::shared_ptr<VTObject>> &get_object_list() const;

		/// @brief Returns an object by its ID
		/// @param[in] objectID The ID of the object to return
		/// @returns The object with the specified ID, or nullptr if not found
		std::shared_ptr<VTObject> get_object_by_id(std::uint16_t objectID) const;

		/// @brief Adds an object to this working set
		/// @param[in] object The object to add
		void add_object(std::shared_ptr<VTObject> object);

		/// @brief Removes an object from this working set
		/// @param[in] object The object to remove
		void remove_object(std::shared_ptr<VTObject> object);

		/// @brief Returns the number of objects in this working set
		/// @returns The number of objects in this working set
		std::size_t get_number_objects() const;

		/// @brief Returns the IOP size
		/// @returns The IOP size
		std::uint32_t get_iop_size() const;

		/// @brief Tracks a failed object pool transfer for retry
		/// @param[in] dataSize The size of the data that failed to transfer
		void track_failed_object_pool_transfer(std::uint32_t dataSize);

		/// @brief Returns if there's a failed object pool transfer that needs retry
		/// @returns true if there's a failed object pool transfer that needs retry
		bool has_failed_object_pool_transfer() const;

		/// @brief Gets the size of the failed object pool transfer
		/// @returns The size of the failed object pool transfer
		std::uint32_t get_failed_object_pool_size() const;

		/// @brief Clears the failed object pool transfer tracking
		void clear_failed_object_pool_transfer();

		/// @brief Increments the retry count for failed object pool transfers
		void increment_retry_count();

		/// @brief Returns the current retry count for failed object pool transfers
		/// @returns The current retry count
		std::uint8_t get_retry_count() const;

		/// @brief Resets the retry count for failed object pool transfers
		void reset_retry_count();

	private:
		/// @brief The worker thread function for parsing object pools
		void worker_thread_function();

		/// @brief Parses IOP data into VT objects
		/// @param[in] data The IOP data to parse
		/// @param[in] size The size of the IOP data
		/// @returns true if the IOP data was parsed successfully
		bool parse_iop_into_objects(const std::uint8_t *data, std::uint32_t size);

		/// @brief Parses a single IOP object
		/// @param[in] data The IOP data to parse
		/// @param[in] size The size of the IOP data
		/// @param[out] objectSize The size of the parsed object
		/// @returns The parsed VT object
		std::shared_ptr<VTObject> parse_iop_object(const std::uint8_t *data, std::uint32_t size, std::uint32_t &objectSize);

		/// @brief The control function associated with this working set
		std::shared_ptr<ControlFunction> workingSetControlFunction;

		/// @brief The list of objects in this working set
		std::list<std::shared_ptr<VTObject>> objectList;

		/// @brief The raw IOP data
		std::vector<std::vector<std::uint8_t>> iopFilesRawData;

		/// @brief The size of the IOP data
		std::uint32_t iopSize = 0;

		/// @brief The size of the transferred IOP data
		std::atomic_uint32_t transferredIopSize = 0;

		/// @brief The timestamp of the last working set maintenance message
		std::atomic_uint32_t workingSetMaintenanceMessageTimestamp_ms = 0;

		/// @brief The timestamp of the last auxiliary input maintenance message
		std::atomic_uint32_t auxiliaryInputMaintenanceMessageTimestamp_ms = 0;

		/// @brief The object focus
		std::atomic_uint16_t focusedObject = NULL_OBJECT_ID;

		/// @brief The object pool processing thread
		std::unique_ptr<std::thread> objectPoolProcessingThread;

		/// @brief The object pool processing state
		ObjectPoolProcessingThreadState processingState = ObjectPoolProcessingThreadState::None;

		/// @brief The mutex for the managed working set
		mutable std::mutex managedWorkingSetMutex;

		/// @brief The list of callback handles
		std::vector<EventCallbackHandle> callbackHandles;

		/// @brief Flag indicating if the object pool was loaded from non-volatile memory
		bool wasLoadedFromNonVolatileMemory = false;

		/// @brief Flag indicating if deletion has been requested
		bool workingSetDeletionRequested = false;

		/// @brief Size of the failed object pool transfer
		std::uint32_t failedObjectPoolSize = 0;

		/// @brief Retry count for failed object pool transfers
		std::uint8_t retryCount = 0;

		/// @brief Maximum number of retries for failed object pool transfers
		static constexpr std::uint8_t MAX_RETRY_COUNT = 3;
	};

} // namespace isobus

#endif // ISOBUS_VIRTUAL_TERMINAL_SERVER_MANAGED_WORKING_SET_HPP