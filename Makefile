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

# Generated image has random Width and Height
#CFLAGS    += -DRANDOM_W_H

# Using transparent images with alpha channel
CFLAGS    += -DIMG_TRANSPARENT

# Selection of ROIs where signs are generated
#CFLAGS    += -DROI_SELECTION

# Debug info, show annotation bounding-boxes
CFLAGS    += -DGENERATOR_DEBUG

# Blue traffic signs
CFLAGS    += -DBLUR

# Rotate TS in Y axis
CFLAGS    += -DROTATE_Y

# Rotate TS in Z axis
CFLAGS    += -DROTATE_Z

# Modify brightness and contrast of inserted signs
CFLAGS    += -DBIGHTCONTRAST

# Correction of gamma channel of inserted signs
CFLAGS    += -DGAMMACORRECT

# Modify hue of inserted signs
CFLAGS    += -DHUE

# Create annotation file for each generated image
CFLAGS    += -DANNOTATION

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

run:
	./$(BIN_NAME) ./../Backgrounds/ ./data/