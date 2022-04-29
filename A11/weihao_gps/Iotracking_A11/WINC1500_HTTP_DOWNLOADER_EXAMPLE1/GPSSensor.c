/**************************************************************************/ /**
 * @file      GPSSensor.c
 * @brief     testing GPS driver


 * @date      2020-04-08

 ******************************************************************************/

/******************************************************************************
 * Includes
 ******************************************************************************/
#include "GPSDriver/GPSSensor.h"

#include "I2cDriver/I2cDriver.h"
#include "SerialConsole/SerialConsole.h"
#define GPS_READ 0x55
/******************************************************************************
 * Variables
 ******************************************************************************/

struct usart_module usart_instance_GPS;  ///< GPS sensor UART module

SemaphoreHandle_t sensorGPSMutexHandle;      ///< Mutex to handle the sensor I2C bus thread access.
SemaphoreHandle_t sensorGPSSemaphoreHandle;  ///< Binary semaphore to notify task that we have received an I2C interrupt on the Sensor bus

/******************************************************************************
 * Structures and Enumerations
 ******************************************************************************/
uint8_t GPSTx;
uint8_t latestRxGPS[2];
/******************************************************************************
 *  Callback Declaration
 ******************************************************************************/
// Callback for when we finish writing characters to UART
void GPSUsartWritecallback(struct usart_module *const usart_module)
{
    BaseType_t xHigherPriorityTaskWoken = pdFALSE;

    xSemaphoreGiveFromISR(sensorGPSSemaphoreHandle, &xHigherPriorityTaskWoken);
    portYIELD_FROM_ISR(xHigherPriorityTaskWoken);
}
// Callback for when we finish writing characters to UART

void GPSUsartReadcallback(struct usart_module *const usart_module)
{
    BaseType_t xHigherPriorityTaskWoken = pdFALSE;

    xSemaphoreGiveFromISR(sensorGPSSemaphoreHandle, &xHigherPriorityTaskWoken);
    portYIELD_FROM_ISR(xHigherPriorityTaskWoken);
}

/******************************************************************************
 * Local Function Declaration
 ******************************************************************************/
static void configure_usart(void);
static void configure_usart_callbacks(void);
static int32_t GPSSensorFreeMutex(void);
static int32_t GPSSensorGetMutex(TickType_t waitTime);
/******************************************************************************
 * Global Local Variables
 ******************************************************************************/

/******************************************************************************
 * Global Functions
 ******************************************************************************/

/**
 * @fn			void InitializeSerialConsole(void)
 * @brief		Initializes the UART - sets up the SERCOM to act as UART and registers the callbacks for
 *				asynchronous reads and writes.
 * @details		Initializes the UART - sets up the SERCOM to act as UART and registers the callbacks for
 *				asynchronous reads and writes.
 * @note			Call from main once to initialize Hardware.
 */

void InitializeGPSSensor(void)
{
    // Configure USART and Callbacks
    configure_usart();
    configure_usart_callbacks();

    sensorGPSMutexHandle = xSemaphoreCreateMutex();
    sensorGPSSemaphoreHandle = xSemaphoreCreateBinary();

    if (NULL == sensorGPSMutexHandle || NULL == sensorGPSSemaphoreHandle) {
        SerialConsoleWriteString((char *)"Could not initialize GPS Sensor!");
    }
}

/**
 * @fn			void DeinitializeSerialConsole(void)
 * @brief		Deinitialises the UART
 * @note
 */
void DeinitializeGPSSerial(void)
{
    usart_disable(&usart_instance_GPS);
}

/**
 * @fn			int32_t DistanceSensorGetDistance (uint16_t *distance)
 * @brief		Gets the distance from the distance sensor.
 * @note			Returns 0 if successful. -1 if an error occurred
 */
