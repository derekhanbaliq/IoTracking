/**************************************************************************//**
* @file      WifiHandler.h
* @brief     File to handle HTTP Download and MQTT support
* @author    Eduardo Garcia
* @date      2020-01-01

******************************************************************************/


 #pragma once

 #pragma once
 #ifdef __cplusplus
 extern "C" {
	 #endif

	 /******************************************************************************
	 * Includes
	 ******************************************************************************/

	 /******************************************************************************
	 * Defines
	 ******************************************************************************/

	 #define WIFI_TASK_SIZE	1000
	 #define WIFI_PRIORITY (configMAX_PRIORITIES - 2) 
	 
/** Wi-Fi AP Settings. */
#define MAIN_WLAN_SSID                       "EdWifi" /**< Destination SSID */
#define MAIN_WLAN_AUTH                       M2M_WIFI_SEC_WPA_PSK /**< Security manner */
#define MAIN_WLAN_PSK                        "Armitage" /**< Password for Destination SSID */

/** IP address parsing. */
#define IPV4_BYTE(val, index)                ((val >> (index * 8)) & 0xFF)

/** Content URI for download. */
#define MAIN_HTTP_FILE_URL                   "http://www.orimi.com/pdf-test.pdf"

/** Maximum size for packet buffer. */
#define MAIN_BUFFER_MAX_SIZE                 (1446)
/** Maximum file name length. */
#define MAIN_MAX_FILE_NAME_LENGTH            (64)
/** Maximum file extension length. */
#define MAIN_MAX_FILE_EXT_LENGTH             (8)
/** Output format with '0'. */
#define MAIN_ZERO_FMT(SZ)                    (SZ == 4) ? "%04d" : (SZ == 3) ? "%03d" : (SZ == 2) ? "%02d" : "%d"

typedef enum {
	NOT_READY = 0, /*!< Not ready. */
	STORAGE_READY = 0x01, /*!< Storage is ready. */
	WIFI_CONNECTED = 0x02, /*!< Wi-Fi is connected. */
	GET_REQUESTED = 0x04, /*!< GET request is sent. */
	DOWNLOADING = 0x08, /*!< Running to download. */
	COMPLETED = 0x10, /*!< Download completed. */
	CANCELED = 0x20 /*!< Download canceled. */
} download_state;





/* Max size of UART buffer. */
#define MAIN_CHAT_BUFFER_SIZE 64

/* Max size of MQTT buffer. */
#define MAIN_MQTT_BUFFER_SIZE 512

/* Limitation of user name. */
#define MAIN_CHAT_USER_NAME_SIZE 64

/* Chat MQTT topic. */
#define TEMPERATURE_TOPIC	"TempData"
#define LED_TOPIC			"LedData"

#define LED_TOPIC_LED_OFF	 "false"
#define LED_TOPIC_LED_ON	 "true"

//Cloud MQTT User
#define CLOUDMQTT_USER_ID	"rttyobej"


//Cloud MQTT pASSWORD
#define CLOUDMQTT_USER_PASSWORD	"BrsJBNVoQBl7"

#define CLOUDMQTT_PORT		11625

/*
 * A MQTT broker server which was connected.
 * m2m.eclipse.org is public MQTT broker.
 */
static const char main_mqtt_broker[] = "m16.cloudmqtt.com";



#define STRING_EOL                      "\r\n"
#define STRING_HEADER                   "-- HTTP file downloader example --"STRING_EOL \
"-- "BOARD_NAME " --"STRING_EOL	\
"-- Compiled: "__DATE__ " "__TIME__ " --"STRING_EOL


	 /******************************************************************************
	 * Structures and Enumerations
	 ******************************************************************************/



	 /******************************************************************************
	 * Global Function Declaration
	 ******************************************************************************/
void vWifiTask( void *pvParameters );
void init_storage(void);

	 #ifdef __cplusplus
 }
 #endif