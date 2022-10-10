CC			= c++ -Wall -Wextra -Werror -std=c++98 -g3 #-fsanitize=address
RM			= rm -rf
NAME		= ./ircserv
NAME_SHORT	= ircserv

INCS_DIR	= .
MAIN_INC	= -I$(INCS_DIR)
INCS		= $(shell find $(INCS_DIR) -type f -name "*.hpp")

SRCS_DIR 	= .
SRCS		= $(shell find $(SRCS_DIR) -type f -name "*.cpp")

OBJS		= $(SRCS:.cpp=.o)

%.o			: %.cpp
			$(CC) $(MAIN_INC) -c $< -o $@

all			: $(NAME)

$(NAME)		: $(OBJS) $(INCS)
			$(CC) $(OBJS) $(MAIN_INC) -o $(NAME)

clean		:
			$(RM) $(OBJS)

fclean		: clean
			$(RM) $(NAME)

re			: fclean all
