#ifndef MAIN_H
#define MAIN_H

#ifdef __cplusplus
extern "C" {
#endif

#include <parameter/parameter.h>
#include <msgbus/messagebus.h>

extern parameter_namespace_t parameter_root;
extern messagebus_t bus;

#ifdef __cplusplus
}
#endif

#endif
