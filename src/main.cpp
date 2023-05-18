/**
 * Copyright (c) 2022 Raspberry Pi (Trading) Ltd.
 *
 * SPDX-License-Identifier: BSD-3-Clause
 */

#include "btstack_run_loop.h"
#include "pico/stdlib.h"
#include "bluetooth/common.h"
#include "hardware/vreg.h"

#include "bt_display.hpp"

int main() {
    //vreg_set_voltage(VREG_VOLTAGE_1_20);
    //sleep_ms(10);
    //set_sys_clock_khz(266000, true);

    stdio_init_all();

    //sleep_ms(4000);

    bt_display_init();

    int res = picow_bt_example_init();
    if (res){
        return -1;
    }

    picow_bt_example_main();
    btstack_run_loop_execute();
}
