/* See LICENSE file for copyright and license details. */
static const unsigned int mincpu      = 20;   /* percentage of cpu usage where turbo boost is enabled */
static const unsigned int mintemp     = 70;   /* degrees celsius where turbo boost is enabled */
static const unsigned int interval    = 5;    /* seconds to wait before running */
static const unsigned int alwaysturbo = 0;    /* 1 means never disables turbo boost */

/* available governors: cat /sys/devices/system/cpu/cpu0/cpufreq/scaling_available_governors */

/* default governor to use when using the battery */
static const char batgovernor[] = "powersave";
/* default governor to use when AC adapter is connected */
static const char acgovernor[]  = "performance";
