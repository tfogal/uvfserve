#ifndef UVFSERVER_ALL_COMMANDS_H
#define UVFSERVER_ALL_COMMANDS_H

#include "command.h"

void disconnect(cmd_arg& arg); // end the session
void list(cmd_arg& arg); // get a list of the files on the server
void sendf(cmd_arg& arg); // send file

#endif /* UVFSERVER_ALL_COMMANDS_H */
