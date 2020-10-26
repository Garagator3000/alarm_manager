#include <unistd.h>
#include "alarm.h"

int main (int argc, char *argv[])
{
    int return_value = 0;
    int quantity = 0;
    struct Connection connection = {NULL, NULL};
    struct Message message = {0};

    return_value = alarm__create_connection(&connection, PUBLISHER, 0);
    if (0 > return_value)
    {
        goto finally;
    }

    if (2 != argc)
    {
        printf("Enter: %s (quantity_message <= %d)\n", argv[0], MAX_QUANTITY_MESSAGES);
        return_value = ER_INVALID_ARG;
        goto finally;
    }
    if (MAX_QUANTITY_MESSAGES < (quantity = atoi(argv[1]))
        || quantity < 1)
    {
        printf("Enter: %s (quantity_message <= %d)\n", argv[0], MAX_QUANTITY_MESSAGES);
        return_value = ER_INVALID_ARG;
        goto finally;
    }

    return_value = alarm__create_message(&message, argv[0], ALARM, IMPORTANT, "all fine");
    if (return_value < 0)
    {
        printf("Message not created\n");
        goto finally;
    }

    printf("send %d messages\n", quantity);

    for (int i = 0; i < quantity; i++)
    {
        return_value = alarm__send_message(connection, message);
        if (0 > return_value)
        {
            printf("Error: send_message()\n");
            goto finally;
        }
        printf("message #%d sent\n", i + 1);
    }

 finally:

    if (NULL != connection.context)
    {
        return_value = alarm__destroy_connection(&connection);
        if(0 > return_value)
        {
            printf("Error: destroy_connection\n");
        }
    }

    return return_value;
}
