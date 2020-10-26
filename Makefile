STYLE_FLAGS = -Werror=all -Werror=extra -pedantic\
-Wno-error=format-nonliteral -Wformat-nonliteral \
-Werror=write-strings \
-Wno-error=discarded-qualifiers -Wno-error=incompatible-pointer-types \
-Wno-clobbered \
-fPIE -fstack-check -fstack-protector-all -ftrapv -D_FORTIFY_SOURCE=2 \
-Wl,-z,relro,-z,now -Wl,-z,noexecstack \
-Wbad-function-cast -Wcast-align -Werror=cast-qual -Wconversion \
-Werror=float-equal -Wformat-security -Werror=format-overflow=2 -Wformat-truncation \
-Winline -Winvalid-pch -Werror=jump-misses-init -Wlogical-op -Wmissing-declarations \
-Wmissing-prototypes -Wnested-externs -Wold-style-definition \
-Woverlength-strings -Werror=redundant-decls -Werror=shadow -Werror=strict-overflow=5 \
-Wsuggest-attribute=const -Werror=switch-default -Wtrampolines \
-Werror=undef -Wunsuffixed-float-constants \
-Werror=unused -Werror=stringop-overflow=4 -Wdeclaration-after-statement

LD_FLAGS = -L./bin/ -lalarm -Wl,-rpath,../bin/ -lzmq
LD_FLAGS_TEST = -L./bin/ -lalarm -Wl,-rpath,./bin/ -lzmq -ltap

LIBNAME = bin/libalarm.so
LIBNAME_obj = libalarm_connections.o libalarm_files.o \
libalarm_send_recv.o libalarm_additional_func.o
LIBNAME_src = src/libalarm_connections.c src/libalarm_files.c \
src/libalarm_send_recv.c src/libalarm_additional_func.c

MODULES_obj = alarm_manager.o module.o other_module.o filter.o
MODULES_src = src/alarm_manager.c src/module.c src/other_module.c src/filter.c

MANAGER_obj = alarm_manager.o
MANAGER_bin = bin/alarm_manager

MODULE_1_obj = module.o
MODULE_2_obj = other_module.o
MODULE_3_obj = filter.o

MODULE_1_bin = bin/module
MODULE_2_bin = bin/other_module
MODULE_3_bin = bin/filter

TESTS_obj = make_test.o test_general_func.o fff_test_zmq_func.o fff_test_alarm_func.o
TESTS_src = tests/make_test.c tests/test_general_func.c tests/fff_test_zmq_func.c tests/fff_test_alarm_func.c

TEST_1 = tests/make_test
TEST_2 = tests/test_general_func
TEST_3 = tests/fff_test_zmq_func
TEST_4 = tests/fff_test_alarm_func

TEST_1_obj = make_test.o
TEST_2_obj = test_general_func.o
TEST_3_obj = fff_test_zmq_func.o
TEST_4_obj = fff_test_alarm_func.o

TEST_1_src = tests/make_test.c
TEST_2_src = tests/test_general_func.c
TEST_3_src = tests/fff_test_zmq_func.c
TEST_4_src = tests/fff_test_alarm_func.c

all: $(LIBNAME) $(TEST_1) run_test $(MANAGER_bin) $(MODULE_1_bin) $(MODULE_2_bin) $(MODULE_3_bin)\
$(TEST_2) $(TEST_3) $(TEST_4)  clean_obj

$(LIBNAME): $(LIBNAME_obj)
	mkdir -p bin/
	rm -f messages.txt
	gcc -shared -o $(LIBNAME) $(LIBNAME_obj)

$(LIBNAME_obj): $(LIBNAME_src)
	gcc $(STYLE_FLAGS) -fPIC -c $(LIBNAME_src)

$(MANAGER_bin): $(MANAGER_obj)
	gcc $(MANAGER_obj) -o $(MANAGER_bin) $(LD_FLAGS)

$(MODULE_1_bin): $(MODULE_1_obj)
	gcc $(MODULE_1_obj) -o $(MODULE_1_bin) $(LD_FLAGS)

$(MODULE_2_bin): $(MODULE_2_obj)
	gcc $(MODULE_2_obj) -o $(MODULE_2_bin) $(LD_FLAGS)

$(MODULE_3_bin): $(MODULE_3_obj)
	gcc $(MODULE_3_obj) -o $(MODULE_3_bin) $(LD_FLAGS)

$(MODULES_obj): $(MODULES_src)
	gcc $(STYLE_FLAGS) -c $(MODULES_src)

$(TEST_1): $(TEST_1_obj)
	gcc $(TEST_1_obj) -o $(TEST_1) $(LD_FLAGS_TEST)

$(TEST_2): $(TEST_2_obj)
	gcc $(TEST_2_obj) -o $(TEST_2) $(LD_FLAGS) -ltap

$(TEST_3): $(TEST_3_obj)
	gcc $(TEST_3_obj) -o $(TEST_3) $(LD_FLAGS) -ltap

$(TEST_4): $(TEST_4_obj)
	gcc $(TEST_4_obj) -o $(TEST_4) $(LD_FLAGS) -ltap

$(TESTS_obj): $(TESTS_src)
	gcc -c $(TESTS_src)


run_test: $(TEST_1)
	$(TEST_1)
	rm -f $(TEST_1)

clean_obj:
	rm -f *.o

clean:
	rm -rf bin/
	rm -f messages.txt
	rm -rf $(TEST_1)
	rm -rf $(TEST_2)
	rm -rf $(TEST_3)
	rm -rf $(TEST_4)
	rm -rf *.o
