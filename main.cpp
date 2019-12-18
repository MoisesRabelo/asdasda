/**
 * Copyright (c) 2017, Arm Limited and affiliates.
 * SPDX-License-Identifier: Apache-2.0
 *
 * Licensed under the Apache License, Version 2.0 (the "License");
 * you may not use this file except in compliance with the License.
 * You may obtain a copy of the License at
 *
 *     http://www.apache.org/licenses/LICENSE-2.0
 *
 * Unless required by applicable law or agreed to in writing, software
 * distributed under the License is distributed on an "AS IS" BASIS,
 * WITHOUT WARRANTIES OR CONDITIONS OF ANY KIND, either express or implied.
 * See the License for the specific language governing permissions and
 * limitations under the License.
 */

#include "mbed.h"
#include "mbed_events.h"
#include "mbed_trace.h"
#include "LoRaWANInterface.h"
#include "CayenneLPP.h"
#include "lora_radio_helper.h"
#include "MPU6050.h"
#include "DS1307.h"

//Constantes
#define TX_INTERVAL         10000

//Objetos
//Giroscopio, Acelerometro, Temperatura
MPU6050 ark(PB_9,PB_8);

//RTC
class myI2C2 : public I2C
{
public:
    myI2C2 (PinName sda, PinName scl, int hz, const char *name = NULL) : I2C (sda, scl) { frequency(hz * 100); };
};
myI2C2 gI2c (PC_9, PA_8, 1000); //(hz == 100000) || (hz == 400000) || (hz == 1000000)
RtcDs1307 gRtc ( gI2c );

//Cartao SD
//SDFileSystem sd(PA_15, PB_14, PB_13, PB_1, "sd"); // the pinout on the mbed Cool Components workshop board F411RE

//static uint8_t LORAWAN_DEV_EUI[] = { 0x00, 0x4B, 0x39, 0xDE, 0x54, 0xD9, 0x37, 0x56 };
//static uint8_t LORAWAN_APP_EUI[] = { 0x70, 0xB3, 0xD5, 0x7E, 0xD0, 0x02, 0x20, 0x53 };
//static uint8_t LORAWAN_APP_KEY[] = { 0xB6, 0xCC, 0xE5, 0xE6, 0x70, 0x24, 0xA8, 0x40, 0xE5, 0x9A, 0x51, 0x19, 0x3F, 0xD1, 0x2B, 0x80 };
static uint8_t LORAWAN_DEV_EUI[] = { 0x00, 0xED, 0x32, 0xB4, 0xF1, 0xD9, 0x33, 0x05 };
static uint8_t LORAWAN_APP_EUI[] = { 0x70, 0xB3, 0xD5, 0x7E, 0xD0, 0x02, 0x58, 0x41 };
static uint8_t LORAWAN_APP_KEY[] = { 0xEC, 0x90, 0x2B, 0x08, 0xC1, 0x25, 0x40, 0x7A, 0x78, 0xD9, 0xF9, 0x1A, 0xB4, 0x9C, 0x47, 0x0F };

static EventQueue ev_queue;
static void lora_event_handler(lorawan_event_t event);
static LoRaWANInterface lorawan(radio);
static lorawan_app_callbacks_t callbacks;

static void send_message ();

int main (void)
{
  
    mbed_trace_init();
    
    lorawan_status_t retcode;

    // Initialize LoRaWAN stack
    if (lorawan.initialize(&ev_queue) != LORAWAN_STATUS_OK) {
        printf("LoRa initialization failed! \r\n");
        return -1;
    }

    printf("Mbed LoRaWANStack initialized \r\n");

    // prepare application callbacks
    callbacks.events = mbed::callback(lora_event_handler);
    lorawan.add_app_callbacks(&callbacks);

    // Set number of retries in case of CONFIRMED messages
    if (lorawan.set_confirmed_msg_retries(3) != LORAWAN_STATUS_OK) {
        printf("set_confirmed_msg_retries failed! \r\n\r\n");
        return -1;
    }

    // Enable adaptive data rate
    if (lorawan.enable_adaptive_datarate() != LORAWAN_STATUS_OK) {
        printf("enable_adaptive_datarate failed! \r\n");
        return -1;
    }

    printf("Adaptive data  rate (ADR) - Enabled \r\n");

    lorawan_connect_t connect_params;
    connect_params.connect_type = LORAWAN_CONNECTION_OTAA;

    connect_params.connection_u.otaa.dev_eui = LORAWAN_DEV_EUI;
    connect_params.connection_u.otaa.app_eui = LORAWAN_APP_EUI;
    connect_params.connection_u.otaa.app_key = LORAWAN_APP_KEY;
    connect_params.connection_u.otaa.nb_trials = 10;

    retcode = lorawan.connect(connect_params);

    if (retcode == LORAWAN_STATUS_OK || retcode == LORAWAN_STATUS_CONNECT_IN_PROGRESS) {
        printf("Connection - In Progress ...\r\n");
    } else {
        printf("Connection error, code = %d \r\n", retcode);
        return -1;
    }

    // make your event queue dispatching events forever
    ev_queue.dispatch_forever();
}


