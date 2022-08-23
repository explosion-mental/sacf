/* See LICENSE file for copyright and license details. */
static const unsigned int mincpu      = 20;   /* percentage of cpu usage where turbo boost is enabled */
static const unsigned int mintemp     = 70;   /* degrees celsius where turbo boost is enabled */
static const unsigned int interval    = 15;   /* seconds to wait before running */
static const unsigned int alwaysturbo = 0;    /* 1 means never disables turbo boost */

/* (usual) governors:
 * 	performance - powersave - userspace - ondemand - conservative - schedutil
 * for supported govenors:
 * 	cat /sys/devices/system/cpu/cpu0/cpufreq/scaling_available_governors
 * NOTE FOR INTEL USERS: (modern) intel cpu's have additional governors which
 * are preferable over the cpufreq's ones. These 'intel specific' suggested governors are:
 * "balance_performance" and "balance_power". To check if you can enable these, check
 * if /sys/devices/system/cpu/intel_pstate/hwp_dynamic_boost path exist */

/* default governor to use when using the battery */
static const char batgovernor[] = "powersave";

/* default governor to use when AC adapter is connected */
static const char acgovernor[]  = "performance";
