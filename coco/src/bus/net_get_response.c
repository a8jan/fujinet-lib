#include <cmoc.h>
#include <coco.h>
#include <dw.h>
#include <fujinet-fuji-coco.h>

/**
 * @brief Get response data from last command
 * @param devid The device ID (0-255) 
 * @param buf Target buffer 
 * @param len Length 
 */

uint8_t net_get_response(uint8_t devid, uint8_t *buf, int len)
{
    /* Get response from Network Device OP_NET,devid */
    struct _getnetresponse
    {
        uint8_t opcode;
        uint8_t id;
        uint8_t command;
        uint16_t len;
    } grc;

    grc.opcode = OP_NET;
    grc.id = devid;
    grc.command = FUJICMD_GET_RESPONSE; 
    grc.len = len;

    bus_ready();
    dwwrite((byte *)&grc, sizeof(grc));
    
    /* Return 0 on a successful read (dwread returns 1) */
    return !dwread((byte *)buf, len);
}
