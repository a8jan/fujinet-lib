#include <dw.h>
#include <fujinet-fuji-pmd85.h>
#include <fujinet-network.h>
#include <fujinet-network-pmd85.h>
#include <string.h>

#define MIN(a, b) ((a) < (b) ? (a) : (b))

int16_t network_json_query_n(const char *devicespec, const char *query, char *s, uint16_t maxlen)
{
    struct _jq
    {
        uint8_t opcode;
        uint8_t unit;
        uint8_t cmd;
        uint8_t aux1;
        uint8_t aux2;
        char query[256];
    } jq;

    uint8_t r = 0;
    uint16_t fetch_size = 0;
    uint8_t unit = network_unit(devicespec);

    int16_t br;
    char *st;

    jq.opcode = OP_NET;
    jq.unit = unit;
    jq.cmd = 'Q';
    jq.aux1 = jq.aux2 = 0;
    strcpy(jq.query, query);

    bus_ready();
    dwwrite((uint8_t *)&jq, sizeof(jq));

    br = network_read_nb(devicespec, (uint8_t *)s, maxlen);

    if (br <= 0) {
        *s = '\0'; // ensure response is always nul terminated
    }
    else {
        st = s + br - 1;
        if (*st == '\n' || *st == '\0' || (uint16_t)br == maxlen) {
            // replace EOL with nul, or drop last char if buffer is full
            *st = '\0';
            --br;
        }
        else {
            st[1] = '\0'; // room left after data, nul terminate
        }
    }

    // // consume any remaining bytes in the buffer to avoid leaving data for the next query
    // r = network_status(devicespec, &fn_network_bw, &fn_network_conn, &fn_network_error);
    // // check if the status failed.
    // if (r != 0) {
    //     return -r;

    // }

    // no need to call extra status
    // we can just use the fn_network_bw value updated by previous network_read_nb call
    while (fn_network_bw > 0) {
        // use jq.query as a temporary buffer to read and discard the remaining data
        fetch_size = MIN(sizeof(jq.query), fn_network_bw);
        if (network_read_pmd85(devicespec, jq.query, fetch_size) != FN_ERR_OK) {
            break; // stop if there is an error reading
        }
        fn_network_bw -= fetch_size;
    }

    return br;
}
