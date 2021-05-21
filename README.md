# cluon-nmea0183

A cluon-based microservice for eavesdropping on a NMEA0183 stream (UDP or TCP). This software does not perform any parsing of the NMEA sentences, merely assembles them to full sentences and:
* Publishes them on the OD4 session
* Logs them to disk (daily rotated files)

Each fully received sentence is timestamped unknowingly of the content in the sentence.

## TODO
* Publishing to OD4 session
  * Define a message
  * Update CMake machinery to spit out type definitions of the message
  * Include in code and use in sender
* CD
  * Docker files and setup
  * Where to host images?