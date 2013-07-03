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

#include "Joystick.h"

RingBuff_t USARTtoUSB_Buffer;
USB_JoystickReport_Data_t JoystickReportData;

int main(void)
{
    SetupHardware();

    for (;;)
    {
        HID_Task();
        USB_USBTask();
    }
}

void SetupHardware(void)
{
    /* Disable watchdog if enabled by bootloader/fuses */
    MCUSR &= ~(1 << WDRF);
    wdt_disable();

    /* Disable clock division */
    clock_prescale_set(clock_div_1);

    RingBuffer_InitBuffer(&USARTtoUSB_Buffer);

    Serial_Init(115200, true);

    USB_Init();

    UCSR1B = ((1 << RXCIE1) | (1 << TXEN1) | (1 << RXEN1));

    TCCR0B = (1 << CS02);

    GlobalInterruptEnable();
}

void EVENT_USB_Device_Connect(void)
{
}

void EVENT_USB_Device_Disconnect(void)
{
}

void EVENT_USB_Device_ConfigurationChanged(void)
{
    Endpoint_ConfigureEndpoint(JOYSTICK_EPADDR, EP_TYPE_INTERRUPT, JOYSTICK_EPSIZE, 1);
}

void EVENT_USB_Device_ControlRequest(void)
{
    switch (USB_ControlRequest.bRequest)
    {
        case HID_REQ_GetReport:
            if (USB_ControlRequest.bmRequestType == (REQDIR_DEVICETOHOST | REQTYPE_CLASS | REQREC_INTERFACE))
            {
                GetNextReport();

                Endpoint_ClearSETUP();
                Endpoint_Write_Control_Stream_LE(&JoystickReportData, sizeof(JoystickReportData));
                Endpoint_ClearOUT();
            }
            break;
    }
}

bool GetNextReport()
{
    bool InputChanged = false;

    uint32_t BufferCount = RingBuffer_GetCount(&USARTtoUSB_Buffer);

    if (BufferCount >= (sizeof(USB_JoystickReport_Data_t))) {
        uint8_t _1 = RingBuffer_Remove(&USARTtoUSB_Buffer);
        uint8_t _2 = RingBuffer_Remove(&USARTtoUSB_Buffer);
        uint8_t _3 = RingBuffer_Remove(&USARTtoUSB_Buffer);
        uint8_t _4 = RingBuffer_Remove(&USARTtoUSB_Buffer);
        uint8_t _5 = RingBuffer_Remove(&USARTtoUSB_Buffer);

        JoystickReportData.X = _1 | (_2 << 8);
        JoystickReportData.Y = _3 | (_4 << 8);
        JoystickReportData.Buttons = _5;

        InputChanged = true;
    }

    return InputChanged;
}

void HID_Task(void)
{
    if (USB_DeviceState != DEVICE_STATE_Configured)
      return;

    Endpoint_SelectEndpoint(JOYSTICK_EPADDR);

    if (Endpoint_IsINReady())
    {
        GetNextReport();
        Endpoint_Write_Stream_LE(&JoystickReportData, sizeof(JoystickReportData), NULL);
        Endpoint_ClearIN();
    }
}

ISR(USART1_RX_vect, ISR_BLOCK)
{
    uint8_t ReceivedByte = UDR1;

    if ((USB_DeviceState == DEVICE_STATE_Configured) &&
            !RingBuffer_IsFull(&USARTtoUSB_Buffer)) {
        RingBuffer_Insert(&USARTtoUSB_Buffer, ReceivedByte);
    }
}
