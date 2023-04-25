.PHONY: test all

INCLUDES=-I.
LIBDIR=.
SOURCES := $(LIBDIR)/ipv6_cidr.c
TEST_SOURCES := $(LIBDIR)/test_ipv6_cidr.cpp
OBJ := $(patsubst %.c, ./%.o,$(SOURCES))
TEST_OBJ := $(patsubst %.cpp, ./%.o,$(TEST_SOURCES))
TARGET := libipv6cidr.a
TEST_TARGET := ipv6cidr_test

CFLAGS +=-O2
CPPFLAGS +=-O2
LDFLAGS +=-lpthread -ldl -lgtest

all: $(TARGET)

test: $(TEST_TARGET)
	@./ipv6cidr_test

clean:
	@rm -f *.o 	
	@rm -f ipv6cidr_test 
	@rm -f *.a

$(TEST_TARGET):$(TEST_OBJ) $(OBJ)
	@echo -n "linking $@ ... "
	@g++ $(CPPFLAGS) $(LDFLAGS) $^ -o $@ /usr/local/lib/libgtest.a $(LDFLAGS)
	@echo "Done!" 

$(TARGET):$(OBJ)
	@echo -n "linking $@ ... "
	@ar rcs $@ $^
	@echo "Done!"

./%.o:%.c
	@[ -d $(dir $@) ] || mkdir -p $(dir $@)
	@echo -n "bulding $@ ... "
	@gcc $(CFLAGS) $(INCLUDES) -o $@ -c $<
	@echo "Done!"

define HELP
    usage:
    make help: print current help contents
    make : make libipv6cidr.a 
    make test: run test
    make clean: delete all the target files
endef
export HELP

help:
	@echo "$$HELP"
