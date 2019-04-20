CXX=g++
LD=

INCDIR=include
SRCDIR=src
OBJDIR=obj
LIBDIR=lib
BINDIR=bin

LIBNAME=cppnet

CXXFLAGS=-Wall -Wextra -Wpedantic -fPIC -std=c++17 -O2 -DNDEBUG -I$(INCDIR)
LDFLAGS=-s -L$(LIBDIR)

LIB_INCLUDES=$(shell find $(INCDIR)/net -type f)

LIB_SOURCES=$(shell find src/ ! -path "*/examples/*" -type f)
TEST_SOURCES=$(shell find $(SRCDIR)/examples -type f)

LIB_OBJECTS=$(patsubst $(SRCDIR)/%,$(OBJDIR)/%.o,$(LIB_SOURCES))
TEST_OBJECTS=$(patsubst $(SRCDIR)/examples/%,$(OBJDIR)/examples/%.o,$(TEST_SOURCES))

LIB_STATIC=$(LIBDIR)/lib$(LIBNAME).a
LIB_SHARE=$(LIBDIR)/lib$(LIBNAME).so

EXAMPLES=$(patsubst $(SRCDIR)/examples/%,$(BINDIR)/examples/%,$(basename $(TEST_SOURCES)))

all: share examples # static

share: $(LIB_SHARE) $(LIB_INCLUDES)

# static: $(LIB_STATIC) $(LIB_INCLUDES)

examples: $(EXAMPLES)

clean:
	rm -f $(LIB_STATIC) $(LIB_SHARE) $(EXAMPLES) $(LIB_OBJECTS) $(TEST_OBJECTS)

install: share # static
	$(if $(INSTALL_DIR),,$(error INSTALL_DIR is undefined))
	@mkdir -pv $(INSTALL_DIR)/lib
	@mkdir -pv $(INSTALL_DIR)/include/net
	@cp -v $(LIB_SHARE) $(INSTALL_DIR)/lib
	@cp -v $(LIB_INCLUDES) $(INSTALL_DIR)/include/net

$(BINDIR)/examples/%: $(OBJDIR)/examples/%.cpp.o $(LIB_SHARE)
	@mkdir -pv $(dir $@)
	$(CXX) $(CXXFLAGS) $(LDFLAGS) -l$(LIBNAME) -lpthread -o $@ $<

# $(LIB_STATIC): $(LIB_OBJECTS)
# 	@mkdir -pv $(dir $@)
# 	ar rs $(LIB_STATIC) $?

$(LIB_SHARE): $(LIB_OBJECTS)
	@mkdir -pv $(dir $@)
	$(CXX) -shared $(CXXFLAGS) $(LDFLAGS) -o $@ $^

$(OBJDIR)/%.o: $(SRCDIR)/% $(LIB_INCLUDES)
	@mkdir -pv $(dir $@)
	$(CXX) $(CXXFLAGS) -c -o $@ $<
