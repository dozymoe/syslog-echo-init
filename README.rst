----------------
syslog-echo-init
----------------

This is cli program that behaves as such:

1. Will start command arguments passed to it as subprocess and wait until it is
   terminated.

2. Will start syslog listener at 127.0.0.1:514 and echo any message it received to
   stdout.

Example: sudo syslog-echo-init python3 web.py


How to build
------------

The project uses Meson build system, see: https://mesonbuild.com/


References
----------

* The UDP server implementation in C++ is copied from
  https://www.geeksforgeeks.org/udp-server-client-implementation-c/


Third Party Libraries
---------------------

* cqueue (https://github.com/torrentg/cqueue) which was licensed under LGPL v3
