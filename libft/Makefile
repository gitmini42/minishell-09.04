NAME = libft.a
File = ft_atoi.c \
       ft_bzero.c \
       ft_calloc.c \
       ft_isalnum.c \
       ft_isalpha.c \
       ft_isascii.c \
       ft_isdigit.c \
       ft_isprint.c \
       ft_itoa.c \
       ft_memchr.c \
       ft_memcmp.c \
       ft_memcpy.c \
       ft_memmove.c \
       ft_memset.c \
       ft_putchar_fd.c \
       ft_putendl_fd.c \
       ft_putnbr_fd.c \
       ft_putstr_fd.c \
       ft_split.c \
       ft_strchr.c \
       ft_strdup.c \
       ft_striteri.c \
       ft_strlcat.c \
       ft_strlcpy.c \
       ft_strlen.c \
       ft_strmapi.c \
       ft_strncmp.c \
       ft_strnstr.c \
       ft_strrchr.c \
       ft_strtrim.c \
       ft_substr.c \
       ft_tolower.c \
       ft_toupper.c \
       ft_printf.c \
       ft_putnbr_base.c \
       ft_putnbr_base_u.c \
       ft_putnbr_base_ul.c \
       ft_strcmp.c \
	   ft_strcpy.c \
	   ft_strcat.c \
	   get_next_line.c \
       get_next_line_utils.c \
	   #ft_strjoin.c


BONUS= ft_lstadd_back.c \
       ft_lstadd_front.c \
       ft_lstclear.c \
       ft_lstdelone.c \
       ft_lstiter.c \
       ft_lstlast.c \
       ft_lstmap.c \
       ft_lstnew.c \
       ft_lstsize.c

OBJS = $(File:.c=.o)
BOBJS = $(BONUS:.c=.o)
Compiler = gcc
Flags = -Wall -Wextra -Werror


all: $(NAME)

$(NAME): $(OBJS)
	@ar rcs $(NAME) $(OBJS)
%.o:%.c
	@${Compiler} ${Flags} -c $< -o $@

clean:
	@rm -f $(OBJS) $(BOBJS)

fclean: clean
	@rm -f $(NAME)

re: fclean all

bonus:	$(OBJS) $(BOBJS)
	ar rcs $(NAME) $(OBJS) $(BOBJS)
