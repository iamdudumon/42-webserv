TARGET = webserv
CXX = c++
CPPFLAGS = -Wall -Wextra -Werror
STD = -std=c++98

SRCDIR = src
OBJDIR = obj

SOURCES = $(shell find $(SRCDIR) -name '*.cpp')

OBJ = $(SOURCES:$(SRCDIR)/%.cpp=$(OBJDIR)/%.o)

all: $(TARGET)

$(TARGET): $(OBJ)
	@$(CXX) $(OBJ) -o $@
	@echo "build success : $(TARGET)"


$(OBJDIR)/%.o: $(SRCDIR)/%.cpp
	@mkdir -p $(dir $@)
	@$(CXX) $(CPPFLAGS) -c $< -o $@
	@echo "compile: $<"

$(OBJDIR):
	@mkdir -p $(OBJDIR)

clean:
	@rm -rf $(OBJDIR)

fclean: clean
	@rm -f $(TARGET)

re : 
	$(MAKE) fclean
	$(MAKE) all