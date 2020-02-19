# make [all | clean] [DEBUG=1] [TEST=1]

# Rules:
#   all     - builds the application (default rule)
#   clean   - cleans the application build
#   vscode  - add a build configuration to VS Code 

# Options:
#   DEBUG=1 - enables a debug build of the application (default is a release build with no debug information)
#   TEST=1  - builds the unit test application

APP_NAME := jsoncfg

NULL  :=
SPACE := $(NULL) #
COMMA := ,

SRC_DIR := ./source/json
ifeq ($(TEST),1)
APP_NAME := $(APP_NAME)-test
SRC_DIR += ./source/test
else
SRC_DIR += ./source/app
endif
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

BUILD_CFG := ./.vscode/c_cpp_properties.json

ifeq ($(DEBUG),1)
BUILD_NAME := Debug
C_DEFINE += DEBUG
C_FLAGS += -g3
else
BUILD_NAME := Release
C_DEFINE += RELEASE
C_FLAGS += -O3
endif
C_DEFINE += APP_NAME="$(APP_NAME)"

C_FLAGS += -c -Wall -Werror $(addprefix -I,$(SRC_DIR)) $(addprefix -D,$(subst ","\",$(C_DEFINE)))


APP := $(BIN_DIR)/$(APP_NAME)

all: $(APP)

clean:
	rm -f $(APP)
	rm -f $(O_FILES) $(D_FILES)

vscode:
	touch $(BUILD_CFG)
	cat $(BUILD_CFG) | ./bin/release/jsoncfg /configurations[/name:\"$(BUILD_NAME)\"] \
	    "{ \
			\"name\":\"$(BUILD_NAME)\", \
            \"includePath\":[\"\$${workspaceFolder\}/**\"], \
            \"defines\":[ $(subst $(SPACE),$(COMMA)$(SPACE),$(patsubst %,\"%\",$(subst ",\\\",$(C_DEFINE)))) ], \
            \"compilerPath\":\"/usr/bin/gcc\", \
            \"cStandard\":\"c11\", \
            \"cppStandard\":\"c++17\", \
            \"intelliSenseMode\":\"clang-x64\" \
        }" >$(BUILD_CFG).tmp
	mv $(BUILD_CFG).tmp $(BUILD_CFG)


$(APP) : $(O_FILES)
	@echo -- BUILDING $(NAME)
	mkdir -p $(@D)
	gcc $(O_FILES) $(LNK_FLAGS) -o $@

-include $(D_FILES)

$(OBJ_DIR)/%.o : %.c
	@echo -- COMPILING $<
	mkdir -p $(@D)
	gcc $< $(C_FLAGS) -MMD -o $@
