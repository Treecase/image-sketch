NAME=image-sketch

SRCDIR=src
OBJDIR=$(SRCDIR)/obj

OBJNAMES=util globals
OBJSRC=$(addprefix $(SRCDIR)/, $(addsuffix .c, $(OBJNAMES)))
OBJ=$(addprefix $(OBJDIR)/, $(addsuffix .o, $(OBJNAMES)))
OBJHEADER=$(addprefix $(SRCDIR)/, $(addsuffix .h, $(OBJNAMES)))

LIBS=SDL2 SDL2_image m


$(NAME) : $(SRCDIR)/main.c $(OBJ)
	gcc  $(SRCDIR)/main.c $(OBJ) -o $(NAME) $(addprefix -l, $(LIBS))

$(OBJ) : $(OBJSRC) $(OBJHEADER)
	gcc -c $(OBJSRC) $(addprefix -l, $(LIBS))
	mv $(notdir $(OBJ)) $(OBJDIR)

clean :
	rm $(OBJ) $(NAME)
