CXX=g++
LD=

INCDIR=include
SRCDIR=src
OBJDIR=obj
LIBDIR=lib
BINDIR=bin

LIBNAME=cppnet

CXXFLAGS=-Wall -Wextra -fPIC -std=c++17 -O2 -DNDEBUG -I$(INCDIR)
LDFLAGS=-s -L$(LIBDIR)

LIB_INCLUDES=$(shell find $(INCDIR)/net -type f)

LIB_SOURCES=$(shell find src/ ! -path "*/tests/*" -type f)
TEST_SOURCES=$(shell find $(SRCDIR)/tests -type f)

LIB_OBJECTS=$(patsubst $(SRCDIR)/%,$(OBJDIR)/%.o,$(LIB_SOURCES))
TEST_OBJECTS=$(patsubst $(SRCDIR)/tests/%,$(OBJDIR)/tests/%.o,$(TEST_SOURCES))

LIB_STATIC=$(LIBDIR)/lib$(LIBNAME).a
LIB_SHARE=$(LIBDIR)/lib$(LIBNAME).so

TESTS=$(patsubst $(SRCDIR)/tests/%,$(BINDIR)/tests/%,$(basename $(TEST_SOURCES)))

all: share tests # static

share: $(LIB_SHARE) $(LIB_INCLUDES)

# static: $(LIB_STATIC) $(LIB_INCLUDES)

tests: $(TESTS)

clean:
	rm -f $(LIB_STATIC) $(LIB_SHARE) $(TESTS) $(LIB_OBJECTS) $(TEST_OBJECTS)

install: share # static
	$(if $(INSTALL_DIR),,$(error INSTALL_DIR is undefined))
	@mkdir -pv $(INSTALL_DIR)/lib
	@mkdir -pv $(INSTALL_DIR)/include/net
	@cp -v $(LIB_SHARE) $(INSTALL_DIR)/lib
	@cp -v $(LIB_INCLUDES) $(INSTALL_DIR)/include/net

$(BINDIR)/tests/%: $(OBJDIR)/tests/%.cpp.o $(LIB_SHARE)
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
