CC := gcc
TARGET ?= adsbparser
SRC_DIRS ?= ./msg_parser/src ./parser_application ./ipc_facade/src ./utils/src

SRCS := $(shell find $(SRC_DIRS) -name *.cpp -or -name *.c -or -name *.s)
OBJS := $(addsuffix .o,$(basename $(SRCS)))

#DEPS := $(OBJS:.o=.d)

INC_DIRS := ./msg_parser/inc ./ipc_facade/inc ./utils/inc
INC_FLAGS := $(addprefix -I,$(INC_DIRS))

CPPFLAGS ?= $(INC_FLAGS)  -fPIC
#LDFLAGS += -shared 
LDLIBS += -lpthread -lm

$(TARGET): $(OBJS)
	$(CC) $(LDFLAGS) $(OBJS) -o $@ $(LOADLIBES) $(LDLIBS)

.PHONY: clean
clean:
	$(RM) $(TARGET) $(OBJS) $(DEPS)

