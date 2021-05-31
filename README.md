# cluon-nmea0183

Copyright 2021 RISE Research Institute of Sweden. All rights reserved. For details, please contact Fredrik Olsson (fredrik.x.olsson@ri.se)

A cluon-based microservice for eavesdropping on a NMEA0183 stream (UDP). This software does not perform any parsing of the NMEA sentences, merely assembles them to full sentences. It can be run in two modes:
* `gather`, connect to an UDP stream of incoming NMEA0183 sentences and either:
  * publish to an OD4 session, or
  * log directly to disk (`--standalone`)
* `log`, listen to an OD4 session for raw NMEA0183 messages from other `gatherers`  and dump these to an aggregated log file on disk


## Tests

Unit tests for the NMEA01843 sentence assembler is compiled into the executable `cluon-nmea0183-tester`.

A simple integration test can be done by running the compiled commandline tool (cluon-nmea0183) in conjunction with the provided python script `fake_nmea_0183_stream_udp.py` in the `tests` folder.

## TODO
* CD
  * Docker files and setup
  * Where to host images?
