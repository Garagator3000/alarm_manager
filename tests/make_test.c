#include <tap.h>
#include "../src/alarm.h"

int main()
{
    char SUCCESS[] = "Завершилась УСПЕШНО";
    char FAIL_CONNECTION[] = "Завершилась С ОШИБКОЙ: Не валидный Connection";
    char FAIL_ROLE[] = "Завершилась С ОШИБКОЙ: Не валидный Role";
    char FAIL_MESSAGE[] = "Завершилась С ОШИБКОЙ: Не валидный Message";
    char FAIL_MESSAGE_ARRAY[] = "Завершилась С ОШИБКОЙ: Не валидный указатель на Message";
    char FAIL_SIZE[] = "Завершилась С ОШИБКОЙ: Не валидный quantity";

    size_t size_array = 10;
    size_t fail_size_array_1 = MAX_QUANTITY_MESSAGES + 100;
    size_t fail_size_array_2 = -11;

    struct Connection test_connection = {NULL, NULL};
    struct Connection null_connection = {NULL, NULL};
    struct Connection *fail_connection = NULL;

    struct Message test_message =
    {
        "test module",
        NORMALIZE,
        IMPORTANT,
        "test text"
    };
    struct Message fail_message =
    {
        "fail message fail message fail message fail message fail message fail message fail message",
        120355,
        -22,
        "failmessagefailmessagefailmessagefailmessagefailmessagefailmessagefailmessagefailmessage"
    };

    struct Message *test_message_array = calloc(size_array, sizeof(struct Message));
    if (NULL == test_message_array)
    {
        printf("\nНе удалось выделить память под test_message_array\n");
        exit(-1);
    }

    struct Message *fail_message_array = NULL;

    plan(14);

        printf("\n\nТесты с неправильными аргументами...\n\n");

        cmp_ok(alarm__create_connection(fail_connection, SUBSCRIBER, 0), "==", ER_INVALID_ARG, FAIL_CONNECTION);
        cmp_ok(alarm__create_connection(&test_connection, 1234, 0), "==", ER_INVALID_ARG, FAIL_ROLE);

        alarm__create_connection(&test_connection, PUBLISHER, 0);

        cmp_ok(alarm__send_message(null_connection, test_message), "==", ER_INVALID_ARG, FAIL_CONNECTION);
        cmp_ok(alarm__send_message(test_connection, fail_message), "==", ER_INVALID_ARG, FAIL_MESSAGE);

        cmp_ok(alarm__recv_quantity(null_connection), "==", ER_INVALID_ARG, FAIL_CONNECTION);

        cmp_ok(alarm__recv_all_message(null_connection, test_message_array, size_array), "==", ER_INVALID_ARG, FAIL_CONNECTION);
        cmp_ok(alarm__recv_all_message(test_connection, fail_message_array, size_array), "==", ER_INVALID_ARG, FAIL_MESSAGE_ARRAY);
        cmp_ok(alarm__recv_all_message(test_connection, test_message_array, fail_size_array_1), "==", ER_INVALID_ARG, FAIL_SIZE);
        cmp_ok(alarm__recv_all_message(test_connection, test_message_array, fail_size_array_2), "==", ER_INVALID_ARG, FAIL_SIZE);

        cmp_ok(alarm__recv_by_filter(null_connection, test_message, test_message_array, size_array), "==", ER_INVALID_ARG, FAIL_CONNECTION);
        cmp_ok(alarm__recv_by_filter(test_connection, fail_message, test_message_array, size_array), "==", ER_INVALID_MESSAGE, FAIL_MESSAGE);
        cmp_ok(alarm__recv_by_filter(test_connection, test_message, fail_message_array, size_array), "==", ER_INVALID_ARG, FAIL_MESSAGE_ARRAY);
        cmp_ok(alarm__recv_by_filter(test_connection, test_message, fail_message_array, fail_size_array_1), "==", ER_INVALID_ARG, FAIL_SIZE);
        cmp_ok(alarm__recv_by_filter(test_connection, test_message, test_message_array, fail_size_array_2), "==", ER_INVALID_ARG, FAIL_SIZE);


    free(test_message_array);
    alarm__destroy_connection(&test_connection);
    printf("\nТесты завершены\n");
    printf("Если тесты не прошли, то 'make clean'\n\n");
    done_testing();
}
