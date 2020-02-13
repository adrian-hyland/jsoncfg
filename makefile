# make [all | clean] [DEBUG=1]

# Rules:
#   all     - builds the application (default rule)
#   clean   - cleans the application build

# Options:
#   DEBUG=1 - enables a debug build of the application (default is a release build with no debug information)

APP_NAME := jsoncfg

SRC_DIR := ./source/json ./source/app
ifeq ($(DEBUG),1)
BIN_DIR := ./bin/debug
else
BIN_DIR := ./bin/release
endif
OBJ_DIR := $(BIN_DIR)/obj

VPATH := $(SRC_DIR)

C_FILES := $(wildcard $(addsuffix /*.c,$(SRC_DIR)))
O_FILES := $(addprefix $(OBJ_DIR)/,$(notdir $(C_FILES:.c=.o)))
D_FILES := $(O_FILES:.o=.d)

ifeq ($(DEBUG),1)
C_DEFINE += DEBUG
C_FLAGS += -g3
else
C_DEFINE += DEBUG
C_FLAGS += -O3
endif

C_FLAGS += -c -Wall -Werror $(addprefix -I,$(SRC_DIR)) $(addprefix -D,$(C_DEFINE))


APP := $(BIN_DIR)/$(APP_NAME)

all: $(APP)

clean:
	rm -f $(APP)
	rm -f $(O_FILES) $(D_FILES)

$(APP) : $(O_FILES)
	@echo -- BUILDING $(NAME)
	mkdir -p $(@D)
	gcc $(O_FILES) $(LNK_FLAGS) -o $@

-include $(D_FILES)

$(OBJ_DIR)/%.o : %.c
	@echo -- COMPILING $<
	mkdir -p $(@D)
	gcc $< $(C_FLAGS) -MMD -o $@
