###############################################################
# @file    Makefile                                           #
# @author  Filip Kocica                                       #
# @date    05/10/2018                                         #
#                                                             #
#                                                             #
###############################################################

################## Build constants ##############
BIN_NAME   = dataset-generator

DOC        =

CC         = g++
CFLAGS     = -std=c++14

LDFLAGS    =
LIBS       =

SRC        = .

HEADERS    = $(wildcard $(SRC)/*.hpp)
SOURCES    = $(wildcard $(SRC)/*.cpp)
OBJS       = $(patsubst %.cpp, %.o, $(SOURCES))

################## Compilation ##################

all: $(BIN_NAME)

$(BIN_NAME): $(HEADERS) $(SOURCES) $(OBJS)

%.o: %.cpp %.hpp
	$(CC) $(CFLAGS) -c $< -o $@

################## Pack/Clean ##################

.PHONY: clean


clean:
	rm -f $(BIN_NAME) $(SRC)/*.o
