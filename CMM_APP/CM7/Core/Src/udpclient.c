#include "cmsis_os.h"
#include "lwip/opt.h"
#include "lwip/api.h"
#include "lwip/sys.h"
#include "lwip/sockets.h"
#include "udpclient.h"
#include "string.h"
#include <stdio.h>

/* Define UDP ports */
#define TRIGGER_SEND_PORT 5005      // Port used to send trigger data
#define SERIAL_SEND_PORT 5007       // Port used to send serial data
#define LOCAL_PORT_RECV 5006        // Common port for receiving both trigger and serial data

/* Function prototypes */
void udpclient_init(void);
static void udprecv_thread(void *arg);
void udpsend_trigger(const char *data, size_t len);
void udpsend_serial(const char *data, size_t len);
void udp_set_server_ip(const char *ip);

/* Global variables */
static struct netconn *conn_send_trigger;
static struct netconn *conn_send_serial;
static struct netconn *conn_recv;
static ip_addr_t server_addr;

/**
  * @brief  UDP thread for receiving data from the server on a specified port.
  * @param  arg: Argument passed to the thread function (unused).
  * @retval None
  */
static void udprecv_thread(void *arg)
{
    err_t recv_err;
    struct netbuf *rxbuf;

    /* Set receive timeout */
    netconn_set_recvtimeout(conn_recv, 5000);

    while (1)
    {
        recv_err = netconn_recv(conn_recv, &rxbuf);
        if (recv_err == ERR_OK)
        {
            void *payload;
            uint16_t len;
            uint16_t recv_port = netbuf_fromport(rxbuf); // Get the source port

            netbuf_data(rxbuf, &payload, &len);

            /* Process received data based on the source port */
            if (recv_port == TRIGGER_SEND_PORT) {
                printf("Received trigger data (len=%d): %.*s\n", len, len, (char *)payload);

                /* Handle trigger data */
                if (len == 1 && ((char *)payload)[0] == '1')
                {
                    HAL_GPIO_WritePin(TRIGGER_OUT_GPIO_Port, TRIGGER_OUT_Pin, GPIO_PIN_SET);
                }
                else if (len == 1 && ((char *)payload)[0] == '0')
                {
                    HAL_GPIO_WritePin(TRIGGER_OUT_GPIO_Port, TRIGGER_OUT_Pin, GPIO_PIN_RESET);
                }
            }
            else if (recv_port == SERIAL_SEND_PORT) {
                printf("Received serial data (len=%d): %.*s\n", len, len, (char *)payload);
                /* Handle serial data */
                Serial_Send((uint8_t *)payload, len); // Use Serial_Send to send data via RS232 or RS485
            }

            netbuf_delete(rxbuf);
        }
        else if (recv_err == ERR_TIMEOUT)
        {
            /* Handle timeout */
            continue;
        }
        else
        {
            /* Handle other errors */
            printf("Receive error: %d\n", recv_err);
            break;
        }
    }

    /* Cleanup */
    netconn_delete(conn_recv);
}

/**
  * @brief  Sends trigger data to the server's IP address.
  * @param  data: Pointer to the data to be sent.
  * @param  len: Length of the data.
  * @retval None
  */
void udpsend_trigger(const char *data, size_t len)
{
    err_t err;
    struct netbuf *buf = netbuf_new();

    if (buf == NULL)
    {
        printf("Failed to create netbuf for trigger data\n");
        return;
    }

    /* Reference the data directly without copying */
    if (netbuf_ref(buf, data, len) != ERR_OK)
    {
        printf("Failed to reference trigger data\n");
        netbuf_delete(buf);
        return;
    }

    /* Send trigger data to the server's IP on the receiving port */
    err = netconn_sendto(conn_send_trigger, buf, &server_addr, LOCAL_PORT_RECV);
    if (err != ERR_OK)
    {
        printf("Failed to send trigger data: %d\n", err);
    }

    netbuf_delete(buf);
}

