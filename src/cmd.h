#ifndef _CMD_H_
#define _CMD_H_

#include <shell.h>
extern const ShellCommand shell_commands[];

void shell_start(BaseSequentialStream *io);

#endif /* _CMD_H_ */
