#include "bsp.h"

uint8_t bsp_GetAddress(void)
{
        uint8_t addr = ~((HAL_GPIO_ReadPin(DIPSW8_GPIO_Port, DIPSW8_Pin) << 3) |
                             (HAL_GPIO_ReadPin(DIPSW4_GPIO_Port, DIPSW4_Pin) << 2) |
                             (HAL_GPIO_ReadPin(DIPSW2_GPIO_Port, DIPSW2_Pin) << 1) |
                             HAL_GPIO_ReadPin(DIPSW1_GPIO_Port, DIPSW1_Pin)) & 0x0F;
        bsp_println("Read Address: %d", addr);
        return addr;
}