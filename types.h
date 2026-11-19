#ifndef TYPES_H
#define TYPES_H

/* User defined types */
typedef unsigned int uint;

/* Status will be used in function return type */
typedef enum
{
    e_success,
    e_failure
} Status;

/* Supported operation modes */
typedef enum
{
    e_encode,       // Encoding operation
    e_decode,       // Decoding operation
    e_unsupported   // Invalid operation
} OperationType;

#endif
