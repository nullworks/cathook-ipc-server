CXX=g++
CXXFLAGS=-std=gnu++14 -O3 -s -Wall -fmessage-length=0 -m32 -DSILENT=1
SIMPLE_IPC_DIR = "$(realpath simple-ipc/include/SimpleIPC)"
INCLUDES=-I$(SIMPLE_IPC_DIR)
CXXFLAGS += $(INCLUDES)
LDFLAGS=-m32
LDLIBS=-lpthread -lrt -lboost_system
SRC_DIR = src
OUT_DIR = bin
SOURCES = $(shell find $(SRC_DIR) -name "*.cpp" -print)
SOURCES += $(shell find $(SIMPLE_IPC_DIR) -name "*.cpp" -print)
DEPENDS = $(SOURCES:.cpp=.d)
TARGETS=server exec undeadlock exec_all console
INSTALL_DIR = /opt/cathook/ipc

SOURCES := $(filter-out $(patsubst %,src/%.cpp,$(TARGETS)),$(SOURCES))

OBJECTS = $(SOURCES:.cpp=.o)

$(info $(SOURCES))

.PHONY: clean directories

all:
	mkdir -p $(OUT_DIR)
	$(MAKE) $(addprefix bin/,$(TARGETS))

install:
	sudo mkdir -p $(INSTALL_DIR)
	sudo cp -R $(OUT_DIR) $(INSTALL_DIR)

.cpp.o:
	$(CXX) $(CXXFLAGS) -c $< -o $@
	
$(OUT_DIR)/%: $(SRC_DIR)/%.o $(OBJECTS)
	$(CXX) $(CXXFLAGS) $(LDFLAGS) $^ $(LDLIBS) -o $@

clean:
	find . -type f -name '*.o' -delete
	find . -type f -name '*.d' -delete
	rm -rf ./bin
