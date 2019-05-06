###############################################################
# @file    Makefile                                           #
# @author  Filip Kocica                                       #
# @date    05/10/2018                                         #
#                                                             #
# Dataset generator makefile                                  #
###############################################################

################## Build constants ##############

BIN_NAME   = datagen

DOC        = doc
SRC        = src

CC         = g++
CFLAGS     = -std=c++17
LDFLAGS    = `pkg-config --libs opencv` -lstdc++fs
DOXYGEN    = doxygen
CFG        = cfg

HEADERS    = $(wildcard $(SRC)/*.h)
SOURCES    = $(wildcard $(SRC)/*.cpp)
OBJS       = $(patsubst %.cpp, %.o, $(SOURCES))

.PHONY: all



# =================== Data used selection =====================

# Leave in case of using transparent images with alpha channel
# otherwise if using cropped images -- comment out this line
CFLAGS    += -DIMG_TRANSPARENT



# ================== General configuration ====================

# Generated image has random Width and Height
#CFLAGS    += -DRANDOM_W_H

# Selection of ROIs where signs are placed
#CFLAGS    += -DROI_SELECTION

# The closer to middle of image TS is, the smaller it is
#CFLAGS    += -DREALISTIC_SIZE

# Create annotation file for each generated image
CFLAGS    += -DANNOTATION

# Debug info, show annotation bounding-boxes
#CFLAGS    += -DGENERATOR_DEBUG



# ======================== Transparent ========================

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

# Additive Gaussian noise
CFLAGS    += -DGAUSSNOISE

# Additive Salt and pepper noise
CFLAGS    += -DPEPPERNOISE

# Traffic sign brightness gradient
CFLAGS    += -DGRADIENT

# Modify luminescence of the traffic sign according to brightness in the image
CFLAGS    += -DREALISTIC_LIGHTNING

# Add partial traffic sign to dataset
CFLAGS    += -DPARTIAL_TS



# ========================== Cropped ==========================

# Seamless cloning of cropped signs to background
CFLAGS    += -DSEAMLESS_CLONE



# ======================== Compilation ========================

all: $(BIN_NAME)

$(BIN_NAME): $(HEADERS) $(SOURCES) $(OBJS)
	$(CC) $(CFLAGS) $(OBJS) -o $@ $(LDFLAGS)

%.o: %.cpp
	$(CC) $(CFLAGS) -c $< -o $@



# ======================= Doc/Clean/Run =======================

.PHONY: all

# Clean compiled objects and binary file
clean:
	rm -f $(BIN_NAME) $(SRC)/*.o

# Clean output folder (generated images & annotations)
cleanout:
	rm -rf out/*

# Run transparent generator
runt:
	./$(BIN_NAME) data/backgrounds/ data/transparent

# Run cropped generator
runc:
	./$(BIN_NAME) data/backgrounds/ data/cropped

# Generate code documentation
$(DOC):
	mkdir -p $(DOC)
	$(DOXYGEN) $(CFG)/doxyConf