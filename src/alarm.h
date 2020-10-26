#ifndef ALARM_H
#define ALARM_H

#include <zmq.h>
#include <stdio.h>
#include <stdlib.h>
#include <time.h>
#include <string.h>

static char const PATH_TO_FILE[] = "../messages.txt";
static char const PATH_TO_SOCKET[] = "/tmp/alarm_manager/";

typedef enum ER_code
{
    OK = 0,
    NOTHING_WRITE = -1,
    NO_RECV_MSG = -2,
    ER_ZMQ_FUNC = -3,
    ER_INVALID_ARG = -4,
    ER_INVALID_MESSAGE = -5,
    ER_FILE_NOPEN = -6,
    ER_FILE_NCLOSE = -7,
    ER_FGETS = -8,
    ER_ALLOC = -10,
    ER_READ_FILE = -11,
} ER_CODE;

typedef enum Not_magic_number
{
    MESSAGE_SIZE = 128,
    DATE_TIME_MAX_SIZE = 30,
    MESSAGE_MODULE_SIZE = 60,
    MESSAGE_TEXT_SIZE = 60,
    /*sizeof "very important"*/
    READ_BUFF_SIZE = 15,
    MAX_QUANTITY_MESSAGES = 8000,
    HIGH_WATER_MARK = 100000000,
    MAX_SIZE_SOCKET_PATH = 64,
    DELAY_ITER = 1000000,
} Not_magic_number;

typedef enum Role
{
    PUBLISHER = ZMQ_PUB,
    SUBSCRIBER = ZMQ_SUB,
} Role;

typedef enum Message_type
{
    NO_TYPE = 0,
    ALARM = 1,
    NORMALIZE = 2,
} Message_type;

typedef enum Message_priority
{
    NO_PRIORITY = 0,
    VERY_IMPORTANT = 1,
    IMPORTANT = 2,
    USUALLY = 3,
    NOT_IMPORTANT = 4,
} Message_priority;


typedef enum Message_signal
{
    SEND_MESSAGE = 0,
    DELETE_ALL = 1,
    GET_ALL = 2,
    GET_FILTER = 3,
    GET_MAX_NUMB = 4,
    DEFAULT = 16,
} Message_signal;

typedef struct Message
{
    char module[MESSAGE_MODULE_SIZE];
    enum Message_type type;
    enum Message_priority priority;
    char message_text[MESSAGE_TEXT_SIZE];
} Message;

typedef struct Note
{
    struct tm registration_time;
    struct Message message;
} Note;

typedef struct Connection
{
    void *context;
    void *socket;
} Connection;

int alarm__create_connection(
    struct Connection *connection,
    enum Role role,
    int spec);

int alarm__destroy_connection(
    struct Connection *connection);

int alarm__write_to_file(
    struct Note note);

int alarm__read_from_file(
    struct Note *note_array,
    int size);

int alarm__send_signal(
    struct Connection connection,
    enum Message_signal sig);

int alarm__recv_signal(
    struct Connection connection,
    enum Message_signal *sig);

int alarm__send_message(
    struct Connection connection,
    struct Message message);

int alarm__recv_message(
    struct Connection connection,
    struct Message *message);

int alarm__recv_all_message(
    struct Connection connection,
    struct Message *message_array,
    int quantity);

int alarm__send_all_message(
    int max_quantity_message);

int alarm__delete_my_messages(void);

int alarm__recv_by_filter(
    struct Connection connection,
    struct Message filter,
    struct Message *message_array,
    int quantity);

int alarm__send_by_filter(
    struct Message filter,
    int quantity);

int alarm__message_compare(
    struct Message message,
    struct Message filter);

int alarm__send_quantity(
    int quantity);

int alarm__recv_quantity(
    struct Connection connection);

void alarm__delay(
    int x);

int alarm__open_file_first_time(
    struct Note *note_array,
    int max_quantity_messages);

int alarm__delete_all_messages(
    struct Connection connection
);

int alarm__check_message(
    struct Message message
);

int alarm__create_message(
    struct Message *message,
    char const module[],
    enum Message_type type,
    enum Message_priority priority,
    char const text[]
);


#endif
