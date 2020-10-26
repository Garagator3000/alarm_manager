#include "../src/alarm.h"
#include "fff.h"
#include <tap.h>

DEFINE_FFF_GLOBALS;

FAKE_VALUE_FUNC(void *, zmq_ctx_new);
FAKE_VALUE_FUNC(void *, zmq_socket, void *, int);
FAKE_VALUE_FUNC(int, zmq_bind, void *, const char *);
FAKE_VALUE_FUNC(int, zmq_connect, void *, const char *);
FAKE_VALUE_FUNC(int, zmq_setsockopt, void *, int, const void *, size_t);
FAKE_VALUE_FUNC(int, zmq_close, void *);
FAKE_VALUE_FUNC(int, zmq_ctx_destroy, void *);
FAKE_VALUE_FUNC(int, zmq_send, void *, const void *, size_t, int);
FAKE_VALUE_FUNC(int, zmq_recv, void *, void *, size_t, int);
FAKE_VALUE_FUNC(int, alarm__check_message, struct Message);

char const *SUCCESS = "Функция завершилась успешно: ТЕСТ ПРОЙДЕН";
char const *FAIL = "Функция завершилась с ошибкой: ТЕСТ ПРОЙДЕН";

int const ZMQ_ER = -1;
int const ZMQ_OK = 0;

void reset_fake_funcs()
{
    RESET_FAKE(zmq_ctx_new);
    RESET_FAKE(zmq_socket);
    RESET_FAKE(zmq_bind);
    RESET_FAKE(zmq_connect);
    RESET_FAKE(zmq_setsockopt);
    RESET_FAKE(zmq_close);
    RESET_FAKE(zmq_ctx_destroy);
    RESET_FAKE(zmq_send);
    RESET_FAKE(zmq_recv);
    RESET_FAKE(alarm__check_message);
}

int alarm__create_connection_test(
    struct Connection *connection,
    enum Role role,
    int spec)
{
    /*to create the appearance
    that the context was created*/
    int rand = 213;
    int *rand_ptr = &rand;

    printf("\n\n%s\n\n", __func__);
    plan(5);
    reset_fake_funcs();
        cmp_ok(alarm__create_connection(connection, role, spec), "==", ER_ZMQ_FUNC, FAIL);

        zmq_ctx_new_fake.return_val = rand_ptr;
        cmp_ok(alarm__create_connection(connection, role, spec), "==", ER_ZMQ_FUNC, FAIL);

        zmq_socket_fake.return_val = rand_ptr;
        zmq_setsockopt_fake.return_val = ZMQ_ER;
        cmp_ok(alarm__create_connection(connection, role, spec), "==", ER_ZMQ_FUNC, FAIL);

        zmq_connect_fake.return_val = ZMQ_ER;
        cmp_ok(alarm__create_connection(connection, role, spec), "==", ER_ZMQ_FUNC, FAIL);

        role = SUBSCRIBER;
        zmq_bind_fake.return_val = ZMQ_ER;
        cmp_ok(alarm__create_connection(connection, role, spec), "==", ER_ZMQ_FUNC, FAIL);
    reset_fake_funcs();
    done_testing();
}

int alarm__destroy_connection_test(
    struct Connection *connection)
{
    printf("\n\n%s\n\n", __func__);
    plan(2);
    reset_fake_funcs();
        zmq_close_fake.return_val = ZMQ_ER;
        cmp_ok(alarm__destroy_connection(connection), "==", ER_ZMQ_FUNC, FAIL);

        zmq_close_fake.return_val = ZMQ_OK;
        zmq_ctx_destroy_fake.return_val = ZMQ_ER;
        cmp_ok(alarm__destroy_connection(connection), "==", ER_ZMQ_FUNC, FAIL);
    reset_fake_funcs();
    done_testing();
}

int alarm__send_signal_test(
    struct Connection connection,
    enum Message_signal sig)
{
    printf("\n\n%s\n\n", __func__);
    plan(1);
    reset_fake_funcs();
        zmq_send_fake.return_val = ZMQ_ER;
        cmp_ok(alarm__send_signal(connection, sig), "==", ER_ZMQ_FUNC, FAIL);
    reset_fake_funcs();
    done_testing();
}

int alarm__recv_signal_test(
    struct Connection connection,
    enum Message_signal *sig)
{
    printf("\n\n%s\n\n", __func__);
    plan(1);
    reset_fake_funcs();
        zmq_recv_fake.return_val = ZMQ_ER;
        cmp_ok(alarm__recv_signal(connection, sig), "==", ER_ZMQ_FUNC, FAIL);
    reset_fake_funcs();
    done_testing();
}

int alarm__send_message_test(
    struct Connection connection,
    struct Message message)
{
    /*Context and socket cannot be equal NULL*/
    int rand_1 = 22138;
    int rand_2 = 21901;
    void *rand_ptr_on_ctx = &rand_1;
    void *rand_ptr_on_sock = &rand_2;
    connection.context = rand_ptr_on_ctx;
    connection.socket = rand_ptr_on_sock;

    printf("\n\n%s\n\n", __func__);
    plan(1);
    reset_fake_funcs();
        zmq_send_fake.return_val = ZMQ_ER;
        alarm__check_message_fake.return_val = OK;
        cmp_ok(alarm__send_message(connection, message), "==", ER_ZMQ_FUNC, FAIL);
    reset_fake_funcs();
    done_testing();
}

int alarm__recv_message_test(
    struct Connection connection,
    struct Message *message)
{
    printf("\n\n%s\n\n", __func__);
    plan(1);
    reset_fake_funcs();
        zmq_recv_fake.return_val = ZMQ_ER;
        cmp_ok(alarm__recv_message(connection, message), "==", ER_ZMQ_FUNC, FAIL);
    reset_fake_funcs();
    done_testing();
}

int main()
{
    struct Connection test_connection = {NULL, NULL};
    enum Role test_role = PUBLISHER;
    int test_spec = 0;

    enum Message_signal test_sig = SEND_MESSAGE;

    struct Message test_message =
    {
        "test module",
        NORMALIZE,
        USUALLY,
        "testmodule"
    };

    alarm__create_connection_test(&test_connection, test_role, test_spec);
    alarm__destroy_connection_test(&test_connection);
    alarm__send_signal_test(test_connection, test_sig);
    alarm__recv_signal_test(test_connection, &test_sig);
    alarm__send_message_test(test_connection, test_message);
    alarm__recv_message_test(test_connection, &test_message);

    return 0;
}
