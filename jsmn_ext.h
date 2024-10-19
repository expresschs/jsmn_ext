#ifndef _JSMN_EXT_H
#define _JSMN_EXT_H

#include <string.h>
#include <stdlib.h>

#define JSMN_PARENT_LINKS
#define JSMN_TOKEN_MAX          (128U)
#include "jsmn/jsmn.h"

#define JSMN_RET_OK             (0U)
#define JSMN_RET_ERR            (1U)

typedef enum {
    JSMN_EXT_INVALID = 0,
    JSMN_EXT_FALSE = 1,
    JSMN_EXT_TRUE = 2,
    JSMN_EXT_NULL = 3,
    JSMN_EXT_NUMBER = 4,
    JSMN_EXT_STRING = 5,
    JSMN_EXT_ARRAY = 6,
    JSMN_EXT_OBJECT = 7,
} jsmn_ext_type;

typedef struct {
    jsmn_parser p;
    jsmntok_t t[JSMN_TOKEN_MAX];
    int token_num;
    const char *str; 
    size_t len;
    int error;
} jsmn_ext;

typedef struct {
    jsmn_ext_type type;
    int idx;

    double val_double;
    int val_int;
    char *val_str;
    int len_str;
} jsmn_obj;

static inline int _jsmnExt_KeyCmp(const char *key, const char *str, int start, int end)
{
    int ret = JSMN_RET_OK;

    for (int i = 0; i < end - start; i++) {
        if (*(key + i) != *(str + start + i)) {
            ret = JSMN_RET_ERR;
            goto exit;
        }
    }
    if ('\0' != *(key + end - start)) {
        ret = JSMN_RET_ERR;
        goto exit;
    }

exit:
    return ret;
}

static inline int _jsmnExt_GetItem(jsmn_ext *jsmn, int idx, jsmn_obj *obj)
{
    int ret = JSMN_RET_ERR;

    if (NULL == jsmn || 0 > idx || NULL == obj) {
        return ret;;
    }

    switch (jsmn->t[idx].type) {
        case JSMN_UNDEFINED:
            obj->idx = idx;
            obj->type = JSMN_EXT_INVALID;
            break;
        case JSMN_OBJECT:
            obj->idx = idx;
            obj->type = JSMN_EXT_OBJECT;
            ret = JSMN_RET_OK;
            break;
        case JSMN_ARRAY:
            obj->idx = idx;
            obj->type = JSMN_EXT_ARRAY;
            ret = JSMN_RET_OK;
            break;
        case JSMN_STRING: {
            obj->idx = idx;
            obj->type = JSMN_EXT_STRING;
            int len = jsmn->t[idx].end - jsmn->t[idx].start;
            const char *str = jsmn->str + jsmn->t[idx].start;
            if (obj->len_str >= len + 1) {
                memcpy(obj->val_str, str, len);
                *(obj->val_str + len) = '\0';
                ret = JSMN_RET_OK;
            }
            break;
        }
        case JSMN_PRIMITIVE: {
            double number = 0;
            char *after_end = NULL;
            const char *str = jsmn->str + jsmn->t[idx].start;
            if (0 == strncmp(str, "true", 4)) {
                obj->idx = idx;
                obj->type = JSMN_EXT_TRUE;
                ret = JSMN_RET_OK;
            } else if (0 == strncmp(str, "false", 5)) {
                obj->idx = idx;
                obj->type = JSMN_EXT_FALSE;
                ret = JSMN_RET_OK;
            } else if (0 == strncmp(str, "null", 4)) {
                obj->idx = idx;
                obj->type = JSMN_EXT_NULL;
                ret = JSMN_RET_OK;
            } else {
                number = strtod(str, (char**)&after_end);
                if (str != after_end) {
                    obj->val_double = number;
                    obj->val_int = (int)number;
                    obj->idx = idx;
                    obj->type = JSMN_EXT_NUMBER;
                    ret = JSMN_RET_OK;
                } else {
                    obj->idx = idx;
                    obj->type = JSMN_EXT_INVALID;
                }
            }
            break;
        }
    }

    return ret;
}

static int jsmnExt_GetObjItem(jsmn_ext *jsmn, const char *key, jsmn_obj *obj, jsmn_obj *parent)
{
    int ret = JSMN_RET_ERR;
    jsmn_obj _parent = {0};

    if (NULL == jsmn || NULL == key || NULL == obj || NULL == parent) {
        goto exit;
    }

    memcpy(&_parent, parent, sizeof(jsmn_obj));
    obj->idx = 0;
    obj->type = JSMN_EXT_INVALID;
    int idx = _parent.idx;
    for (int i = idx; i < jsmn->token_num; i++) {
        if (idx == jsmn->t[i].parent) {
            if (JSMN_RET_OK == _jsmnExt_KeyCmp(key, jsmn->str, jsmn->t[i].start, jsmn->t[i].end)) {
                ret = _jsmnExt_GetItem(jsmn, i + 1, obj);
                goto exit;;
            }
        }
    }

exit:
    return ret;
}

static int jsmnExt_GetArrItem(jsmn_ext *jsmn, int idx, jsmn_obj *obj, jsmn_obj *parent)
{
    int ret = JSMN_RET_ERR;
    int _idx = 0;
    jsmn_obj _parent = {0};

    if (NULL == jsmn || NULL == obj || NULL == parent || JSMN_EXT_ARRAY != parent->type) {
        goto exit;
    }

    memcpy(&_parent, parent, sizeof(jsmn_obj));
    obj->idx = 0;
    obj->type = JSMN_EXT_INVALID;
    for (int i = _parent.idx; i < jsmn->token_num; i++) {
        if (_parent.idx == jsmn->t[i].parent) {
            if (idx == _idx++) {
                ret = _jsmnExt_GetItem(jsmn, i, obj);
                goto exit;
            }
        }
    }

exit:
    return ret;
}

static int jsmnExt_GetArrSize(jsmn_ext *jsmn, jsmn_obj *obj)
{
    int size = 0;

    if (NULL == jsmn || NULL == obj || JSMN_EXT_ARRAY != obj->type) {
        return -1;
    }

    for (int i = obj->idx; i < jsmn->token_num; i++) {
        if (obj->idx == jsmn->t[i].parent) {
            size += 1;
        }
    }

    return size;
}

static int jsmnExt_Init(jsmn_ext *jsmn, jsmn_obj *root, const char *str)
{
    int ret = JSMN_RET_ERR;
    int token_num = 0;

    if (NULL != str && NULL != root && NULL != str) {
        jsmn->str = str;
        jsmn->len = strlen((const char*)str);
        jsmn_init(&jsmn->p);
        token_num = jsmn_parse(&jsmn->p, jsmn->str, jsmn->len, jsmn->t, sizeof(jsmn->t)/sizeof(jsmn->t[0]));
        if ((token_num > 0) && (jsmn->t[0].type == JSMN_OBJECT)) {
            jsmn->token_num = token_num;
            root->idx = 0;
            root->type = JSMN_EXT_OBJECT;
            ret = JSMN_RET_OK;
        }
    }

    return ret;
}

#endif /* JSMN_EXT_H */

