# ================================= NAME =================================== #

NAME = cub3d

# =============================== DIRECTORIES =============================== #

SRCDIR		= srcs/
OBJDIR		= objs/
INCDIR		= includes/
LIBSDIR 	= libs/
LIBFTDIR 	= $(LIBSDIR)Libft/
MLIBXDIR 	= $(LIBSDIR)minilibx-linux/
INCLUDE 	= -I$(INCDIR)
PARSEDIR	= parsing/
UTILSDIR	= utils/

# ================================= FILES =================================== #

SRC_FILES	= main.c 
UTILS_FILES	= utils.c parse_utils.c
PARSE_FILES = parsing1.c parse_map.c

SRC = $(addprefix $(SRCDIR), $(SRC_FILES)) \
	  $(addprefix $(SRCDIR)$(PARSEDIR), $(PARSE_FILES)) \
	  $(addprefix $(SRCDIR)$(UTILSDIR), $(UTILS_FILES)) 

# ================================ OBJECTS =================================== #

OBJS = $(addprefix $(OBJDIR), $(SRC:$(SRCDIR)%.c=%.o))

# ================================ LIBRARY =================================== #

LIBFT		= $(LIBFTDIR)libft.a

# =============================== COMPILER ================================== #

CC				= cc
CFLAGS			= -Wall -Wextra -Werror -g3
MLX_FLAGS = -L$(MLIBXDIR) -lmlx -lXext -lX11 -lm -lz
RM				= rm -f
MAKE			= make

# ================================= COLORS =================================== #

DEF_COLOR	= \033[0;39m
GRAY		= \033[0;90m
RED			= \033[0;91m
GREEN		= \033[0;92m
YELLOW		= \033[0;93m
BLUE		= \033[0;94m
LIGHT_BLUE	= \033[38;2;85;205;252m
PINK		= \033[38;2;247;168;184m
MAGENTA		= \033[0;95m
CYAN		= \033[0;96m
WHITE		= \033[0;97m

# ================================= RULES ==================================== #

all: $(NAME)

# --------------------------------- Targets ---------------------------------- #
$(NAME): $(OBJS) $(LIBFT)
	@$(MAKE) -s -C $(LIBFTDIR)
	@$(MAKE) -s -C $(MLIBXDIR)
	@echo "$(YELLOW)Linking $(NAME)...$(DEF_COLOR)"
	@$(CC) $(CFLAGS) $(OBJS) $(LIBFT) $(MLX_FLAGS) -o $(NAME)
	@echo "$(GREEN)✓ $(NAME) created successfully!$(DEF_COLOR)"

# ----------------------------- Object Files -------------------------------- #
$(OBJDIR)%.o: $(SRCDIR)%.c
	@mkdir -p $(dir $@)
	@echo "$(CYAN)Compiling $<...$(DEF_COLOR)"
	@$(CC) $(CFLAGS) -c $(INCLUDE) -I$(LIBFTDIR)includes -I$(MLIBXDIR) $< -o $@

# ------------------------------- Library ----------------------------------- #
$(LIBFT):
	@echo "$(MAGENTA)Building libft...$(DEF_COLOR)"
	@$(MAKE) -s -C $(LIBFTDIR)
	@echo "$(GREEN)✓ libft built successfully!$(DEF_COLOR)"

mlx:
	@cd $(LIBSDIR) && git clone https://github.com/42Paris/minilibx-linux.git

# ------------------------------ Clean Rules -------------------------------- #
clean:
	@echo "$(RED)Cleaning object files...$(DEF_COLOR)"
	@rm -rf $(OBJDIR)
	@$(MAKE) clean -s -C $(LIBFTDIR)
	@cd $(MLIBXDIR) && $(MAKE) -s clean
	@echo "$(GREEN)✓ Object files cleaned!$(DEF_COLOR)"

mlxclean:
	@rm -rf $(MLIBXDIR)
	@echo "$(LIGHT_BLUE)MLX deleted$(DEF_COLOR)"

fclean: clean
	@echo "$(RED)Cleaning executables...$(DEF_COLOR)"
	@$(RM) $(NAME)
	@$(MAKE) fclean -s -C $(LIBFTDIR)
	@echo "$(GREEN)✓ All files cleaned!$(DEF_COLOR)"

re: fclean all

reclear: 
	@make re && clear

v: reclear
	valgrind --leak-check=full  --track-origins=yes --show-leak-kinds=all --track-fds=yes ./$(NAME) maps/map1.cub
# ------------------------------- Phony Targets ----------------------------- #
.PHONY: all clean fclean re