CXX = g++
CXXFLAGS = -std=c++0x -Wall

OBJECTS = generateEncryptionKeys.o mathFunctions.o unitTests.o encryptionFunctions.o udpBroadcast.o nfc-dep-initiator.o nfc-dep-target.o

main: $(OBJECTS)
	$(CXX) $(CXXFLAGS) -o $@ $^ -lgmp -pthread -lnfc

$(OBJECTS): mathFunctions.h unitTests.h encryptionFunctions.h udpBroadcast.h nfc-dep-target.h nfc-dep-initiator.h