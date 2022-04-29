/* Automatically generated nanopb header */
/* Generated by nanopb-0.4.6-dev */

#ifndef PB_RINA_MESSAGES_APPLICATIONPROCESSNAMINGINFOMESSAGE_PB_H_INCLUDED
#define PB_RINA_MESSAGES_APPLICATIONPROCESSNAMINGINFOMESSAGE_PB_H_INCLUDED
#include <pb.h>

#if PB_PROTO_HEADER_VERSION != 40
#error Regenerate this file with the current version of nanopb generator.
#endif

/* Struct definitions */
typedef struct _rina_messages_applicationProcessNamingInfo_t { /* carries the naming information of an application process */
    char applicationProcessName[20]; 
    bool has_applicationProcessInstance;
    char applicationProcessInstance[20]; 
    bool has_applicationEntityName;
    char applicationEntityName[20]; 
    bool has_applicationEntityInstance;
    char applicationEntityInstance[20]; 
} rina_messages_applicationProcessNamingInfo_t;


#ifdef __cplusplus
extern "C" {
#endif

/* Initializer values for message structs */
#define rina_messages_applicationProcessNamingInfo_t_init_default {"", false, "", false, "", false, ""}
#define rina_messages_applicationProcessNamingInfo_t_init_zero {"", false, "", false, "", false, ""}

/* Field tags (for use in manual encoding/decoding) */
#define rina_messages_applicationProcessNamingInfo_t_applicationProcessName_tag 1
#define rina_messages_applicationProcessNamingInfo_t_applicationProcessInstance_tag 2
#define rina_messages_applicationProcessNamingInfo_t_applicationEntityName_tag 3
#define rina_messages_applicationProcessNamingInfo_t_applicationEntityInstance_tag 4

/* Struct field encoding specification for nanopb */
#define rina_messages_applicationProcessNamingInfo_t_FIELDLIST(X, a) \
X(a, STATIC,   REQUIRED, STRING,   applicationProcessName,   1) \
X(a, STATIC,   OPTIONAL, STRING,   applicationProcessInstance,   2) \
X(a, STATIC,   OPTIONAL, STRING,   applicationEntityName,   3) \
X(a, STATIC,   OPTIONAL, STRING,   applicationEntityInstance,   4)
#define rina_messages_applicationProcessNamingInfo_t_CALLBACK NULL
#define rina_messages_applicationProcessNamingInfo_t_DEFAULT NULL

extern const pb_msgdesc_t rina_messages_applicationProcessNamingInfo_t_msg;

/* Defines for backwards compatibility with code written before nanopb-0.4.0 */
#define rina_messages_applicationProcessNamingInfo_t_fields &rina_messages_applicationProcessNamingInfo_t_msg

/* Maximum encoded size of messages (where known) */
#define rina_messages_applicationProcessNamingInfo_t_size 84

#ifdef __cplusplus
} /* extern "C" */
#endif

#endif