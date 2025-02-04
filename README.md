# Hack 1.0.3, ANSIfied

Most global prototypes now in hack.h.

Made static anything that could be.

Converted varargs functions to use stdarg.h.

Signal handlers take an int, return void.

Most remaining warnings are char subscripts.

Y2K fix for writing record.

Cross builds for Fuzix (see "fuzix" branch for some sample build rules), but
_(WAY)_ too big for the 6809 memory map.  Who knows, may just work on 68K?
Almost certainly would need more work to adapt to the native compiler
(hopefully only tool invocation...).  Tested under Linux with xterm in VT52
mode.
