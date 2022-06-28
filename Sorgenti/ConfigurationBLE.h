/*
******************************************************************************
 @file:    ConfigurationBLE.h
 @author:  $ Author: FrancOlino $
 @brief:   Firmware component: BLE configuration
 @version: $ Revision: 1.0 $
 @date:    $ Date: 2022-06-27 $
 -----------------------------------------------------------------------------
 */

#ifndef CONFIGURATIONBLE_H
#define CONFIGURATIONBLE_H


#include <adi_ble_config.h>
#include <drivers/pwr/adi_pwr.h>
#include <radio/adi_ble_radio.h>
#include <common/adi_error_handling.h>
#include <framework/noos/adi_ble_noos.h>

/* Macro to enable or disable Bluetooth */
#define ADI_APP_USE_BLUETOOTH   (1u)

/* ID of the sensor sending "data". In this case data is a string */
#define BIA_ID                   (0x01u)

/* BLE event processing dispatcher timeout. Waits for 2sec for BLE events */
#define ADI_APP_DISPATCH_TIMEOUT (2000)

/* Maximum string size that the data packet can handle  */
#define ADI_MAX_STRING_SIZE         (20u)

/* Macro function to assist in setting the packet header */
#define ADI_SET_HEADER(packet_type, id) ((id & 0x7Fu) | (packet_type << 7))

/* Peripheral advertisement mode */
#define PERIPHERAL_ADV_MODE      ((ADI_BLE_GAP_MODE)(ADI_BLE_GAP_MODE_CONNECTABLE | ADI_BLE_GAP_MODE_DISCOVERABLE))

#pragma pack(pop)

/**
 *  @struct ADI_DATA_PACKET
 *
 *  @brief  packet structure to send data to Matlab Application
 *
 */
#pragma pack(push)
#pragma pack(1)

/* */
typedef struct
{
  uint8_t                     nPacketHeader;      /*< Packet header has the first bit set to 0x1u to indicated this is a */
  
  uint8_t                     aPayload[15];
} ADI_DATA_PACKET;
#pragma pack(pop)

#endif /* CONFIGURATIONBLE_H */