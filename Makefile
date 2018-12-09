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
CFLAGS     = -std=c++17
LDFLAGS    = `pkg-config --libs opencv` -lstdc++fs

HEADERS    = $(wildcard $(SRC)/*.h)
SOURCES    = $(wildcard $(SRC)/*.cpp)
OBJS       = $(patsubst %.cpp, %.o, $(SOURCES))

#CFLAGS    += -DRANDOM_W_H         # Generated image has random Width and Height
CFLAGS    += -DIMG_CROPPED        # Using cropped images with alpha channel
#CFLAGS    += -DROI_SELECTION      # Selection of ROIs where signs are generated
#CFLAGS    += -DGENERATOR_DEBUG    # Debug info, show annotation bounding-boxes
#CFLAGS    += -DBLUR               # Blue traffic signs
#CFLAGS    += -DROTATE_XY          # Rotate TS XY
#CFLAGS    += -DROTATE_Z           # Rotate TS Z
CFLAGS    += -DLUMINESCENCE       # Modify luminescence of inserted signs

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
