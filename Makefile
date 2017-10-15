NAME=image-sketch

SRCDIR=src
OBJDIR=$(SRCDIR)/obj

OBJNAMES=util threading drawing
OBJSRC=$(addprefix $(SRCDIR)/, $(addsuffix .c, $(OBJNAMES)))
OBJ=$(addprefix $(OBJDIR)/, $(addsuffix .o, $(OBJNAMES)))
OBJHEADER=$(addprefix $(SRCDIR)/, $(addsuffix .h, $(OBJNAMES)))

HEADERNAMES=globals
HEADERS=$(addprefix $(SRCDIR)/, $(addsuffix .h, $(HEADERNAMES)))

LIBS=SDL2 SDL2_image m


$(NAME) : $(SRCDIR)/main.c $(OBJ) $(HEADERS)
	gcc  $(SRCDIR)/main.c $(OBJ) -o $(NAME) $(addprefix -l, $(LIBS)) -pthread -Ofast

$(OBJ) : $(OBJSRC) $(OBJHEADER) $(HEADERS)
	gcc -c $(OBJSRC) $(addprefix -l, $(LIBS)) -pthread -Ofast
	mv $(notdir $(OBJ)) $(OBJDIR)

git :
	rm $(OBJ) pics/*

clean :
	rm $(OBJ) $(NAME)
