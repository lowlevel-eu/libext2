#  Konvertier *.c in *.o
OBJECT_FILES = $(addsuffix .o,$(basename $(1)))


LL_OBJS= $(LIBEXT2_OBJ)

TESTS= $(notdir $(wildcard tests/*))




##############################################################################
# Targets fuer Tests
##############################################################################
# Quelldateien der testlib
TESTLIB_SRC= $(wildcard testlib/*.c)

# Objektdateien der testlib
TESTLIB_OBJ= $(call OBJECT_FILES,$(TESTLIB_SRC))
ALL_OBJS+= $(TESTLIB_OBJ)

libtest.a: CFLAGS+= -I libext2/include
libtest.a: $(TESTLIB_OBJ)
	ar rs $@ $^

define TEST_template
$(1)_SRC := $(wildcard tests/$(1)/*.c)
$(1)_OBJ := $$(call OBJECT_FILES,$$($(1)_SRC))
ALL_OBJS+= $$($(1)_OBJ)
tests/$(1)/test: CFLAGS+= -I libext2/include -I testlib -L .
tests/$(1)/test: libext2.a libtest.a $$($(1)_OBJ)
	$(CC) -L . -o tests/$(1)/test $$($(1)_OBJ) -lext2 -ltest

run_tests/$(1)/test: tests/$(1)/test
	@tests/$(1)/test tests/$(1)/image || true
endef
$(foreach test,$(TESTS),$(eval $(call TEST_template,$(test))))


build_tests: $(TESTS:%=tests/%/test)
run_tests: $(TESTS:%=run_tests/%/test)

##############################################################################
# Targets fuer libext2
##############################################################################
# Quelldateien
LIBEXT2_SRC= $(wildcard libext2/*.c)

# Objektdateien
LIBEXT2_OBJ= $(call OBJECT_FILES,$(LIBEXT2_SRC))
ALL_OBJS+= $(LIBEXT2_OBJ)

libext2.a: $(LIBEXT2_OBJ)
	ar rs $@ $^
libext2: libext2.a


##############################################################################
# Allgemeine Targets
##############################################################################
all: libext2 build_tests
clean:
	@rm -f $(ALL_OBJS) $(TESTS:%=tests/%/test) libtest.a libext2.a
