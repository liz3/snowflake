# Snowflake
C implementation of a simple socket server replying with unique ids based on Twitters snowflake system.
This might be used later in our services should it become necessary.

Binary
```
111111111111111111111111111111111111111111 11111 11111 111111111111
64                                         22    17    12          0
```
Left to right:
* first 42 bits: amount of ms since epoch (start od 2015), retrieval: `(value >> 22) + 1420070400000`
* next 5 bits: region id(defined during compile time), retrieval: `(value & 0x3E0000) >> 17`
* next 5 bits: worker id(defined during compile time), retrieval: `(value & 0x1F000) >> 12`
* last 12 bits: For every ID that is generated on that process, this number is incremented, retrieval: `snowflake & 0xFFF`