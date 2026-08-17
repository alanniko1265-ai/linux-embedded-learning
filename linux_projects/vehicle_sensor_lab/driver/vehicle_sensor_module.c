// SPDX-License-Identifier: GPL-2.0

#include <linux/init.h>
#include <linux/module.h>
#include <linux/printk.h>

#define VEHICLE_SENSOR_NAME "vehicle_sensor"
#define VEHICLE_SENSOR_VERSION "0.1.0"

static int __init vehicle_sensor_init(void)
{
    /* TODO(Day55-1): 使用 pr_info 打印模块名称、版本和 loaded。 */

    return 0;
}

static void __exit vehicle_sensor_exit(void)
{
    /* TODO(Day55-2): 使用 pr_info 打印模块名称和 unloaded。 */
}

module_init(vehicle_sensor_init);
module_exit(vehicle_sensor_exit);

MODULE_LICENSE("GPL");
MODULE_AUTHOR("TODO: write your name");
MODULE_DESCRIPTION("Vehicle sensor lab minimal kernel module");
MODULE_VERSION(VEHICLE_SENSOR_VERSION);

