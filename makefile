# make [all | clean] [DEBUG=1] [TEST=1]

# Rules:
#   all     - builds the application (default rule)
#   clean   - cleans the application build
#   vscode  - add a build configuration to VS Code 

# Options:
#   DEBUG=1 - enables a debug build of the application (default is a release build with no debug information)
#   TEST=1  - builds the unit test application

APP_NAME := jsoncfg

# Some useful character constants
NULL  :=
SPACE := $(NULL) $(NULL)
COMMA := ,

# @brief A function to escape characters in a text string
# @param $(1) The text string to escape
# @return The escaped text
# @note This will escape the '"', '$' and '\' characters (with a '\' character prefix)
escape = $(subst $$,\$$,$(subst ",\",$(subst \,\\,$(1))))

# @brief Escapes a text item and marks it as an item in a list
# @param $(1) The text item to escape and mark as a list item
# @return The list item
# @note The text item must not contain a '¬' character (as this is used for marking the list item).
list_add = ¬$(call escape,$(1))¬¬

# @brief Gets the text of items that are in an item list
# @param $(1) The text to prefix each item text
# @param $(2) The item list
# @param $(3) The text to suffix each item text
# @return The list of text items
# @note By using list_add() and list_get() you can manipulate text items that have any spaces in them.
# @note This would normally be quite difficult using the normal text functions as these use the space to separate out each text item.
list_get = $(patsubst ¬%,$(1)%,$(patsubst %¬¬,%$(3),$(patsubst ¬%¬¬,$(1)%$(3),$(2))))

# @brief Gets the comma separated list of text items from an item list
# @param $(1) The item list
# @return The comma separated list of text items
# @note Each text item will be escaped and surrounded by quotes (").
# @note By using list_add() and list_get() you can manipulate text items that have any spaces in them.
# @note This would normally be quite difficult using the normal text functions as these use the space to separate out each text item.
list_get_csv = $(filter-out $(COMMA)¬,$(call escape,$(call list_get,",$(1)," $(COMMA)))¬)

SRC_DIR := $(call list_add,./source/json)
ifeq ($(TEST),1)
APP_NAME := $(APP_NAME)-test
SRC_DIR += $(call list_add,./source/test)
else
SRC_DIR += $(call list_add,./source/app)
endif
ifeq ($(DEBUG),1)
BIN_DIR := ./bin/debug
else
BIN_DIR := ./bin/release
endif
OBJ_DIR := $(BIN_DIR)/obj

VPATH := $(call list_get,,$(SRC_DIR),)

C_FILES := $(wildcard $(addsuffix /*.c,$(call list_get,,$(SRC_DIR),)))
O_FILES := $(addprefix $(OBJ_DIR)/,$(notdir $(C_FILES:.c=.o)))
D_FILES := $(O_FILES:.o=.d)

ifeq ($(DEBUG),1)
BUILD_NAME := Debug
C_DEFINE += $(call list_add,DEBUG)
C_FLAGS += -g3
else
BUILD_NAME := Release
C_DEFINE += $(call list_add,RELEASE)
C_FLAGS += -O2
endif

C_DEFINE += $(call list_add,APP_NAME="$(APP_NAME) ($(BUILD_NAME))")
C_FLAGS += -c -std=c11 -Wall -Werror $(call list_get,-I",$(SRC_DIR),") $(call list_get,-D",$(C_DEFINE),")

BUILD_CFG := ./.vscode/c_cpp_properties.json
BUILD_DEFINE := $(call list_get_csv,$(C_DEFINE))
BUILD_INCLUDE := $(call list_get_csv,$(SRC_DIR) $(call list_add,$${workspaceFolder}/**))

ifeq ($(CC),cc)
CC := gcc
endif

APP := $(BIN_DIR)/$(APP_NAME)

all: $(APP)

clean:
	rm -f $(APP)
	rm -f $(O_FILES) $(D_FILES)

vscode:
	mkdir -p ./.vscode
	touch $(BUILD_CFG)
	cat $(BUILD_CFG) | ./bin/release/jsoncfg /configurations[/name:\"$(BUILD_NAME)\"] \
	    "{ \
            \"name\":\"$(BUILD_NAME)\", \
            \"includePath\":[ $(BUILD_INCLUDE) ], \
            \"defines\":[ $(BUILD_DEFINE) ] \
        }" >$(BUILD_CFG).tmp
	mv $(BUILD_CFG).tmp $(BUILD_CFG)


$(APP) : $(O_FILES)
	@echo -- BUILDING $(NAME)
	mkdir -p $(@D)
	$(CC) $(O_FILES) $(LNK_FLAGS) -o $@

-include $(D_FILES)

$(OBJ_DIR)/%.o : %.c
	@echo -- COMPILING $<
	mkdir -p $(@D)
	$(CC) $< $(C_FLAGS) -MMD -o $@
