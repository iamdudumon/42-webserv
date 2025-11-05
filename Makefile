NAME		:= webserv
CXX			:= c++
CXXFLAGS	:= -Wall -Wextra -Werror -std=c++98 -MMD -MP
LDFLAGS		:=

SRCDIR		:= src
BUILDDIR	:= obj

SRC			:= $(shell find $(SRCDIR) -name '*.cpp')
OBJ			:= $(SRC:$(SRCDIR)/%.cpp=$(BUILDDIR)/%.o)
DEP			:= $(OBJ:.o=.d)

.PHONY: all clean fclean re

DEBUG ?= 0
ifeq ($(DEBUG),1)
	CXXFLAGS += -O0 -g -DDEBUG
endif

SANITIZE ?= 0
ifeq ($(SANITIZE),1)
	CXXFLAGS += -fsanitize=address -fno-omit-frame-pointer
	LDFLAGS  += -fsanitize=address
endif

all: $(NAME)

$(NAME): $(OBJ)
	$(CXX) $(OBJ) $(LDFLAGS) -o $@
	@echo "build success : $(NAME)"

$(BUILDDIR)/%.o: $(SRCDIR)/%.cpp
	@mkdir -p $(dir $@)
	$(CXX) $(CXXFLAGS) -c $< -o $@
	@echo "compile: $<"

clean:
	@rm -rf $(BUILDDIR)

fclean: clean
	@rm -f $(NAME)

re: fclean all

-include $(DEP)
