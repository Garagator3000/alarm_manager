#include "alarm.h"

int alarm__create_message(
    struct Message *message,
    char const module[],
    enum Message_type type,
    enum Message_priority priority,
    char const text[]
)
{
    enum ER_code return_value = OK;

    if (NULL == module || NULL == text)
    {
        printf("Error: %s(): module and text - not be equal NULL", __func__);
        return_value = ER_INVALID_ARG;
        goto finally;
    }
    if (type < NO_TYPE || type > NORMALIZE)
    {
        return_value = ER_INVALID_ARG;
        goto finally;
    }
    if (priority < NO_PRIORITY || priority > NOT_IMPORTANT)
    {
        printf("Error: %s(): priority - not in the range of acceptable values", __func__);
        return_value = ER_INVALID_ARG;
        goto finally;
    }
    strncpy(message->module, module, MESSAGE_MODULE_SIZE);
    strncpy(message->message_text, text, MESSAGE_TEXT_SIZE);
    message->priority = priority;
    message->type = type;

 finally:
    return return_value;
}

/*zero - not equal, higher than zero - equal*/
int alarm__message_compare(
    struct Message message,
    struct Message filter)
{
    int return_value = 0;
    int likeness = 0;
    int fields = 0;

    if (0 != strcmp(filter.module, ""))
    {
        fields = fields + 1;
        if (0 == strcmp(message.module, filter.module))
        {
            likeness = likeness + 1;
        }
    }
    if (0 != filter.type)
    {
        fields = fields + 2;
        if (message.type == filter.type)
        {
            likeness = likeness + 2;
        }
    }
    if (0 != filter.priority)
    {
        fields = fields + 3;
        if (message.priority == filter.priority)
        {
            likeness = likeness + 3;
        }
    }

    if (fields == likeness)
    {
        return_value = likeness;
    }

    return return_value;
}

/*X-iteration delay
to let zmq make a context and socket*/
void alarm__delay(int x)
{
    for (int i = 0; i < x; i++);
}

int alarm__check_message(
    struct Message message
)
{
    enum ER_code return_value = OK;

    if    (strnlen(message.module, MESSAGE_MODULE_SIZE) > sizeof(message.module)
        || strnlen(message.message_text, MESSAGE_TEXT_SIZE) > sizeof(message.message_text)
        || message.type < NO_TYPE
        || message.type > NORMALIZE
        || message.priority < NO_PRIORITY
        || message.priority > NOT_IMPORTANT)
    {
        return_value = ER_INVALID_MESSAGE;
    }

    return return_value;
}
