#ifndef RPC_PARAMETER_SERVER_H
#define RPC_PARAMETER_SERVER_H

#ifdef __cplusplus
extern "C" {
#endif

#include "rpc.h"

/* RPC used to provide dynamic parameter discovery to a client.
 *
 * It can be used for example by a GUI to display a list of all parameters.
 * Dynamic discovery means that the code of the client does not need to be
 * changed when a parameter is added to the system.
 *
 * The idea is that the client will send a serie of
 * ParameterEnumerationRequest, incrementing the index while the
 * ParameterEnumerationReply sent by the server contains a valid parameter.
 *
 * @param [in] input A protobuf encoded ParameterEnumerationRequest.
 * @param [out] output A protobuf encoded ParameterEnumerationReply.
 * @param [in] p User parameter of the RPC. Should point to the root namespace
 * of the parameter tree.
 */
void parameter_server_enumerate(void *p,
                                pb_istream_t *input,
                                pb_ostream_t *output);

/** RPC used to set a parameter from a client.
 *
 * @param [in] input A protobuf encoded ParameterSetRequest.
 * @param [in] output A protobuf encoded ParameterSetReply.
 * @param [in] p User parameter of the RPC. Should point to the root namespace
 * of the parameter tree.
 */
void parameter_server_set(void *p,
                          pb_istream_t *input,
                          pb_ostream_t *output);

#ifdef __cplusplus
}
#endif

#endif