int32_t GPSSensorRead(char *gps, const TickType_t xMaxBlockTime)
{
    int error = ERROR_NONE;

    // 1. Get MUTEX. DistanceSensorGetMutex. If we cant get it, goto
    error = GPSSensorGetMutex(WAIT_I2C_LINE_MS);
    if (ERROR_NONE != error) goto exitf;

    //---2. Initiate sending data. First populate TX with the distance command. Use usart_write_buffer_job to transmit 1 character
    GPSTx = GPS_READ;
    if (STATUS_OK != usart_write_buffer_job(&usart_instance_GPS, (uint8_t *)&GPSTx, 1)) {
        goto exitf;
    }

    // 3. )Wait until the TX finished. TX should release the binary semaphore - so wait until semaphore
    if (xSemaphoreTake(sensorGPSSemaphoreHandle, xMaxBlockTime) == pdTRUE) {
        /* The transmission ended as expected. We now delay until the I2C sensor is finished */

    } else {
        /* The call to ulTaskNotifyTake() timed out. */
        error = ERR_TIMEOUT;
        goto exitf;
    }

    // 4. Initiate an rx job - usart_read_buffer_job - to read two characters. Read into variable latestRxDistance
    usart_read_buffer_job(&usart_instance_GPS, (uint8_t *)&latestRxGPS, 2);  // Kicks off constant reading of characters

    //---7. Wait for notification
    if (xSemaphoreTake(sensorGPSSemaphoreHandle, xMaxBlockTime) == pdTRUE) {
        /* The transmission ended as expected. We now delay until the I2C sensor is finished */
        *gps = (latestRxGPS[0] << 8) + latestRxGPS[1];
    } else {
        /* The call to ulTaskNotifyTake() timed out. */
        error = ERR_TIMEOUT;
        goto exitf;
    }

exitf:
    // Release mutex and return error
    GPSSensorFreeMutex();

    return error;
}

/**
 * @fn			static void configure_usart(void)
 * @brief		Code to configure the SERCOM "EDBG_CDC_MODULE" to be a UART channel running at 115200 8N1
 * @note
 */
static void configure_usart(void)
{
    struct usart_config config_usart;
    usart_get_config_defaults(&config_usart);

    config_usart.baudrate = 9600;
    config_usart.mux_setting = USART_RX_2_TX_0_RTS_2_CTS_3;
    config_usart.pinmux_pad0 = PINMUX_PA11D_SERCOM2_PAD3; //TX
    config_usart.pinmux_pad1 = PINMUX_PA10D_SERCOM2_PAD2; //RX
    config_usart.pinmux_pad2 = PINMUX_UNUSED;
    config_usart.pinmux_pad3 = PINMUX_UNUSED;

    while (usart_init(&usart_instance_GPS, SERCOM2, &config_usart) != STATUS_OK) {
    }

    usart_enable(&usart_instance_GPS);
}

/**
 * @fn			static void configure_usart_callbacks(void)
 * @brief		Code to register callbacks
 * @note
 */
static void configure_usart_callbacks(void)
{
    usart_register_callback(&usart_instance_GPS, GPSUsartWritecallback, USART_CALLBACK_BUFFER_TRANSMITTED);
    usart_register_callback(&usart_instance_GPS, GPSUsartReadcallback, USART_CALLBACK_BUFFER_RECEIVED);
    usart_enable_callback(&usart_instance_GPS, USART_CALLBACK_BUFFER_TRANSMITTED);
    usart_enable_callback(&usart_instance_GPS, USART_CALLBACK_BUFFER_RECEIVED);
}

/**
 * @fn			int32_t DistanceSensorFreeMutex(eI2cBuses bus)
 * @brief       Frees the mutex of the given UART bus
 * @details
 * @param[in]   bus Enum that represents the bus in which we are interested to free the mutex of.
 * @return      Returns (0) if the bus is ready, (1) if it is busy.
 * @note
 */
static int32_t GPSSensorFreeMutex(void)
{
    int32_t error = ERROR_NONE;

    if (xSemaphoreGive(sensorGPSMutexHandle) != pdTRUE) {
        error = ERROR_NOT_INITIALIZED;  // We could not return the mutex! We must not have it!
    }
    return error;
}

/**
 * @fn			int32_t I2cGetMutex(TickType_t waitTime)
 * @brief       Frees the mutex of the given UART bus
 * @details
 * @param[in]   waitTime Time to wait for the mutex to be freed.
 * @return      Returns (0) if the bus is ready, (1) if it is busy.
 * @note
 */
static int32_t GPSSensorGetMutex(TickType_t waitTime)
{
    int32_t error = ERROR_NONE;
    if (xSemaphoreTake(sensorGPSMutexHandle, waitTime) != pdTRUE) {
        error = ERROR_NOT_READY;
    }
    return error;
}