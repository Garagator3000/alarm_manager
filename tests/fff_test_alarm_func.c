#include "../src/alarm.h"
#include "fff.h"
#include <tap.h>

DEFINE_FFF_GLOBALS;

FAKE_VALUE_FUNC(int, alarm__create_connection, struct Connection *, enum Role, int);
FAKE_VALUE_FUNC(int, alarm__destroy_connection, struct Connection *);
FAKE_VALUE_FUNC(int, alarm__send_signal, struct Connection, enum Message_signal);
FAKE_VALUE_FUNC(int, alarm__recv_signal, struct Connection, enum Message_signal *);
FAKE_VALUE_FUNC(int, alarm__send_message, struct Connection, struct Message);
FAKE_VALUE_FUNC(int, alarm__recv_message, struct Connection, struct Message *);
FAKE_VALUE_FUNC(int, alarm__read_from_file, struct Note *, int);
FAKE_VALUE_FUNC(int, alarm__message_compare, struct Message, struct Message);

char const *SUCCESS = "Функция завершилась успешно: ТЕСТ ПРОЙДЕН";
char const *FAIL = "Функция завершилась с ошибкой: ТЕСТ ПРОЙДЕН";

void reset_fake_funcs()
{
    RESET_FAKE(alarm__create_connection);
    RESET_FAKE(alarm__destroy_connection);
    RESET_FAKE(alarm__send_signal);
    RESET_FAKE(alarm__recv_signal);
    RESET_FAKE(alarm__send_message);
    RESET_FAKE(alarm__recv_message);
    RESET_FAKE(alarm__read_from_file);
    RESET_FAKE(alarm__message_compare);
}

int alarm__send_all_message_test(
    int max_quantity_message)
{
    printf("\n\n%s\n\n", __func__);

    plan(NO_PLAN);
    reset_fake_funcs();
        alarm__read_from_file_fake.return_val = 10;
        alarm__create_connection_fake.return_val = ER_ZMQ_FUNC;
        cmp_ok(alarm__send_all_message(max_quantity_message), "==", ER_ZMQ_FUNC, FAIL);

        alarm__create_connection_fake.return_val = OK;
        alarm__send_message_fake.return_val = ER_ZMQ_FUNC;
        cmp_ok(alarm__send_all_message(max_quantity_message), "==", ER_ZMQ_FUNC, FAIL);
    reset_fake_funcs();
    done_testing();
}

int alarm__recv_all_message_test(
    struct Connection connection,
    struct Message *message_array,
    int quantity)
{
    printf("\n\n%s\n\n", __func__);

    plan(NO_PLAN);
    reset_fake_funcs();
        alarm__create_connection_fake.return_val = ER_ZMQ_FUNC;
        cmp_ok(alarm__recv_all_message(connection, message_array, quantity), "==", ER_ZMQ_FUNC, FAIL);

        alarm__create_connection_fake.return_val = OK;
        alarm__send_signal_fake.return_val = ER_ZMQ_FUNC;
        cmp_ok(alarm__recv_all_message(connection, message_array, quantity), "==", ER_ZMQ_FUNC, FAIL);

        alarm__send_signal_fake.return_val = OK;
        alarm__recv_signal_fake.return_val = ER_ZMQ_FUNC;
        cmp_ok(alarm__recv_all_message(connection, message_array, quantity), "==", ER_ZMQ_FUNC, FAIL);
    reset_fake_funcs();
    done_testing();
}

int alarm__recv_by_filter_test(
    struct Connection connection,
    struct Message filter,
    struct Message *message_array,
    int quantity)
{
    printf("\n\n%s\n\n", __func__);

    plan(NO_PLAN);
    reset_fake_funcs();
        alarm__create_connection_fake.return_val = ER_ZMQ_FUNC;
        cmp_ok(alarm__recv_by_filter(connection, filter, message_array, quantity), "==", ER_ZMQ_FUNC, FAIL);

        alarm__create_connection_fake.return_val = OK;
        alarm__send_signal_fake.return_val = ER_ZMQ_FUNC;
        cmp_ok(alarm__recv_by_filter(connection, filter, message_array, quantity), "==", ER_ZMQ_FUNC, FAIL);

        alarm__send_signal_fake.return_val = OK;
        alarm__send_message_fake.return_val = ER_ZMQ_FUNC;
        cmp_ok(alarm__recv_by_filter(connection, filter, message_array, quantity), "==", ER_ZMQ_FUNC, FAIL);

        alarm__send_message_fake.return_val = OK;
        alarm__recv_signal_fake.return_val = ER_ZMQ_FUNC;
        cmp_ok(alarm__recv_by_filter(connection, filter, message_array, quantity), "==", ER_ZMQ_FUNC, FAIL);

        alarm__recv_signal_fake.return_val = OK;
        alarm__recv_message_fake.return_val = ER_ZMQ_FUNC;
        cmp_ok(alarm__recv_by_filter(connection, filter, message_array, quantity), "==", NO_RECV_MSG, FAIL);
    reset_fake_funcs();
    done_testing();
}

