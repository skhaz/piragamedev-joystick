/*
             LUFA Library
     Copyright (C) Dean Camera, 2013.

  dean [at] fourwalledcubicle [dot] com
           www.lufa-lib.org
*/

/*
  Copyright 2013  Dean Camera (dean [at] fourwalledcubicle [dot] com)

  Permission to use, copy, modify, distribute, and sell this
  software and its documentation for any purpose is hereby granted
  without fee, provided that the above copyright notice appear in
  all copies and that both that the copyright notice and this
  permission notice and warranty disclaimer appear in supporting
  documentation, and that the name of the author not be used in
  advertising or publicity pertaining to distribution of the
  software without specific, written prior permission.

  The author disclaims all warranties with regard to this
  software, including all implied warranties of merchantability
  and fitness.  In no event shall the author be liable for any
  special, indirect or consequential damages or any damages
  whatsoever resulting from loss of use, data or profits, whether
  in an action of contract, negligence or other tortious action,
  arising out of or in connection with the use or performance of
  this software.
*/

#include "Descriptors.h"

const USB_Descriptor_HIDReport_Datatype_t PROGMEM JoystickReport[] =
{
    HID_RI_USAGE_PAGE(8, 0x01),
    HID_RI_USAGE(8, 0x04),
    HID_RI_COLLECTION(8, 0x01),
        HID_RI_USAGE(8, 0x01),
        HID_RI_COLLECTION(8, 0x00),
            HID_RI_USAGE(8, 0x30),
            HID_RI_USAGE(8, 0x31),
            HID_RI_LOGICAL_MINIMUM(16, -32768),
            HID_RI_LOGICAL_MAXIMUM(16, 32767),
            HID_RI_PHYSICAL_MINIMUM(16, -32768),
            HID_RI_PHYSICAL_MAXIMUM(16, 32768),
            HID_RI_REPORT_SIZE(8, 0x10),
            HID_RI_REPORT_COUNT(8, 0x02),
            HID_RI_INPUT(8, HID_IOF_DATA | HID_IOF_VARIABLE | HID_IOF_ABSOLUTE),
        HID_RI_END_COLLECTION(0),
        HID_RI_USAGE_PAGE(8, 0x09),
        HID_RI_USAGE_MINIMUM(8, 0x01),
        HID_RI_USAGE_MAXIMUM(8, 0x08),
        HID_RI_LOGICAL_MINIMUM(8, 0x00),
        HID_RI_LOGICAL_MAXIMUM(8, 0x01),
        HID_RI_REPORT_COUNT(8, 0x08),
        HID_RI_REPORT_SIZE(8, 0x01),
        HID_RI_INPUT(8, HID_IOF_DATA | HID_IOF_VARIABLE | HID_IOF_ABSOLUTE),
    HID_RI_END_COLLECTION(0),
};

const USB_Descriptor_Device_t PROGMEM DeviceDescriptor =
{
    .Header                 = { .Size = sizeof(USB_Descriptor_Device_t), .Type = DTYPE_Device },

    .USBSpecification       = VERSION_BCD(01.10),
    .Class                  = USB_CSCP_NoDeviceClass,
    .SubClass               = USB_CSCP_NoDeviceSubclass,
    .Protocol               = USB_CSCP_NoDeviceProtocol,

    .Endpoint0Size          = FIXED_CONTROL_ENDPOINT_SIZE,

    .VendorID               = 0x03EB,
    .ProductID              = 0x2043,
    .ReleaseNumber          = VERSION_BCD(00.01),

    .ManufacturerStrIndex   = 0x01,
    .ProductStrIndex        = 0x02,
    .SerialNumStrIndex      = NO_DESCRIPTOR,

    .NumberOfConfigurations = FIXED_NUM_CONFIGURATIONS
};

