//================================================================================================
/// @file innomaker_usb2can_plugin.cpp
///
/// @brief An interface for using an INNO-Maker USB2CAN
/// @attention Use of this plugin may affect your license, as the LGPL-2.1 libusb will be linked to.
/// However, if you do not link to it, your executable should remain MIT.
/// This is not legal advice though, and you should be sure you understand the implications
/// of including this plugin.
/// @author Adrian Del Grosso
///
/// @copyright 2023 Adrian Del Grosso
//================================================================================================

#include "isobus/hardware_integration/innomaker_usb2can_windows_plugin.hpp"
#include "isobus/isobus/can_warning_logger.hpp"

#include <thread>

std::unique_ptr<InnoMakerUsb2CanLib::innomaker_can> InnomakerUSB2CANWindowsPlugin::sharedTxContexts = nullptr;
std::unique_ptr<InnoMakerUsb2CanLib> InnomakerUSB2CANWindowsPlugin::driverInstance = nullptr;

InnomakerUSB2CANWindowsPlugin::InnomakerUSB2CANWindowsPlugin(int channel) :
  myChannel(channel)
{
	if (nullptr == driverInstance)
	{
		driverInstance = std::unique_ptr<InnoMakerUsb2CanLib>(new InnoMakerUsb2CanLib());
		driverInstance->setup();
		driverInstance->scanInnoMakerDevice();
	}
	if (nullptr == sharedTxContexts)
	{
		sharedTxContexts = std::unique_ptr<InnoMakerUsb2CanLib::innomaker_can>(new InnoMakerUsb2CanLib::innomaker_can());
	}
}

InnomakerUSB2CANWindowsPlugin::~InnomakerUSB2CANWindowsPlugin()
{
	close();
}

bool InnomakerUSB2CANWindowsPlugin::get_is_valid() const
{
	return true;
}

void InnomakerUSB2CANWindowsPlugin::close()
{
	InnoMakerUsb2CanLib::InnoMakerDevice *device = driverInstance->getInnoMakerDevice(myChannel);

	if (nullptr != device)
	{
		driverInstance->urbResetDevice(device);
		driverInstance->closeInnoMakerDevice(device);
		driverInstance->setdown();
	}
}

void InnomakerUSB2CANWindowsPlugin::open()
{
	InnoMakerUsb2CanLib::UsbCanMode usbCanMode = InnoMakerUsb2CanLib::UsbCanMode::UsbCanModeNormal;
	InnoMakerUsb2CanLib::Innomaker_device_bittming bitTiming;

	InnoMakerUsb2CanLib::InnoMakerDevice *device = driverInstance->getInnoMakerDevice(myChannel);

	if (nullptr != device)
	{
		driverInstance->urbResetDevice(device);
		driverInstance->closeInnoMakerDevice(device);
	}

	// Their documentation is quite bad, but it seems to be
	// a 24 MHz clock, so this would be 250K baud if that is correct.
	bitTiming.prop_seg = 6;
	bitTiming.phase_seg1 = 7;
	bitTiming.phase_seg2 = 2;
	bitTiming.sjw = 1;
	bitTiming.brp = 12;
	driverInstance->urbSetupDevice(device, usbCanMode, bitTiming);
	driverInstance->openInnoMakerDevice(device);
}

bool InnomakerUSB2CANWindowsPlugin::read_frame(isobus::HardwareInterfaceCANFrame &canFrame)
{
	InnoMakerUsb2CanLib::InnoMakerDevice *device = driverInstance->getInnoMakerDevice(myChannel);
	bool retVal = false;

	if (nullptr != device)
	{
		BYTE recvBuffer[sizeof(InnoMakerUsb2CanLib::innomaker_host_frame)];
		memset(recvBuffer, 0, sizeof(InnoMakerUsb2CanLib::innomaker_host_frame));
		retVal = driverInstance->recvInnoMakerDeviceBuf(device, recvBuffer, sizeof(InnoMakerUsb2CanLib::innomaker_host_frame), 1000);
		canFrame.channel = recvBuffer[9];
		canFrame.dataLength = recvBuffer[8];
		canFrame.identifier = (static_cast<std::uint32_t>(recvBuffer[4]) |
		                       (static_cast<std::uint32_t>(recvBuffer[5]) << 8) |
		                       (static_cast<std::uint32_t>(recvBuffer[6]) << 16) |
		                       (static_cast<std::uint32_t>(recvBuffer[7]) << 24));
		canFrame.isExtendedFrame = true;
		canFrame.timestamp_us = (static_cast<std::uint32_t>(recvBuffer[20]) |
		                         (static_cast<std::uint32_t>(recvBuffer[21]) << 8) |
		                         (static_cast<std::uint32_t>(recvBuffer[22]) << 16) |
		                         (static_cast<std::uint32_t>(recvBuffer[23]) << 24));

		for (std::uint_fast8_t i = 0; i < 8; i++)
		{
			canFrame.data[i] = recvBuffer[12 + i];
		}
	}
	return retVal;
}

bool InnomakerUSB2CANWindowsPlugin::write_frame(const isobus::HardwareInterfaceCANFrame &canFrame)
{
	InnoMakerUsb2CanLib::innomaker_host_frame frame;
	bool retVal;

	// Find an empty context to keep track of transmission
	InnoMakerUsb2CanLib::innomaker_tx_context *txc = driverInstance->innomaker_alloc_tx_context(sharedTxContexts.get());
	InnoMakerUsb2CanLib::InnoMakerDevice *device = driverInstance->getInnoMakerDevice(myChannel);
	memcpy(frame.data, canFrame.data, 8);
	frame.can_dlc = canFrame.dataLength;
	frame.channel = canFrame.channel;
	frame.can_id = canFrame.identifier;
	frame.flags = 0;
	frame.reserved = 0;
	frame.echo_id = txc->echo_id;

	BYTE sendBuffer[sizeof(InnoMakerUsb2CanLib::innomaker_host_frame)];
	memcpy(sendBuffer, &frame, sizeof(InnoMakerUsb2CanLib::innomaker_host_frame));
	retVal = driverInstance->sendInnoMakerDeviceBuf(device, sendBuffer, sizeof(InnoMakerUsb2CanLib::innomaker_host_frame), 1);
	return retVal;
}
