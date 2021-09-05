#include <iostream>
#include <string>
#include <vector>
#include <memory>
#include <dlt_header.h>

namespace auto_os::middleware {

int dlt_standard_header::encode(uint8_t *buff, size_t buff_size, size_t &off)
{
    return -1;
}

int dlt_standard_header::decode(uint8_t *buff, size_t buff_size, size_t &off)
{
    return -;
}

}
