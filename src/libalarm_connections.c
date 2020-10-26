#include "alarm.h"

/*role: PUBLISHER/SUBSCRIBER, spec: always 0*/
int alarm__create_connection(
    struct Connection *connection,
    enum Role role,
    int spec)
{
    enum ER_code check = OK;
    void *context = NULL;
    void *socket = NULL;
    char addr_of_socket[MAX_SIZE_SOCKET_PATH] = "ipc://";
    int hwm = HIGH_WATER_MARK;

    if (NULL == connection)
    {
        printf("Error: %s(): arg[1] cannot be equal NULL\n", __func__);
        check = ER_INVALID_ARG;
        goto finally;
    }

    if (role != SUBSCRIBER && role != PUBLISHER)
    {
        printf("Error: %s(): invalid value \"role\"\n", __func__);
        check = ER_INVALID_ARG;
        goto finally;
    }

    if (0 == spec)
    {
        strcat(addr_of_socket, PATH_TO_SOCKET);
        strcat(addr_of_socket, "sock0");
    }
    else
    {
        strcat(addr_of_socket, PATH_TO_SOCKET);
        strcat(addr_of_socket, "sock1");
    }

    context = zmq_ctx_new();
    if (NULL == context)
    {
        printf("Error: create_connection: zmq_ctx_new(): %s\n",
            zmq_strerror(errno));
        check = ER_ZMQ_FUNC;
        goto finally;
    }

    socket = zmq_socket(context, role);
    if (NULL == socket)
    {
        printf ("Error: create_connection: zmq_socket(): %s\n",
            zmq_strerror(errno));
        check = ER_ZMQ_FUNC;
        goto finally;
    }

    if (SUBSCRIBER == role)
    {
        check = zmq_bind(socket, addr_of_socket);
        if (-1 == check)
        {
            printf("Error: create_connection: zmq_bind(): %s\n",
                zmq_strerror(errno));
            check = ER_ZMQ_FUNC;
            goto finally;
        }
        check = zmq_setsockopt(socket, ZMQ_SUBSCRIBE, "", 0);
        if (-1 == check)
        {
            printf("Error: create_connection: zmq_setsockopt(): %s\n",
                zmq_strerror(errno));
            check = ER_ZMQ_FUNC;
            goto finally;
        }
        check = zmq_setsockopt(socket, ZMQ_RCVHWM, &hwm, sizeof(int));
        if (-1 == check)
        {
            printf("Error: create_connection: zmq_setsockopt(): %s\n",
                zmq_strerror(errno));
            check = ER_ZMQ_FUNC;
            goto finally;
        }
    }
    else if (PUBLISHER == role)
    {
        check = zmq_connect(socket, addr_of_socket);
        if (-1 == check)
        {
            printf("Error: create_connection: zmq_connect(): %s\n",
                zmq_strerror(errno));
            check = ER_ZMQ_FUNC;
            goto finally;
        }
        check = zmq_setsockopt(socket, ZMQ_SNDHWM, &hwm, sizeof(int));
        if (-1 == check)
        {
            printf("Error: create_connection: zmq_setsockopt(): %s\n",
                zmq_strerror(errno));
            check = ER_ZMQ_FUNC;
            goto finally;
        }
    }
    else
    {
        printf("Error: create_connection: invalid arguments\n");
        check = ER_INVALID_ARG;
        goto finally;
    }

    connection->context = context;
    connection->socket = socket;

 finally:

    alarm__delay(DELAY_ITER);

    return check;
}

/*not sure about how this function works*/
int alarm__destroy_connection(
    struct Connection *connection)
{
        enum ER_code return_value = OK;

    if (NULL == connection)
    {
        printf("Error: %s(): arg[1] cannot be equal NULL\n", __func__);
        return_value = ER_INVALID_ARG;
        goto finally;
    }

    if (-1 == zmq_close(connection->socket))
    {
        return_value = ER_ZMQ_FUNC;
        printf("Error: destroy_connection(): zmq_close(): %s\n",
            zmq_strerror(errno));
        goto finally;
    }

    if (-1 == zmq_ctx_destroy(connection->context))
    {
        return_value = ER_ZMQ_FUNC;
        printf("Error: destroy_connection(): zmq_ctx_destroy(): %s\n",
            zmq_strerror(errno));
        goto finally;
    }

    connection->socket = NULL;
    connection->context = NULL;

 finally:

    return return_value;
}