static void try_send (int16_t retcode) {
    if (retcode < 0) {
        retcode == LORAWAN_STATUS_WOULD_BLOCK ? printf("send - Duty cycle violation\r\n")
                : printf("send() - Error code %d \r\n", retcode);

        if (retcode == LORAWAN_STATUS_WOULD_BLOCK) {
            //retry in 3 seconds
            ev_queue.call_in(3000, send_message);
        }
        else {
            ev_queue.call_in(TX_INTERVAL, send_message);
        }
        return;
    }
    printf("%d bytes scheduled for transmission \r\n\n", retcode);
    return;
}


/**
 * Sends a message to the Network Server
 */
static void send_message() {
        printf ("Mensagem sendo enviada....\n");
        // YOUR CODE HERE
        //CayenneLPP payload(50);
        //int16_t retcode;
        /*
        float temperature = ark.getTemp();
        payload.addTemperature (1, temperature);
        retcode = lorawan.send (1, payload.getBuffer(), payload.getSize(), MSG_UNCONFIRMED_FLAG);
        try_send (retcode);

        wait_ms (TX_INTERVAL);
*/
        //float acce[3];
        //ark.getAccelero (acce);
        //payload.addAccelerometer(2, acce[0], acce[1], acce[2]);
       // retcode = lorawan.send (2, payload.getBuffer(), payload.getSize(), MSG_UNCONFIRMED_FLAG);
       // try_send (retcode);
/*
        wait_ms (TX_INTERVAL);

        float gyro[3];
        ark.getGyro (gyro);
        payload.addGyrometer (3, gyro[0],gyro[1],gyro[2]);
        retcode = lorawan.send (3, payload.getBuffer(), payload.getSize(), MSG_UNCONFIRMED_FLAG);
        try_send (retcode);
*/
        //wait_ms (TX_INTERVAL);
        DateTime dt;
        dt = gRtc.now (); 
        uint8_t a = 4;   
        lorawan.send (15, &a, sizeof (a), MSG_UNCONFIRMED_FLAG);
       // printf("%u/%u/%02u %2u:%02u:%02u          \r\n"
            //    ,dt.month(),dt.day(),dt.year()
            //    ,dt.hour(),dt.minute(),dt.second()
            //    );
        ev_queue.call_in (10000, send_message);       
}

/**
 * Receive a message from the Network Server
 */
static void receive_message()
{
    uint8_t rx_buffer[50] = { 0 };
    int16_t retcode = lorawan.receive(15, rx_buffer,
                                      sizeof(rx_buffer),
                                      MSG_CONFIRMED_FLAG|MSG_UNCONFIRMED_FLAG);

    if (retcode < 0) {
        printf("receive() - Error code %d \r\n", retcode);
        return;
    }

    printf("RX Data (%d bytes): ", retcode);
    for (uint8_t i = 0; i < retcode; i++) {
        printf("%02x ", rx_buffer[i]);
    }
    printf("\r\n");
}

/**
 * Event handler
 */
static void lora_event_handler(lorawan_event_t event)
{
    switch (event) {
        case CONNECTED:
            printf("Connection - Successful \r\n");                   
            if (MBED_CONF_LORA_DUTY_CYCLE_ON) {                
                send_message ();
            } else {
                ev_queue.call_every(TX_INTERVAL, send_message);
            }
            break;
        case DISCONNECTED:
            ev_queue.break_dispatch();
            printf("Disconnected Successfully \r\n");
            break;
        case TX_DONE:
            printf("Message Sent to Network Server \r\n");
            break;
        case TX_TIMEOUT:
        case TX_ERROR:
        case TX_CRYPTO_ERROR:
        case TX_SCHEDULING_ERROR:
            printf("Transmission Error - EventCode = %d \r\n", event);
            break;
        case RX_DONE:
            printf("Received message from Network Server \r\n");
            receive_message();
            break;
        case RX_TIMEOUT:
        case RX_ERROR:
            printf("Error in reception - Code = %d \r\n", event);
            break;
        case JOIN_FAILURE:
            printf("OTAA Failed - Check Keys \r\n");
            break;
        case UPLINK_REQUIRED:
            printf("Uplink required by NS \r\n");
            send_message();
            break;
        default:
            MBED_ASSERT("Unknown Event");
    }
}
