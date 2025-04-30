ENTRY	:= main_generator.c

EXEC	:= $(if $(filter main_parser.c,$(ENTRY)),elf_parser,elf_generator)


BIN_DIR := bin
NAME	:= $(addprefix $(BIN_DIR)/, $(EXEC))

LIBS				:=	ft
LIBS_TARGET			:=	libs/libft/libft.a


SRC_DIR				:=	srcs

SRCS				:=	\
						elf_generator.c \
						elf_utils.c \
						$(ENTRY)

SRCS				:=	${SRCS:%=${SRC_DIR}/%}


OBJ_DIR				:=	objs
OBJS				:=	${SRCS:${SRC_DIR}/%.c=${OBJ_DIR}/%.o}

CFLAGS				:=	-Wall -Wextra
CPPFLAGS			:=	-I includes -I libs/libft/src -gdwarf
LDFLAGS				:=	${addprefix -L, ${dir ${LIBS_TARGET}}}
LDLIBS				:=	${addprefix -l, ${LIBS}}


RM					:=	${RM} -r


all: $(NAME)
.PHONY: all

$(NAME): $(LIBS_TARGET) $(OBJS)
	mkdir -p $(BIN_DIR)
	$(CC) ${LDFLAGS} $^ ${LDLIBS} -o $@ 
.PHONY: $(NAME)

parser:
	$(MAKE) ENTRY=main_parser.c
.PHONY: parser

$(LIBS_TARGET):
	$(MAKE) -C $(@D)
.PHONY: $(LIBS_TARGET)

${OBJ_DIR}/%.o: ${SRC_DIR}/%.c
	mkdir -p ${@D}
	${CC} ${CFLAGS} ${CPPFLAGS} -c -o $@ $<
.PHONY: $(OBJ_DIR)

clean:
	$(MAKE) fclean -C  libs/libft
	$(RM) $(OBJ_DIR)
.PHONY: clean

fclean: clean
	$(RM) $(BIN_DIR)
.PHONY: fclean

re: fclean
	$(MAKE) all
.PHONY: re
