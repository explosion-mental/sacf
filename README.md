scaf - simple auto cpu frequencer
=================================
sacf it's a simple program that checks the cpu usage, system load and the
temperature every 15 seconds (by default), and tweak turbo boost if needed.

Depending on the AC state, charging or using the battery, sacf will change the
[scaling governor](https://wiki.archlinux.org/title/CPU_frequency_scaling#Scaling_governors).

Usage
-----
**sacf** will run only if `/var/run/sacf.lock` doesn't exist, created when run. This
is to avoid multiple instances of **sacf** run at the same time.
The lock is **ignored** if any cli option is used but `-b` or `--daemon` is used.

With no arguments sacf will start to run. In order to enable/disable turbo
boost, sacf will require permissions to write on /sys/.

    sacf [-bltTv] [-g governor]

For a more detail information read the man page.

Installation
------------
Edit config.mk to match your local setup (sacf is installed into
the /usr/local namespace by default).

Afterwards enter the following command to build and install sacf
(if necessary as root):

    make clean install


Credits
=======
Thanks to the author of
[auto-cpufreq](https://github.com/AdnanHodzic/auto-cpufreq) for the solution to
linux + battery + performance use case.

TODO
====
- freebsd and openbsd support