/**
  * @brief  Sends serial data to the server's IP address.
  * @param  data: Pointer to the data to be sent.
  * @param  len: Length of the data.
  * @retval None
  */
void udpsend_serial(const char *data, size_t len)
{
    err_t err;
    struct netbuf *buf = netbuf_new();

    if (buf == NULL)
    {
        printf("Failed to create netbuf for serial data\n");
        return;
    }

    // Allocate space and copy the single byte (or data of size `len`)
    char *udp_data = (char*)netbuf_alloc(buf, len);
    if (udp_data == NULL)
    {
        printf("Failed to allocate space for serial data\n");
        netbuf_delete(buf);
        return;
    }

    memcpy(udp_data, data, len);  // Copy the data (usually 1 byte)

    // Debugging: Log the byte being sent
    printf("Sending serial byte: %.*s\n", (int)len, udp_data);

    // Send the byte to the server's IP on the receiving port
    err = netconn_sendto(conn_send_serial, buf, &server_addr, LOCAL_PORT_RECV);
    if (err != ERR_OK)
    {
        printf("Failed to send serial data: %d\n", err);
    }

    netbuf_delete(buf);
}


/**
  * @brief  Sets the IP address of the UDP server.
  * @param  ip: Pointer to a string containing the IP address.
  * @retval None
  */
void udp_set_server_ip(const char *ip)
{
    /* Update the server IP address dynamically */
    ipaddr_aton(ip, &server_addr);
}

/**
  * @brief  Initializes the UDP client, setting up connections for sending
  *         trigger and serial data, and receiving data on a common port.
  * @retval None
  */
void udpclient_init(void)
{
    err_t err;

    /* Set the server address to 192.168.35.8 */
    IP4_ADDR(&server_addr, 192, 168, 35, 8);

    /* Create netconn for sending trigger data */
    conn_send_trigger = netconn_new(NETCONN_UDP);
    if (conn_send_trigger == NULL)
    {
        printf("Failed to create netconn for sending trigger data\n");
        return;
    }

    /* Bind conn_send_trigger to any available local port */
    err = netconn_bind(conn_send_trigger, IP_ADDR_ANY, TRIGGER_SEND_PORT);
    if (err != ERR_OK)
    {
        printf("Failed to bind conn_send_trigger: %d\n", err);
        netconn_delete(conn_send_trigger);
        return;
    }

    /* Create netconn for sending serial data */
    conn_send_serial = netconn_new(NETCONN_UDP);
    if (conn_send_serial == NULL)
    {
        printf("Failed to create netconn for sending serial data\n");
        netconn_delete(conn_send_trigger);
        return;
    }

    /* Bind conn_send_serial to any available local port */
    err = netconn_bind(conn_send_serial, IP_ADDR_ANY, SERIAL_SEND_PORT);
    if (err != ERR_OK)
    {
        printf("Failed to bind conn_send_serial: %d\n", err);
        netconn_delete(conn_send_trigger);
        netconn_delete(conn_send_serial);
        return;
    }

    /* Create netconn for receiving */
    conn_recv = netconn_new(NETCONN_UDP);
    if (conn_recv == NULL)
    {
        printf("Failed to create netconn for receiving\n");
        netconn_delete(conn_send_trigger);
        netconn_delete(conn_send_serial);
        return;
    }

    /* Bind conn_recv to the fixed local port for receiving both types of data */
    err = netconn_bind(conn_recv, IP_ADDR_ANY, LOCAL_PORT_RECV);
    if (err != ERR_OK)
    {
        printf("Failed to bind conn_recv: %d\n", err);
        netconn_delete(conn_send_trigger);
        netconn_delete(conn_send_serial);
        netconn_delete(conn_recv);
        return;
    }

    /* Start the receiving thread */
    sys_thread_new("udprecv_thread", udprecv_thread, NULL, DEFAULT_THREAD_STACKSIZE, osPriorityNormal);
}
