#include "alarm.h"

int alarm__send_signal(
    struct Connection connection,
    enum Message_signal sig)
{
    enum ER_code return_value = OK;
    return_value = zmq_send(connection.socket, &sig, sizeof(sig), ZMQ_DONTWAIT);
    if (return_value < 0)
    {
        return_value = ER_ZMQ_FUNC;
    }
    return return_value;
}

int alarm__recv_signal(
    struct Connection connection,
    enum Message_signal *sig)
{
    enum ER_code return_value = OK;
    return_value = zmq_recv(connection.socket, sig, sizeof(*sig), 0);
    if (return_value < 0)
    {
        return_value = ER_ZMQ_FUNC;
    }
    return return_value;
}

int alarm__send_message(
    struct Connection connection,
    struct Message message)
{
    enum ER_code return_value = OK;

    if (NULL == connection.context || NULL == connection.socket)
    {
        printf("Error: %s(): arg[1] cannot be equal NULL\n", __func__);
        return_value = ER_INVALID_ARG;
        goto finally;
    }

    if (alarm__check_message(message) < 0)
    {
        printf("Error: %s(): your message have invalid field\n", __func__);
        return_value = ER_INVALID_ARG;
        goto finally;
    }

    return_value = alarm__send_signal(connection, SEND_MESSAGE);
    if (return_value < 0)
    {
        printf("Error: %s(): send_signal()\n", __func__);
        return_value = ER_ZMQ_FUNC;
        goto finally;
    }

    return_value = zmq_send(connection.socket, &message, sizeof(message), ZMQ_DONTWAIT);
    if (return_value < 0)
    {
        printf("Error: %s(): %s\n", __func__, zmq_strerror(errno));
        return_value = ER_ZMQ_FUNC;
    }

 finally:

    return return_value;
}

int alarm__recv_message(
    struct Connection connection,
    struct Message *message)
{
    enum ER_code return_value = OK;
    return_value = zmq_recv(connection.socket, message, sizeof(*message), 0);
    if (return_value < 0)
    {
        return_value = ER_ZMQ_FUNC;
    }
    return return_value;
}

int alarm__recv_all_message(
    struct Connection connection,
    struct Message *message_array,
    int quantity)
{
    enum ER_code return_value = OK;
    enum Message_signal sig = DEFAULT;
    struct Connection spec_connection = {NULL, NULL};
    int quantity_messages = 0;

    if (NULL == connection.context || NULL == connection.socket)
    {
        printf("Error: %s(): arg[1] cannot be equal NULL\n", __func__);
        return_value = ER_INVALID_ARG;
        goto finally;
    }

    if (NULL == message_array)
    {
        printf("Error: %s(): arg[2] cannot be equal NULL\n", __func__);
        return_value = ER_INVALID_ARG;
        goto finally;
    }

    if (quantity > MAX_QUANTITY_MESSAGES || quantity < 1)
    {
        printf("Error: %s(): arg[3] must be less than %d and more than 0\n",
                __func__, MAX_QUANTITY_MESSAGES);
        return_value = ER_INVALID_ARG;
        goto finally;
    }

    return_value = alarm__create_connection(&spec_connection, SUBSCRIBER, 1);
    if (return_value < 0)
    {
        printf("Error: %s(): create_spec_connection()\n", __func__);
        goto finally;
    }

    return_value = alarm__send_signal(connection, GET_ALL);
    if (return_value < 0)
    {
        printf("Error: %s(): send_signal()\n", __func__);
        goto finally;
    }

    /*quantity + 1, because need to receive DEFAULT signal*/
    for(int i = 0; i < quantity + 1; i++)
    {
        return_value = alarm__recv_signal(spec_connection, &sig);
        if (return_value < 0)
        {
            printf("Error: recv_signal(): %s\n", zmq_strerror(errno));
            goto finally;
        }

        if (SEND_MESSAGE == sig)
        {
            return_value = alarm__recv_message(spec_connection, &message_array[i]);
            if (return_value < 0)
            {
                printf("Error: recv_message(): %s\n", zmq_strerror(errno));
                goto finally;
            }
            quantity_messages++;
        }
        else
        {
            return_value = quantity_messages;
            if (0 == quantity_messages)
            {
                printf("%s(): No messages were received\n", __func__);
                return_value = NO_RECV_MSG;
            }
            goto finally;
        }
    }
 finally:
    if (spec_connection.context != NULL &&
        alarm__destroy_connection(&spec_connection) < 0)
    {
        printf("Error: %s(): destroy_connection\n", __func__);
    }
    return return_value;
}

