# nfc-iot-pairing
A proof of concept implementation for pairing IoT devices via NFC

Tested on two raspberry pi:s on the same network, communicating using two pn532 based grove nfc modules.

In order to use this software, two different versions need to be compiled, one being the target and the
other being the initiator. By changing the variable bool deviceIsInitiator in generateEncryptionKeys.cpp,
the role can be selected for the version you are compiling. Once you have the two versions, one raspberry
pi needs to run the initiator version, and the other needs to run the target version of the software.
Due to the prototype nature of this software, the target version needs to be started before the initiator
is started, since the initiator will otherwise time out when it fails to detect a target. The nfc antennas
also need to be within range of each other when the initiator version is started.
