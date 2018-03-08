#include <parameter/parameter.h>
#include <string.h>
#include "rpc_parameter_server.h"
#include "messages/RPCParameter.pb.h"

static bool parameter_type_match(uint8_t parameter_type, uint8_t request_type)
{
    if (parameter_type == _PARAM_TYPE_BOOLEAN &&
        request_type == Parameter_bool_value_tag) {
        return true;
    } else if (parameter_type == _PARAM_TYPE_INTEGER
            && request_type == Parameter_int_value_tag) {
        return true;
    } else if (parameter_type == _PARAM_TYPE_SCALAR
            && request_type == Parameter_scalar_value_tag) {
        return true;
    }

    return false;
}

static parameter_t *_parameter_find_by_index(parameter_namespace_t *root, int wanted, int *current)
{
    parameter_t *p = root->parameter_list;

    while (p) {
        if ((*current) == wanted) {
            return p;
        }
        (*current) ++;
        p = p->next;
    }

    parameter_namespace_t *ns = root->subspaces;

    while (ns != NULL) {
        p = _parameter_find_by_index(ns, wanted, current);

        if (p) {
            return p;
        }

        ns = ns->next;
    }

    return NULL;
}

static int parameter_tree_height(parameter_t *leaf)
{
    int i = 0;
    parameter_namespace_t *ns = leaf->ns;

    while (ns != NULL) {
        i++;
        ns = ns->parent;
    }

    return i;
}

void parameter_server_enumerate(void *p,
                                pb_istream_t *input,
                                pb_ostream_t *output)
{
    ParameterEnumerationRequest request;
    ParameterEnumerationReply reply = ParameterEnumerationReply_init_default;
    int current = 0;

    /* First decode the request */
    pb_decode(input, ParameterEnumerationRequest_fields, &request);

    /* Then find the indexed parameter */
    parameter_namespace_t *ns = (parameter_namespace_t *)p;
    parameter_t *param = _parameter_find_by_index(ns, request.index, &current);

    /* If a parameter was found, output its value and name. */
    if (param == NULL) {
        reply.has_parameter = false;
    } else {
        reply.has_parameter = true;

        /* Copy the parameter name */
        int height = parameter_tree_height(param);
        while (height > 1) {

            parameter_namespace_t *ns = param->ns;
            for (int i = 0; i < height - 2; i++) {
                ns = ns->parent;
            }

            /* TODO: Strcat means possible buffer overflow in case a parameter
             * name is too long. */
            strcat(reply.parameter.name, "/");
            strcat(reply.parameter.name, ns->id);

            height --;
        }

        strcat(reply.parameter.name, "/");
        strcat(reply.parameter.name, param->id);

        switch (param->type) {
            case _PARAM_TYPE_SCALAR:
                reply.parameter.which_value = Parameter_scalar_value_tag;
                reply.parameter.value.scalar_value = parameter_scalar_read(param);
                break;

            case _PARAM_TYPE_INTEGER:
                reply.parameter.which_value = Parameter_int_value_tag;
                reply.parameter.value.int_value = parameter_integer_read(param);
                break;

            case _PARAM_TYPE_BOOLEAN:
                reply.parameter.which_value = Parameter_bool_value_tag;
                reply.parameter.value.bool_value = parameter_boolean_read(param);
                break;

                /* TODO use other parameter types. */
        }

        pb_encode(output, ParameterEnumerationReply_fields, &reply);
    }
}

void parameter_server_set(void *p,
                          pb_istream_t *input,
                          pb_ostream_t *output)
{
    parameter_namespace_t *ns = (parameter_namespace_t *)p;
    ParameterSetRequest request;
    ParameterSetReply reply = ParameterSetReply_init_default;

    pb_decode(input, ParameterSetRequest_fields, &request);

    parameter_t *param = parameter_find(ns, request.parameter.name);

    if (param == NULL) {
        reply.has_error = true;
        reply.error = ParameterSetReply_Error_NOT_FOUND;
    } else if (!parameter_type_match(param->type, request.parameter.which_value)) {
        reply.has_error = true;
        reply.error = ParameterSetReply_Error_WRONG_TYPE;
    } else {
        switch (request.parameter.which_value) {
            case Parameter_int_value_tag:
                parameter_integer_set(param, request.parameter.value.int_value);
                break;
            case Parameter_bool_value_tag:
                parameter_boolean_set(param, request.parameter.value.bool_value);
                break;
            case Parameter_scalar_value_tag:
                parameter_scalar_set(param, request.parameter.value.scalar_value);
                break;
        }
    }

    /* Send the reply */
    pb_encode(output, ParameterSetReply_fields, &reply);
}