int alarm__send_all_message(
    int quantity_message)
{
    enum ER_code return_value = OK;
    struct Note *note_array_for_send = NULL;
    struct Connection spec_connection = {NULL, NULL};

    return_value = alarm__create_connection(&spec_connection, PUBLISHER, 1);
    if (return_value < 0)
    {
        printf("Error: create_connection()\n");
        goto finally;
    }

    note_array_for_send = calloc((size_t)quantity_message, sizeof(*note_array_for_send));
    if (NULL == note_array_for_send)
    {
        printf("Error: %s(): calloc()\n", __func__);
        return_value = ER_ALLOC;
        goto finally;
    }

    quantity_message = alarm__read_from_file(note_array_for_send, quantity_message);
    if (quantity_message <= 0)
    {
        printf("Error: %s(): read_from_file()\n", __func__);
        return_value = ER_READ_FILE;
        goto finally;
    }

    for (int i = 0; i < quantity_message; i++)
    {
        return_value = alarm__send_message(spec_connection, note_array_for_send[i].message);
        if (return_value < 0)
        {
            printf("Error: %s(): send_message(): %s\n", __func__, zmq_strerror(errno));
            return_value = ER_ZMQ_FUNC;
            goto finally;
        }
    }

    return_value = alarm__send_signal(spec_connection, DEFAULT);
    if (return_value < 0)
    {
        printf("Error: %s(): send_signal(): %s\n", __func__, zmq_strerror(errno));
        return_value = ER_ZMQ_FUNC;
        goto finally;
    }

 finally:

    free(note_array_for_send);

    if (spec_connection.context != NULL &&
        alarm__destroy_connection(&spec_connection) < 0)
    {
        printf("Error: %s(): destroy_connection\n", __func__);
    }

    return return_value;
}

int alarm__recv_by_filter(
    struct Connection connection,
    struct Message filter,
    struct Message *message_array,
    int quantity)
{
    enum ER_code return_value = OK;
    enum Message_signal sig = DEFAULT;
    struct Connection spec_connection = {NULL, NULL};
    int quantity_messages = 0;

    if (NULL == connection.context || NULL == connection.socket)
    {
        printf("Error: %s(): arg[1] cannot be equal NULL\n", __func__);
        return_value = ER_INVALID_ARG;
        goto finally;
    }

    if (alarm__check_message(filter) < 0)
    {
        printf("Error: %s(): your message have invalid field\n", __func__);
        return_value = ER_INVALID_MESSAGE;
        goto finally;
    }

    if (NULL == message_array)
    {
        printf("Error: %s(): arg[3] cannot be equal NULL\n", __func__);
        return_value = ER_INVALID_ARG;
        goto finally;
    }

    if (quantity > MAX_QUANTITY_MESSAGES || quantity < 1)
    {
        printf("Error: %s(): arg[4] must be less than %d and more than 0\n",
                __func__, MAX_QUANTITY_MESSAGES);
        return_value = ER_INVALID_ARG;
        goto finally;
    }

    return_value = alarm__create_connection(&spec_connection, SUBSCRIBER, 1);
    if (return_value < 0)
    {
        printf("Error: %s(): create_connection()\n", __func__);
        goto finally;
    }

    return_value = alarm__send_signal(connection, GET_FILTER);
    if (return_value < 0)
    {
        printf("Error: %s(): send_signal()\n", __func__);
        goto finally;
    }

    return_value = alarm__send_message(connection, filter);
    if (return_value < 0)
    {
        printf("Error: %s(): send_message()\n", __func__);
        goto finally;
    }
    /*quantity + 1, because need to receive DEFAULT signal*/
    for (int i = 0; i < quantity + 1; i++)
    {
        return_value = alarm__recv_signal(spec_connection, &sig);
        if (return_value < 0)
        {
            printf("Error: %s(): recv_signal()\n", __func__);
            goto finally;
        }

        if (SEND_MESSAGE == sig)
        {
            return_value = alarm__recv_message(spec_connection, &message_array[i]);
            if (return_value < 0)
            {
                printf("Error: %s(): recv_message()\n", __func__);
                goto finally;
            }

            quantity_messages++;
        }
        else
        {
            return_value = quantity_messages;
            if (0 == quantity_messages)
            {
                printf("%s(): No messages were received\n", __func__);
                return_value = NO_RECV_MSG;
            }
            goto finally;
        }
    }

 finally:
    if (spec_connection.context != NULL &&
        alarm__destroy_connection(&spec_connection) < 0)
    {
        printf("Error: %s(): destroy_connection\n", __func__);
    }

    return return_value;
}

