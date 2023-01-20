//================================================================================================
/// @file innomaker_usb2can_plugin.hpp
///
/// @brief An interface for using a PEAK PCAN device.
/// @attention Use of the PEAK driver is governed in part by their license, and requires you
/// to install their driver first, which in-turn requires you to agree to their terms and conditions.
/// @author Adrian Del Grosso
///
/// @copyright 2022 Adrian Del Grosso
//================================================================================================
#ifndef INNOMAKER_USB2CAN_PLUGIN_HPP
#define INNOMAKER_USB2CAN_PLUGIN_HPP

#include <memory>
#include <string>

#include "isobus/hardware_integration/InnoMakerUsb2CanLib.h"
#include "isobus/hardware_integration/can_hardware_plugin.hpp"
#include "isobus/isobus/can_frame.hpp"
#include "isobus/isobus/can_hardware_abstraction.hpp"

/// @brief A Windows CAN Driver for Innomaker USB2CAN Devices
class InnomakerUSB2CANWindowsPlugin : public CANHardwarePlugin
{
public:
	/// @brief Constructor for the Windows version of the Innomaker USB2CAN Windows CAN driver
	/// @param[in] channel The channel to use by index, passed directly to the consuming driver
	explicit InnomakerUSB2CANWindowsPlugin(int channel);

	/// @brief The destructor for InnomakerUSB2CANWindowsPlugin
	virtual ~InnomakerUSB2CANWindowsPlugin();

	/// @brief Returns if the connection with the hardware is valid
	/// @returns `true` if connected, `false` if not connected
	bool get_is_valid() const override;

	/// @brief Closes the connection to the hardware
	void close() override;

	/// @brief Connects to the hardware you specified in the constructor's channel argument
	void open() override;

	/// @brief Returns a frame from the hardware (synchronous), or `false` if no frame can be read.
	/// @param[in, out] canFrame The CAN frame that was read
	/// @returns `true` if a CAN frame was read, otherwise `false`
	bool read_frame(isobus::HardwareInterfaceCANFrame &canFrame) override;

	/// @brief Writes a frame to the bus (synchronous)
	/// @param[in] canFrame The frame to write to the bus
	/// @returns `true` if the frame was written, otherwise `false`
	bool write_frame(const isobus::HardwareInterfaceCANFrame &canFrame) override;

private:
	static std::unique_ptr<InnoMakerUsb2CanLib::innomaker_can> sharedTxContexts; /// Stores Tx tickets for the driver
	static std::unique_ptr<InnoMakerUsb2CanLib> driverInstance; ///< The driver itself
	int myChannel; ///< Stores the channel associated with this object
};

#endif // INNOMAKER_USB2CAN_PLUGIN_HPP
