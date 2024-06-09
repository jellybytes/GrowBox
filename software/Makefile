################################################################################
# Makefile XYZ SR
################################################################################

CC = gcc
BIN_DIR = bin
OBJ_DIR = obj

# main
EXE = $(BIN_DIR)/growBoxCtrl
SRC	= $(wildcard src/*.c)
INC = $(wildcard include/*.h)
OBJ = $(patsubst %.c, $(OBJ_DIR)/%.o, $(notdir $(SRC)))
LDLIBS = -lwiringPi -lm -lmosquitto
CFLAGS = -Iinclude

# libs
LIB_ONE = lib/bme280
SRC_LIB_ONE = $(wildcard $(LIB_ONE)/src/*.c)
INC_LIB_ONE = $(wildcard $(LIB_ONE)/include/*.h)
OBJ_LIB_ONE = $(patsubst %.c, $(OBJ_DIR)/%.o, $(notdir $(SRC_LIB_ONE)))
CFLAGS += -I$(sort $(dir $(INC_LIB_ONE)))

LIB_TWO = lib/ssd1306
SRC_LIB_TWO = $(wildcard $(LIB_TWO)/src/*.c)
INC_LIB_TWO = $(wildcard $(LIB_TWO)/include/*.h)
OBJ_LIB_TWO = $(patsubst %.c, $(OBJ_DIR)/%.o, $(notdir $(SRC_LIB_TWO)))
CFLAGS += -I$(sort $(dir $(INC_LIB_TWO)))

LIB_THREE = lib/mqtt_utils
SRC_LIB_THREE = $(wildcard $(LIB_THREE)/src/*.c)
INC_LIB_THREE = $(wildcard $(LIB_THREE)/include/*.h)
OBJ_LIB_THREE = $(patsubst %.c, $(OBJ_DIR)/%.o, $(notdir $(SRC_LIB_THREE)))
CFLAGS += -I$(sort $(dir $(INC_LIB_THREE)))

OBJECTS = $(OBJ) $(OBJ_LIB_ONE) $(OBJ_LIB_TWO) $(OBJ_LIB_THREE)

.PHONY: all clean objs

all: $(EXE)

# Linking
$(EXE): $(OBJECTS) | $(BIN_DIR)
	$(CC) $(CFLAGS) $^ -o $@ $(LDLIBS)

# Compilation
$(OBJ_LIB_ONE): $(SRC_LIB_ONE) $(INC_LIB_ONE) | $(OBJ_DIR)
	$(CC) $(CFLAGS) -c $< -o $@

$(OBJ_LIB_TWO): $(SRC_LIB_TWO) $(INC_LIB_TWO) | $(OBJ_DIR)
	$(CC) $(CFLAGS) -c $< -o $@

$(OBJ_LIB_THREE): $(SRC_LIB_THREE) $(INC_LIB_THREE) | $(OBJ_DIR)
	$(CC) $(CFLAGS) -c $< -o $@

$(OBJ): $(SRC) $(INC) | $(OBJ_DIR)
	$(CC) $(CFLAGS) -c $< -o $@

# Utility
$(BIN_DIR) $(OBJ_DIR):
	mkdir -p $@

clean:
	@$(RM) -rv $(BIN_DIR) $(OBJ_DIR)