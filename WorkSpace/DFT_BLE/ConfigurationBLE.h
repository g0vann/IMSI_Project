#ifndef CONFIGURATIONBLE_H
#define CONFIGURATIONBLE_H


#include <adi_ble_config.h>
#include <drivers/pwr/adi_pwr.h>
#include <radio/adi_ble_radio.h>
#include <common/adi_error_handling.h>
#include <framework/noos/adi_ble_noos.h>

/* Macro to enable or disable Bluetooth */
#define ADI_APP_USE_BLUETOOTH   (1u)

/* Accelerometer instance ID */
#define ADI_TEMPERATURE_ID    (1u)

/* Indicates the packet is sensor data */
//#define ADI_SENSOR_DATA_PACKET  (1u)

/* BLE event processing dispatcher timeout. Waits for 2sec for BLE events */
#define ADI_APP_DISPATCH_TIMEOUT (2000)


#endif /* CONFIGURATIONBLE_H */