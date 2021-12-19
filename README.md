scaf - simple auto cpu frequencer
=================================
sacf it's a daemon that monitors the cpu usage, system load and the temperature
over 5 seconds, by default, and simply tweak governors and decides whether to
use turbo boost or not.

Usage
-----
With no arguments it runs as daemon.

    sacf [-ltTv] [-g governor]


Installation
------------
Edit config.mk to match your local setup (sacf is installed into
the /usr/local namespace by default).

Afterwards enter the following command to build and install sacf
(if necessary as root):

    make clean install


TODO
====
- freebsd and openbsd support
- man page
- save the turbo state into a variable instead of opening /sys every time
