#ifndef CONFIGURATIONBLE_H
#define CONFIGURATIONBLE_H


#include <adi_ble_config.h>
#include <drivers/pwr/adi_pwr.h>
#include <radio/adi_ble_radio.h>
#include <common/adi_error_handling.h>
#include <framework/noos/adi_ble_noos.h>

/* Macro to enable or disable Bluetooth */
#define ADI_APP_USE_BLUETOOTH   (1u)



/* Indicates the packet is sensor data */
//#define ADI_SENSOR_DATA_PACKET  (1u)

/* BLE event processing dispatcher timeout. Waits for 2sec for BLE events */
#define ADI_APP_DISPATCH_TIMEOUT (2000)

/**************************************************************************

                                Prese dal hello.h

**************************************************************************/

/* Peripheral advertisement mode */
#define PERIPHERAL_ADV_MODE      ((ADI_BLE_GAP_MODE)(ADI_BLE_GAP_MODE_CONNECTABLE | ADI_BLE_GAP_MODE_DISCOVERABLE))

/* Maximum string size that the data packet can handle  */
#define ADI_MAX_STRING_SIZE         (20u)

/* ID of the sensor sending "data". In this case data is a string */
#define DFT_ID                   (0x01u)

/* Macro function to assist in setting the packet header */
#define ADI_SET_HEADER(packet_type, id) ((id & 0x7Fu) | (packet_type << 7))

/*
 * enum ADI_PACKET_TYPE
 * brief Packet type IDs
 *
 * note These correlate with the Android application. So any
 *       changes here must be reflected in SharedDefines.java
 */
typedef enum {
    ADI_REGISTRATION_PACKET     = 0x00u,
    ADI_SENSOR_PACKET           = 0x01u,
} ADI_PACKET_TYPE;

/**
 * \enum ADI_SENSOR_TYPE
 * \brief Senor type IDs for the various types of sensors.
 *
 * \note These correlate with the Android application. So any
 *       changes here must be reflected in SharedDefines.java
 */
  typedef enum {
      ADI_GENERIC_TYPE              = 0x00u,
      ADI_ACCELEROMETER_2G_TYPE     = 0x01u,  
      ADI_CO_TYPE                   = 0x02u,
      ADI_TEMPERATURE_TYPE          = 0x03u,
      ADI_VISIBLELIGHT_TYPE         = 0x04u,
      ADI_PRINTSTRING_TYPE          = 0x05u,
      ADI_ACCELEROMETER_4G_TYPE     = 0x0Bu,
      ADI_ACCELEROMETER_8G_TYPE     = 0x0Cu,
  }ADI_SENSOR_TYPE;


/*
 *  @struct ADI_STRING_DATA
 *
 *  @brief  data structure for the #ADI_PRINTSTRING_TYPE
 *
 */
#pragma pack(push)
#pragma pack(1)
typedef struct
{
  uint8_t           nStringSize;      /*< Size of the string to send. Must not be larger than 13 bytes. */
  uint8_t           aStringData[13];  /*< String to send.                                               */

}ADI_STRING_DATA;
#pragma pack(pop)

/*
 *  @struct ADI_DATA_PACKET
 *
 *  @brief  packet structure to send data to the Android application
 *
 */
#pragma pack(push)
#pragma pack(1)
typedef struct
{
  uint8_t                     nPacketHeader;      /*< Packet header has the first bit set to 0x1u to indicated this is a
                                                      data packet. The rest of the 7 bits are used for the sensor id.
                                                      This should be a unique identifier of the sensor instance.    */
 /* ADI_SENSOR_TYPE             eSensorType;    */    /*< Sensor type                                                   */
  /*uint8_t                     aTimestamp[4];   */   /*< Timestamp value                                               */
  ADI_STRING_DATA             eStringData;        /*< String data                                                   */
} ADI_DATA_PACKET;
#pragma pack(pop)

/********************************************************************
**********************************************************************/

#endif /* CONFIGURATIONBLE_H */