#include <dw.h>
#include <fujinet-fuji-pmd85.h>
#include <fujinet-network.h>
#include <fujinet-network-pmd85.h>
#include <string.h>

int16_t network_json_query(const char *devicespec, const char *query, char *s)
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

    br = network_read_nb(devicespec, (uint8_t *)s, ~1);

    if (br <= 0) {
        *s = '\0'; // ensure response is always nul terminated
    }
    else {
        st = s + br - 1;
        if (*st == '\n' || *st == '\0') {
            // replace EOL with nul
            *st = '\0';
            --br;
        }
        else {
            st[1] = '\0'; // room left after data, nul terminate
        }
    }

    return br;
}
