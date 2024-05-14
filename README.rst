----------------
syslog-echo-init
----------------

This is cli program that behaves as such:

1. Will start command arguments passed to it as subprocess and wait until it is
   terminated.

2. Will start syslog listener at 127.0.0.1:514 and echo any message it received to
   stdout.

Example: sudo syslog-echo-init python3 web.py
