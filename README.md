# cluon-nmea0183

A cluon-based microservice for eavesdropping on a NMEA0183 stream (UDP). This software does not perform any parsing of the NMEA sentences, merely assembles them to full sentences and:
* Publishes them on the OD4 session
* Logs them to disk (daily rotated files)

Each fully received sentence is timestamped unknowingly of the content in the sentence.

## Tests

Unit tests for the NMEA01843 sentence assembler is compiled into the executable `cluon-nmea0183-tester`.

A simple integration test can be done by running the compiled commandline tool (cluon-nmea0183) in conjunction with the provided python script `fake_nmea_0183_stream_udp.py` in the `tests` folder.

## TODO
* Publishing to OD4 session
  * Define a message
  * Update CMake machinery to spit out type definitions of the message
  * Include in code and use in sender
* CD
  * Docker files and setup
  * Where to host images?