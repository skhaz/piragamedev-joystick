#include <avr/io.h>
#include <avr/wdt.h>
#include <avr/interrupt.h>
#include <util/delay.h>
#include <avr/pgmspace.h>
#include <stdlib.h>
#include <string.h>

#include "usbdrv.h"

PROGMEM const char usbHidReportDescriptor [] = {
    0x05, 0x01,                    // USAGE_PAGE (Generic Desktop)
    0x09, 0x04,                    // USAGE (Joystick)
    0xa1, 0x01,                    // COLLECTION (Application)
    0x09, 0x01,                    //   USAGE (Pointer)
    0xa1, 0x00,                    //   COLLECTION (Physical)
    0x09, 0x30,                    //     USAGE (X)
    0x09, 0x31,                    //     USAGE (Y)
    0x16, 0x00, 0x80,              //     LOGICAL_MINIMUM (-32768)
    0x26, 0xff, 0x7f,              //     LOGICAL_MAXIMUM (32767)
    0x36, 0x00, 0x80,              //     PHYSICAL_MINIMUM (-32768)
    0x46, 0xff, 0x7f,              //     PHYSICAL_MAXIMUM (32767)
    0x75, 0x10,                    //     REPORT_SIZE (16)
    0x95, 0x02,                    //     REPORT_COUNT (2)
    0x81, 0x02,                    //     INPUT (Data,Var,Abs)
    0x05, 0x09,                    //     USAGE_PAGE (Button)
    0x1b, 0x01, 0x00, 0x09, 0x00,  //     USAGE_MINIMUM (Button:Button 1)
    0x2b, 0x08, 0x00, 0x09, 0x00,  //     USAGE_MAXIMUM (Button:Button 8)
    0x15, 0x00,                    //     LOGICAL_MINIMUM (0)
    0x25, 0x01,                    //     LOGICAL_MAXIMUM (1)
    0x95, 0x08,                    //     REPORT_COUNT (8)
    0x75, 0x01,                    //     REPORT_SIZE (1)
    0x81, 0x02,                    //     INPUT (Data,Var,Abs)
    0xc0,                          //   END_COLLECTION
    0xc0                           // END_COLLECTION
};

typedef struct {
    int16_t x;
    int16_t y;
    uint8_t buttons;
} Report_t;

static Report_t report;
static uint32_t idleRate;

usbMsgLen_t usbFunctionSetup(uchar data[8])
{
    usbRequest_t *request = (void *)data;

    if ((request->bmRequestType & USBRQ_TYPE_MASK) == USBRQ_TYPE_CLASS) {
        if (request->bRequest == USBRQ_HID_GET_REPORT) {
            usbMsgPtr = (void *)&report;
            return sizeof(report);
        } else if (request->bRequest == USBRQ_HID_GET_IDLE) {
            usbMsgPtr = &idleRate;
            return 1;
        } else if (request->bRequest == USBRQ_HID_SET_IDLE) {
            idleRate = request->wValue.bytes[1];
        }
    }

    return 0;
}

void buttons_init()
{
    DDRB &= ~(1 << DDB1);
    PORTB |= (1 << PORTB1);

    DDRB &= ~(1 << DDB2);
    PORTB |= (1 << PORTB2);
}

void usb_init()
{
    uint8_t i = 0;
    usbInit();
    usbDeviceDisconnect();

    while (--i) {
        wdt_reset();
        _delay_ms(1);
    }

    usbDeviceConnect();
}

void adc_init()
{
    // Configura o ADC para usar o AVcc como referencia
    ADMUX = (1 << REFS0) | (1 << ADLAR);

    // ADC Enable and prescaler of 128
    // 16000000/128 = 125000
    ADCSRA = (1 << ADEN) | (1 << ADPS2) | (1 << ADPS1) | (1 << ADPS0);
}

int16_t adc_read(uint8_t pin)
{
    // Seleciona o canal
    ADMUX = (ADMUX & 0xE0) + pin;

    // Inicia a conversão
    ADCSRA |= (1 << ADSC);

    // Aguarda até o termino da leitura
    while(ADCSRA & (1 << ADSC))
        ;

    return ADCH;
}

int main()
{
    wdt_enable(WDTO_1S);

    buttons_init();
    adc_init();
    usb_init();

    // Habilita as interrupções
    sei();

    for (;;) {
        wdt_reset();
        usbPoll();

        if (usbInterruptIsReady()) {
            memset(&report, 0, sizeof(report));
            report.x = adc_read(4);
            report.y = adc_read(5);

            if ((PINB & (1 << PINB1)) == 0) {
                report.buttons |= (1 << 4);
            }

            if ((PINB & (1 << PINB2)) == 0) {
                report.buttons |= (1 << 5);
            }

            usbSetInterrupt((void *)&report, sizeof(report));
        }
    }

    return 0;
}

