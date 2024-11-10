SERVER_SRC = server.cpp
CLIENT_SRC = client.cpp
SERVER_TARGET = ./target/serwer
CLIENT_TARGET = ./target/klient
TARGET_DIR = ./target
KEY_FILE = key

CXX = g++
CXXFLAGS = -Wall -O2

# Default target
all: $(TARGET_DIR) $(SERVER_TARGET) $(CLIENT_TARGET)

$(TARGET_DIR):
	mkdir -p $(TARGET_DIR)

# server.cpp -> ./target/serwer
$(SERVER_TARGET): $(SERVER_SRC) | $(TARGET_DIR)
	$(CXX) $(CXXFLAGS) -o $@ $(SERVER_SRC)

# client.cpp -> ./target/klient
$(CLIENT_TARGET): $(CLIENT_SRC) | $(TARGET_DIR)
	$(CXX) $(CXXFLAGS) -o $@ $(CLIENT_SRC)

$(KEY_FILE):
	touch $(KEY_FILE)

clean:
	rm -rf $(TARGET_DIR)
