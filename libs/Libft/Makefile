# Variables
INCDIR = ./includes/
SRCDIR = ./srcs/
OBJDIR = ./objs/
SRC_FILES = $(wildcard $(SRCDIR)/*.c)
SRC = $(addprefix $(SRCDIR), $(SRC_FILES))
OBJS = $(addprefix $(OBJDIR), $(notdir $(SRC:.c=.o)))
NAME = libft.a
CC = cc
CFLAGS = -Wall -Wextra -Werror -g
RM = rm -f
AR = ar rcs

# Colors
DEF_COLOR = \033[0;39m
LIGHT_BLUE = \033[38;2;85;205;252m
PINK = \033[38;2;247;168;184m
WHITE = \033[38;2;255;255;255m

all: $(NAME)

$(NAME): $(OBJS) 
	@$(AR) $(NAME) $(OBJS)
	@echo "$(PINK)Library $(NAME) created successfully.$(DEF_COLOR)"
	@echo "$(LIGHT_BLUE)Objects compiled successfully.$(DEF_COLOR)"

$(OBJDIR)%.o: $(SRCDIR)%.c
	@mkdir -p $(OBJDIR)
	@$(CC) $(CFLAGS) -c -I$(INCDIR) $< -o $@
	@echo "$(WHITE)Compiling...$(DEF_COLOR)"

clean:
	@rm -rf $(OBJDIR)
	@echo "$(LIGHT_BLUE)libft objs pa mrd$(DEF_COLOR)"

fclean: clean
	@$(RM) $(NAME)
	@echo "$(LIGHT_BLUE)libft.a pa mrd$(DEF_COLOR)"

re: fclean all

.PHONY: all clean fclean re