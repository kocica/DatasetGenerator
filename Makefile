###############################################################
# @file    Makefile                                           #
# @author  Filip Kocica                                       #
# @date    05/10/2018                                         #
#                                                             #
# Dataset generator makefile                                  #
###############################################################

################## Build constants ##############

BIN_NAME   = dataset-generator

DOC        = doc
SRC        = src

CC         = g++
CFLAGS     = -std=c++11
LDFLAGS    = `pkg-config --libs opencv`

HEADERS    = $(wildcard $(SRC)/*.h)
SOURCES    = $(wildcard $(SRC)/*.cpp)
OBJS       = $(patsubst %.cpp, %.o, $(SOURCES))

################## Compilation ##################

all: $(BIN_NAME)

$(BIN_NAME): $(HEADERS) $(SOURCES) $(OBJS)
	$(CC) $(CFLAGS) $(OBJS) -o $@ $(LDFLAGS)

%.o: %.cpp
	$(CC) $(CFLAGS) -c $< -o $@

################## Pack/Clean ##################

.PHONY: clean


clean:
	rm -f $(BIN_NAME) $(SRC)/*.o