int alarm__send_by_filter(
    struct Message filter,
    int quantity)
{
    enum ER_code return_value = OK;
    struct Connection spec_connection = {NULL, NULL};
    struct Note *note_array = NULL;

    return_value = alarm__create_connection(&spec_connection, PUBLISHER, 1);
    if (return_value < 0)
    {
        printf("Error: %s(): create_connection()\n", __func__);
        goto finally;
    }

    note_array = calloc((size_t)quantity, sizeof(*note_array));
    if (NULL == note_array)
    {
        printf("Error: %s(): allocation memory error\n", __func__);
        return_value = ER_ALLOC;
        goto finally;
    }

    quantity = alarm__read_from_file(note_array, quantity);
    if (quantity <= 0)
    {
        printf("Error: %s(): the file was not read or empty\n", __func__);
        return_value = ER_READ_FILE;
        goto finally;
    }

    for (int i = 0; i < quantity; i++)
    {
        if (0 != alarm__message_compare(note_array[i].message, filter))
        {
            return_value = alarm__send_message(spec_connection, note_array[i].message);
            if (return_value < 0)
            {
                printf("Error: %s(): zmq_send: %s\n", __func__, zmq_strerror(errno));
                goto finally;
            }
        }
    }

    return_value = alarm__send_signal(spec_connection, DEFAULT);
    if(return_value < 0)
    {
        printf("Error: %s(): the last signal was not send\n", __func__);
    }

 finally:

    free(note_array);
    if (spec_connection.context != NULL &&
        alarm__destroy_connection(&spec_connection) < 0)
    {
        printf("Error: %s(): destroy_connection\n", __func__);
    }
    return return_value;
}

int alarm__send_quantity(
    int quantity)
{
    struct Connection spec_connection = {NULL, NULL};
    enum ER_code return_value = OK;

    return_value = alarm__create_connection(&spec_connection, PUBLISHER, 1);
    if (return_value < 0)
    {
        printf("Error: %s(): connection was not created\n", __func__);
        goto finally;
    }

    return_value = zmq_send(spec_connection.socket, &quantity, sizeof(quantity), 0);
    if (return_value < 0)
    {
        printf("Error: %s(): quantity was not sent\n", __func__);
        goto finally;
    }

 finally:

    if (spec_connection.context != NULL &&
        alarm__destroy_connection(&spec_connection) < 0)
    {
        printf("Error: %s(): destroy_connection\n", __func__);
    }

    return return_value;
}

int alarm__recv_quantity(
    struct Connection connection)
{
    struct Connection spec_connection = {NULL, NULL};
    int quantity = 0;
    enum ER_code return_value = OK;

    if (NULL == connection.context || NULL == connection.socket)
    {
        return_value = ER_INVALID_ARG;
        printf("Error: %s(): arg[1] cannot be equal NULL\n", __func__);
        goto finally;
    }

    return_value = alarm__create_connection(&spec_connection, SUBSCRIBER, 1);
    if (return_value < 0)
    {
        printf("Error: %s(): create_connection()\n", __func__);
        goto finally;
    }

    return_value = alarm__send_signal(connection, GET_MAX_NUMB);
    if (return_value < 0)
    {
        printf("Error: %s(): send_signal()\n", __func__);
        goto finally;
    }

    return_value = zmq_recv(spec_connection.socket, &quantity, sizeof(quantity), 0);
    if (return_value < 0)
    {
        printf("Error: %s(): zmq_recv(): %s()\n", __func__, zmq_strerror(errno));
        goto finally;
    }

    return_value = quantity;

 finally:

    if (spec_connection.context != NULL &&
        alarm__destroy_connection(&spec_connection) < 0)
    {
        printf("Error: %s(): destroy_connection\n", __func__);
    }

    return return_value;
}

int alarm__delete_all_messages(
    struct Connection connection)
{
    return alarm__send_signal(connection, DELETE_ALL);
}
