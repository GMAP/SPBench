PREFIX = ${PARSECDIR}/pkgs/apps/ferret/inst/${PARSECPLAT}
TARGET = ferret
ARCH = parsec

CFLAGS += -no-pie -O3

LDLIBS += `pkg-config --cflags --libs gsl` $(LIBS) -lrt -lm -lcass -lgsl -lgslcblas -ljpeg -lstdc++ `pkg-config --cflags --libs libjpeg`
#`pkg-config --cflags --libs gsl` $(LIBS) -lrt -lm -lcass -lgsl -lgslcblas -ljpeg -lstdc++ `pkg-config --cflags --libs libjpeg`

.PHONY:	all clean install dirs tools image server $(OBJDIR)/%.o $(LIBDIR)/libcass.a $(LIBDIR)/libcassimage.a

PKG=#`pkg-config --cflags --libs libjpeg` `pkg-config --cflags --libs gsl`

ROOT = $(CURDIR)
SRCDIR = $(ROOT)/src
INCDIR = $(ROOT)/include

BINDIR = $(ROOT)/$(ARCH)/bin
LIBDIR = $(ROOT)/$(ARCH)/lib
OBJDIR = $(ROOT)/$(ARCH)/obj

CFLAGS += -I$(INCDIR) 
LDFLAGS += -L$(LIBDIR) 

VPATH = $(CURDIR)/src:$(CURDIR)/src/lsh:$(CURDIR)/tools:$(CURDIR)/image

export LIBDIR BINDIR OBJDIR CC CXX CFLAGS LDFLAGS LDLIBS

all:	dirs tools $(OBJDIR)/%.o $(LIBDIR)/libcass.a $(LIBDIR)/libcassimage.a #benchmark

dirs:
	mkdir -p $(BINDIR)
	mkdir -p $(LIBDIR)
	mkdir -p $(OBJDIR)

# build the cass library
lib:	$(LIBDIR)/libcass.a 

lib_src := $(wildcard src/*.c) $(wildcard src/lsh/*.c)
lib_src := $(notdir $(lib_src))
lib_obj := $(addprefix $(OBJDIR)/, $(lib_src:.c=.o))


$(LIBDIR)/libcass.a:	$(lib_obj)
	@echo "   A  '$@'"
	@$(AR) rcs $@ $^

# build the image support library
libimage_src := image.c extract.c edge.c srm.c
libimage_obj := $(addprefix $(OBJDIR)/, $(libimage_src:.c=.o))


$(LIBDIR)/libcassimage.a:	$(libimage_obj)
	@echo "   A  '$@'"
	@$(AR) rcs $@ $^

# build the tool programs
tools_src := $(wildcard tools/*.c)
tools_tgt := $(notdir $(tools_src))
tools_tgt := $(addprefix $(BINDIR)/, $(tools_tgt:.c=))

tools : $(tools_tgt)

server_src := $(wildcard server/*.c) 
server_tgt := $(notdir $(server_src))
server_tgt := $(addprefix $(BINDIR)/, $(server_tgt:.c=))

server : $(server_tgt)

image_tgt := cass_img_extract
image_tgt := $(addprefix $(BINDIR)/, $(image_tgt))

image: $(image_tgt)

#CC = gcc-7
#CXX = g++ -std=c++1y
clean:
	-rm $(OBJDIR)/*

install:
	cp -r $(BINDIR) $(PREFIX)
	cp -r $(LIBDIR) $(PREFIX)
	mv $(PREFIX)/bin/$(benchmark_base) $(PREFIX)/bin/$(TARGET)

$(OBJDIR)/%.o : %.c
	@echo "   CC '$<' ==> '$@'"
	@$(CC) $(CFLAGS) -c $< -o $@ $(PKG) $(LDLIBS)

$(OBJDIR)/%.o : %.cpp
	@echo "   CXX '$<' ==> '$@'" 
	@$(CXX) $(CXXFLAGS) $(CFLAGS) -c $< -o $@ $(PKG) $(LDLIBS)

$(tools_tgt): $(BINDIR)/% : $(OBJDIR)/%.o $(LIBDIR)/libcass.a
	@echo "   CXX '$<' ==> '$@'"
	@$(CXX) $(CFLAGS) $(LDFLAGS) $^ -o $@ $(PKG) $(LDLIBS)

$(image_tgt): $(BINDIR)/% : $(OBJDIR)/%.o $(LIBDIR)/libcass.a $(LIBDIR)/libcassimage.a
	@echo "   CXX '$<' ==> '$@'"
	@$(CXX) $(CFLAGS) $(LDFLAGS) $^ -o $@ $(PKG) $(LDLIBS)
