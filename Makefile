TARGET = webserv
CXX = c++
CPPFLAGS = -Wall -Wextra -Werror
STD = -std=c++98

SRCDIR = SRCDIR
INCDIR = include
OBJDIR = obj

SOURCES = $(shell find $(SRCDIR) -name '*.cpp')

OBJ = $(SOURCES:$(SRCDIR)/%.cpp=$(OBJDIR)/%.o)

INCLUDE_DIRS = $(shell find $(INCDIR) -type d)
INCLUDE_FLAGS = $(addprefix -I, $(INCLUDE_DIRS))

all: $(TARGET)

$(TARGET): $(OBJ)
	$(CXX) $(OBJ) -o $@
	@echo "build success : $(TARGET)"


$(OBJDIR)/%.o: $(SRCDIR)/%.cpp
	@mkdir -p $(dir $@)
	$(CXX) $(CPPFLAGS) $(INCLUDE_FLAGS) -c $< -o $@
	@echo "compile: $<"

$(OBJDIR):
	mkdir -p $(OBJDIR)

clean:
	rm -rf $(OBJDIR)

fclean: clean
	rm -f $(TARGET)

re : 
	$(MAKE) fclean
	$(MAKE) all