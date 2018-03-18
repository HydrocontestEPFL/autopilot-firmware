#ifndef INPUT_MAPPER_THREAD_H
#define INPUT_MAPPER_THREAD_H

#ifdef __cplusplus
extern "C" {
#endif

/** Starts the thread mapping from raw SBUS packets to RC input channels. */
void input_mapper_start(void);

#ifdef __cplusplus
}
#endif

#endif
