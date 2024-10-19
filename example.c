#include <stdio.h>
#include "jsmn_ext.h"

int main()
{
    jsmn_ext jsmn;
    jsmn_obj root;
    int file_sz = 0;
    char file_str[1024] = {0};

    jsmn_obj obj0 = {0};
    jsmn_obj obj1 = {0};
    jsmn_obj obj2 = {0};
    jsmn_obj obj3 = {0};
    int size0 = 0;
    int size1 = 0;
    char str[32] = {0};

    printf("................................................\n");
    printf("A extension for jsmn, Make it easier to use.....\n");
    printf("................................................\n");

    FILE* fp = fopen("sample.json", "rb");
    if (NULL != fp) {
        if (0 != fseek(fp, 0, SEEK_END)) goto exit;
        if ((file_sz = ftell(fp)) <= 0) goto exit;
        if (0 != fseek(fp, 0, SEEK_SET)) goto exit;
        if (file_sz != fread(file_str, 1, file_sz, fp)) goto exit;
        if (JSMN_RET_OK != jsmnExt_Init(&jsmn, &root, file_str)) goto exit;

        obj0.val_str = str;
        obj0.len_str = sizeof(str);
        if (JSMN_RET_OK != jsmnExt_GetObjItem(&jsmn, "squadName", &obj0, &root)) goto exit;
        printf("squadName:%s \n", obj0.val_str);
        if (JSMN_RET_OK != jsmnExt_GetObjItem(&jsmn, "homeTown", &obj0, &root)) goto exit;
        printf("homeTown:%s \n", obj0.val_str);
        if (JSMN_RET_OK != jsmnExt_GetObjItem(&jsmn, "formed", &obj0, &root)) goto exit;
        printf("formed:%d \n", obj0.val_int);
        if (JSMN_RET_OK != jsmnExt_GetObjItem(&jsmn, "secretBase", &obj0, &root)) goto exit;
        printf("secretBase:%s \n", obj0.val_str);
        if (JSMN_RET_OK != jsmnExt_GetObjItem(&jsmn, "active", &obj0, &root)) goto exit;
        if (JSMN_EXT_TRUE != obj0.type) goto exit;
        printf("active is json true \n");
        if (JSMN_RET_OK != jsmnExt_GetObjItem(&jsmn, "members", &obj0, &root)) goto exit;
        if (JSMN_EXT_ARRAY != obj0.type) goto exit;
        size0 = jsmnExt_GetArrSize(&jsmn, &obj0);
        printf("members is json array, %d items \n", size0);

        for (int i = 0; i < size0; i++) {
            if (JSMN_RET_OK != jsmnExt_GetArrItem(&jsmn, i, &obj1, &obj0)) goto exit;
            obj2.val_str = str;
            obj2.len_str = sizeof(str);
            if (JSMN_RET_OK != jsmnExt_GetObjItem(&jsmn, "name", &obj2, &obj1)) goto exit;
            printf("name:%s \n", obj2.val_str);
            if (JSMN_RET_OK != jsmnExt_GetObjItem(&jsmn, "age", &obj2, &obj1)) goto exit;
            printf("age:%d \n", obj2.val_int);
            if (JSMN_RET_OK != jsmnExt_GetObjItem(&jsmn, "secretIdentity", &obj2, &obj1)) goto exit;
            printf("secretIdentity:%s \n", obj2.val_str);
            if (JSMN_RET_OK != jsmnExt_GetObjItem(&jsmn, "powers", &obj2, &obj1)) goto exit;
            if (JSMN_EXT_ARRAY != obj2.type) goto exit;
            size1 = jsmnExt_GetArrSize(&jsmn, &obj2);
            printf("powers is json array, %d items \n", size1);

            for (int j = 0; j < size1; j++) {
                obj3.val_str = str;
                obj3.len_str = sizeof(str);
                if (JSMN_RET_OK != jsmnExt_GetArrItem(&jsmn, j, &obj3, &obj2)) goto exit;
                printf("%s \n", obj3.val_str);
            }
        }
    }

exit:
    return 0;
}

