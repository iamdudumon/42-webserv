CC = c++

CFLAGS = -Wall -Wextra -Werror -g -fsanitize=address

STD = -std=c++98

TARGET = ex02

SRCS = src/main.cpp

#HEADER = 

OBJS = $(SRCS:.cpp=.o)

all: $(TARGET) $(HEADER)

$(TARGET): $(OBJS)
	$(CC) $(CFLAGS) $(STD) $(OBJS) -o $(TARGET)

%.o: %.cpp
	@$(CC) $(CFLAGS) $(STD) -c $< -o $@

clean:
	rm -f $(OBJS)

fclean:	clean
	rm -f $(TARGET)

re: fclean all

.PHONY: all clean fclean re