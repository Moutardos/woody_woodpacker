CC = cc
AR= ar
NAME = libft.a
ODIR = obj
SDIR = src
_OBJS =  ft_atoi.o ft_calloc.o ft_isalpha.o ft_isdigit.o ft_itoa.o ft_memcmp.o\
ft_memmove.o ft_putchar_fd.o ft_putnbr_fd.o ft_split.o ft_strdup.o\
ft_strjoin.o ft_strlcpy.o ft_strmapi.o ft_strnstr.o ft_strtrim.o ft_tolower.o\
ft_bzero.o ft_isalnum.o ft_isascii.o ft_isprint.o ft_memchr.o ft_memcpy.o\
ft_memset.o  ft_putendl_fd.o ft_putstr_fd.o ft_strchr.o ft_striteri.o ft_strlcat.o\
ft_strlen.o  ft_strncmp.o ft_strrchr.o ft_substr.o  ft_toupper.o ft_isspace.o\
ft_itob.o ft_itohexa.o ft_strlow.o ft_strup.o ft_cinstr.o ft_strmap.o ft_striter.o\
ft_puthex_fd.o ft_putnbru_fd.o ft_pow.o ft_printf/ft_printf.o ft_printf/convert.o gnl/get_next_line.o gnl/get_next_line_utils.o\
gnl/count_lines.o ft_bitsize.o ft_free_split.o ft_strtoi.o ft_strtoll.o
_OBJst_bONUS =ft_lstnew.o ft_lstdelone.o ft_lstclear.o ft_lstadd_front.o ft_lstadd_back.o\
ft_lstmap.o ft_lstiter.o ft_lstlast.o ft_lstsize.o
OBJS = $(patsubst %, $(ODIR)/%, $(_OBJS))
OBJst_bONUS = $(patsubst %, $(ODIR)/%, $(_OBJst_bONUS))
RM = rm -f
LIBFLAGS = -rc
CFLAGS = -Wall -Wextra -Werror -o 

all: hello | $(NAME)
	@ echo "libft.a has been compiled!"

hello:
	@ echo "starting compilation..."
$(ODIR):
	@ mkdir -p $(ODIR)
	@ mkdir -p $(ODIR)/ft_printf
	@ mkdir -p $(ODIR)/gnl

$(ODIR)/%.o : $(SDIR)/%.c | $(ODIR)
	@ $(CC) $(CFLAGS)  $@ -c $<

$(NAME) : $(OBJS)
	@ $(AR) $(LIBFLAGS) $(NAME) $(OBJS)

clean:
	$(RM) -R $(ODIR)

fclean : clean
	$(RM) $(NAME)

re: fclean all

bonus: $(BONUS_OBJ)
	$(AR) $(LIBFLAGS) $(NAME) $(OBJst_bONUS)

.PHONY: all clean fclean re test bonus so
