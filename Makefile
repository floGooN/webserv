#==============================================================================#
#							  SETTING VARIABLES								   #
#==============================================================================#

RESET		= \033[0m
GREEN		= \033[32m
YELLOW		= \033[33m

#------------------------# ==== EXECUTABLE NAME ==== #-------------------------#
NAME		= webserv

#------------------------# ==== DEFAULT COMPILER ==== #------------------------#
CXX			= c++

#---------------------------# ==== SHELL CMDS ==== #---------------------------#
RM		  = rm -rf
MKD			= mkdir -p
VALGRIND	= valgrind --leak-check=full 

#-----------------------# ==== SOURCE CODE DIRECTORIES ==== #------------------#
INCLUDES	=	./sources/includes ./sources ./sources/configBuilder ./sources/parsing \
				./sources/errorManagement
SRC_PATH	= sources

#---------------------# ==== TEMPORARY FILES DIRECTORIES ==== #----------------#
TEMP_PATH	= .temp
OBJ_PATH	= $(TEMP_PATH)/obj
DEP_PATH	= $(TEMP_PATH)/dep

#-----------------------# ==== COMPILATION FLAGS ==== #------------------------#
MODE		?= release

ifeq ($(MODE), debug)
	COMPFLAGS = -g3 -Wall -Wextra -Wshadow -Wpedantic -std=c++98 -D=TEST
else
	COMPFLAGS = -O -Wall -Wextra -Werror -Wshadow -Wpedantic -std=c++98
endif

DEPFLAGS	= -MM -MT $@ $< -MF $(DEP_PATH)/$*.d

#------------------------# ==== MANDATORY FILES ==== #-------------------------#
SRC = $(shell find $(SRC_PATH) -type f -name "*.cpp"  | sed 's|^\./||')

#------------------------# ==== TEMPORARY FILES ==== #-------------------------#
DEP	= $(patsubst $(SRC_PATH)/%.cpp,$(DEP_PATH)/%.d,$(SRC))
OBJ	= $(patsubst $(SRC_PATH)/%.cpp,$(OBJ_PATH)/%.o,$(SRC))

DEP_DIRS = $(sort $(dir $(DEP)))

#==============================================================================#
#							COMPILATION MANDATORY							 #
#==============================================================================#
default: all

all: $(NAME)
	@echo
	@echo "$(GREEN)***************** $(NAME) is READY *****************$(RESET)"
	@echo

debug:
	$(MAKE) MODE=debug
	@echo
	@echo "$(GREEN)************* $(NAME) is in debug mode *************$(RESET)"
	@echo

test:
	$(MAKE) MODE=debug
	$(VALGRIND) --track-fds=yes ./$(NAME) 
#-------------------# ==== LINKING & BUILDING PROGRAM ==== #-------------------#
$(NAME)	: $(OBJ)
	@echo "$(GREEN)-- compilation completed --$(RESET)"
	$(CXX) $^ -o $@
	@echo "$(GREEN)-- linking & building completed --$(RESET)"

#--------------------# ==== COMPILATION OBJ - DEPS ==== #----------------------#
$(OBJ_PATH)/%.o : $(SRC_PATH)/%.cpp Makefile
	@$(MKD) $(dir $@) $(dir $(patsubst $(OBJ_PATH)/%.o,$(DEP_PATH)/%.d,$@))
	$(CXX) $(COMPFLAGS) $(foreach dir,$(INCLUDES),-I$(dir)) -c $< -o $@
	@$(CXX) $(DEPFLAGS) $(foreach dir,$(INCLUDES),-I$(dir))

-include $(DEP)

#------------------------------------------------------------------------------#
############################# RECOMPILATION CALL ###############################
#------------------------------------------------------------------------------#
re:
	@$(MAKE) fclean
	$(MAKE) all

re_debug:
	@$(MAKE) fclean
	$(MAKE) MODE=debug

#------------------------------------------------------------------------------#
############################### CLEANING RULES #################################
#------------------------------------------------------------------------------#
clean:
	@$(RM) $(TEMP_PATH)
	@echo "$(YELLOW)-- removed temporary files --$(RESET)"

fclean:
	@$(RM) $(TEMP_PATH) $(NAME)
	@echo "$(YELLOW)-- removed $(NAME) & temporary files --$(RESET)"


.PHONY: default all debug test re re_debug clean fclean
