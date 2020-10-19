#include <unistd.h>
#include "message.h"

int main (int argc, char *argv[])
{
    struct Connection connection = create_connection(ZMQ_PUB, 1, 0);

    struct Message *messages = (Message*)calloc(atoi(argv[1]), sizeof(Message));

    recv_meassage(connection, messages);
    sleep(1);
    for(int i = 0; i < atoi(argv[1]); i++)
    {
        printf("%s\n%d\n%d\n%s\n", messages[i].modul, messages[i].type, messages[i].priority, messages[i].message_text);
    }

    return 0;
}