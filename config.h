/* See LICENSE file for copyright and license details. */
static const unsigned int interval = 15;  /* seconds to wait before running */
/* Usual avaliable governors:
 * 	performance - powersave - userspace - ondemand - conservative - schedutil
 * Check supported govenors:
 * 	cat /sys/devices/system/cpu/cpu0/cpufreq/scaling_available_governors
 *
 * NOTE FOR INTEL USERS: (modern) intel CPUs have additional governors which
 * are preferable over the cpufreq's ones. These 'intel specific' suggested
 * governors are: "balance_performance" and "balance_power".
 * Check if the path /sys/devices/system/cpu/intel_pstate/hwp_dynamic_boost exist */

/* default governor to use when using the battery */
static const char batgovernor[] = "powersave";

/* default governor to use when AC adapter is connected */
static const char acgovernor[]  = "performance";

/* Always: turbo boost will always be on.
 * Never:  turbo boost will never be on.
 * Auto:   depending on the cpu usage, system load and temperature, turbo boost
 *         will be on/off. */
static const unsigned int batturbo = Auto; /* value to use in battery */
static const unsigned int acturbo  = Auto; /* value to use in AC */

/* values used in Auto mode (turbo boost) */
static const unsigned int mincpu  = 20;    /* percentage of cpu usage where turbo boost is enabled */
static const unsigned int mintemp = 70;    /* degrees celsius where turbo boost is enabled */
/* Path to the sensors, in order to fetch the overall temperature.
 * Usually at /sys/class/thermal/ */
static const char thermal[] = "/sys/class/thermal/thermal_zone0/temp";