const USB_Descriptor_Configuration_t PROGMEM ConfigurationDescriptor =
{
    .Config =
    {
        .Header                 = { .Size = sizeof(USB_Descriptor_Configuration_Header_t), .Type = DTYPE_Configuration },

        .TotalConfigurationSize = sizeof(USB_Descriptor_Configuration_t),
        .TotalInterfaces        = 1,

        .ConfigurationNumber    = 1,
        .ConfigurationStrIndex  = NO_DESCRIPTOR,

        .ConfigAttributes       = (USB_CONFIG_ATTR_RESERVED | USB_CONFIG_ATTR_SELFPOWERED),

        .MaxPowerConsumption    = USB_CONFIG_POWER_MA(100)
     },

    .HID_Interface =
    {
        .Header                 = { .Size = sizeof(USB_Descriptor_Interface_t), .Type = DTYPE_Interface },

        .InterfaceNumber        = 0x00,
        .AlternateSetting       = 0x00,

        .TotalEndpoints         = 1,

        .Class                  = HID_CSCP_HIDClass,
        .SubClass               = HID_CSCP_NonBootSubclass,
        .Protocol               = HID_CSCP_NonBootProtocol,

        .InterfaceStrIndex      = NO_DESCRIPTOR
    },

    .HID_JoystickHID =
    {
        .Header                 = { .Size = sizeof(USB_HID_Descriptor_HID_t), .Type = HID_DTYPE_HID },

        .HIDSpec                = VERSION_BCD(01.11),
        .CountryCode            = 0x00,
        .TotalReportDescriptors = 1,
        .HIDReportType          = HID_DTYPE_Report,
        .HIDReportLength        = sizeof(JoystickReport)
    },

    .HID_ReportINEndpoint =
    {
        .Header                 = { .Size = sizeof(USB_Descriptor_Endpoint_t), .Type = DTYPE_Endpoint },

        .EndpointAddress        = JOYSTICK_EPADDR,
        .Attributes             = (EP_TYPE_INTERRUPT | ENDPOINT_ATTR_NO_SYNC | ENDPOINT_USAGE_DATA),
        .EndpointSize           = JOYSTICK_EPSIZE,
        .PollingIntervalMS      = 0x05
    }
};

const USB_Descriptor_String_t PROGMEM LanguageString =
{
    .Header                 = { .Size = USB_STRING_LEN(1), .Type = DTYPE_String },

    .UnicodeString          = { LANGUAGE_ID_ENG }
};

const USB_Descriptor_String_t PROGMEM ManufacturerString =
{
    .Header                 = { .Size = USB_STRING_LEN(15), .Type = DTYPE_String },
    .UnicodeString          = L"Rodrigo Delduca"
};

const USB_Descriptor_String_t PROGMEM ProductString =
{
    .Header                 = { .Size = USB_STRING_LEN(16), .Type = DTYPE_String },
    .UnicodeString          = L"Skhaz's Joystick (Piece of Cake)"
};

uint16_t CALLBACK_USB_GetDescriptor(const uint16_t wValue,
                                    const uint8_t wIndex,
                                    const void** const DescriptorAddress)
{
    const uint8_t  DescriptorType   = (wValue >> 8);
    const uint8_t  DescriptorNumber = (wValue & 0xFF);

    const void* Address = NULL;
    uint16_t    Size    = NO_DESCRIPTOR;

    switch (DescriptorType)
    {
        case DTYPE_Device:
            Address = &DeviceDescriptor;
            Size    = sizeof(USB_Descriptor_Device_t);
            break;
        case DTYPE_Configuration:
            Address = &ConfigurationDescriptor;
            Size    = sizeof(USB_Descriptor_Configuration_t);
            break;
        case DTYPE_String:
            switch (DescriptorNumber)
            {
                case 0x00:
                    Address = &LanguageString;
                    Size    = pgm_read_byte(&LanguageString.Header.Size);
                    break;
                case 0x01:
                    Address = &ManufacturerString;
                    Size    = pgm_read_byte(&ManufacturerString.Header.Size);
                    break;
                case 0x02:
                    Address = &ProductString;
                    Size    = pgm_read_byte(&ProductString.Header.Size);
                    break;
            }

            break;
        case DTYPE_HID:
            Address = &ConfigurationDescriptor.HID_JoystickHID;
            Size    = sizeof(USB_HID_Descriptor_HID_t);
            break;
        case DTYPE_Report:
            Address = &JoystickReport;
            Size    = sizeof(JoystickReport);
            break;
    }

    *DescriptorAddress = Address;
    return Size;
}