int alarm__send_by_filter_test(
    struct Message filter,
    int quantity)
{
    printf("\n\n%s\n\n", __func__);

    plan(NO_PLAN);
    reset_fake_funcs();
        alarm__create_connection_fake.return_val = ER_ZMQ_FUNC;
        cmp_ok(alarm__send_by_filter(filter, quantity), "==", ER_ZMQ_FUNC, FAIL);

        alarm__create_connection_fake.return_val = OK;
        alarm__read_from_file_fake.return_val = ER_READ_FILE;
        cmp_ok(alarm__send_by_filter(filter, quantity), "==", ER_READ_FILE, FAIL);

        alarm__read_from_file_fake.return_val = quantity;
        alarm__message_compare_fake.return_val = 1;
        alarm__send_message_fake.return_val = ER_ZMQ_FUNC;
        cmp_ok(alarm__send_by_filter(filter, quantity), "==", ER_ZMQ_FUNC, FAIL);

        alarm__send_message_fake.return_val = ER_ZMQ_FUNC;
        alarm__send_signal_fake.return_val = ER_ZMQ_FUNC;
        cmp_ok(alarm__send_by_filter(filter, quantity), "==", ER_ZMQ_FUNC, FAIL);
    reset_fake_funcs();
    done_testing();
}

int alarm__send_quantity_test(
    int quantity)
{
    printf("\n\n%s\n\n", __func__);

    plan(NO_PLAN);
    reset_fake_funcs();
        alarm__create_connection_fake.return_val = ER_ZMQ_FUNC;
        cmp_ok(alarm__send_quantity(quantity), "==", ER_ZMQ_FUNC, FAIL);
    reset_fake_funcs();
    done_testing();
}

int alarm__recv_quantity_test(
    struct Connection connection)
{
    printf("\n\n%s\n\n", __func__);

    plan(NO_PLAN);
    reset_fake_funcs();
        alarm__create_connection_fake.return_val = ER_ZMQ_FUNC;
        cmp_ok(alarm__recv_quantity(connection), "==", ER_ZMQ_FUNC, FAIL);

        alarm__create_connection_fake.return_val = OK;
        alarm__send_signal_fake.return_val = ER_ZMQ_FUNC;
        cmp_ok(alarm__recv_quantity(connection), "==", ER_ZMQ_FUNC, FAIL);
    reset_fake_funcs();
    done_testing();
}


int main()
{
    void *ctx = zmq_ctx_new();
    void *sock = zmq_socket(ctx, ZMQ_SUB);
    struct Connection test_connection = {ctx, sock};
    enum Role test_role = PUBLISHER;
    int test_spec = 0;
    int test_max_quantity = 10;

    enum Message_signal test_sig = SEND_MESSAGE;

    struct Message test_message =
    {
        "test module",
        NORMALIZE,
        USUALLY,
        "testmodule"
    };
    struct Message *test_message_array = calloc(test_max_quantity, sizeof(Message));
    if (NULL == test_message_array)
    {
        printf("Не удалось выделить память под массив Message\n");
        goto finally;
    }

    alarm__send_all_message_test(test_max_quantity);
    alarm__recv_all_message_test(test_connection, test_message_array, test_max_quantity);
    alarm__recv_by_filter_test(test_connection, test_message, test_message_array, test_max_quantity);
    alarm__send_by_filter_test(test_message, test_max_quantity);
    alarm__send_quantity_test(test_max_quantity);
    alarm__recv_quantity_test(test_connection);

 finally:
    return 0;
}
