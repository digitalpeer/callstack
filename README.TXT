callstack - Example mutex deadlock protection with callstack printout

This is a simple proof of concept that implements a mutex that has 
deadlock detection and when it detects a deadlock it ptints out the 
callstack of the deadlocked threads.

The guts of the callstack printing are from OpenLieroX by Albert Zeyer.
