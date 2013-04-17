/***
 * -*- C++ -*-
 * -*- UTF-8 -*-
 *
 * Copyright (C),
 * @file
 * @brief
 * @author:
 * 		<name>		<email>
 * 		kins ren	kins.ren@me.com
 * @version
 * @date
 * @warning
 * @History:
 * 		<author>	<time>	<desc>
*/
#include <zip.h>

#include "JSCContext.h"

extern zip *apkArchive;

void onLocalFileLoad(const void *data, int size, void *arg)
{
    string *content = static_cast<string *>(arg);
    if( content && size > 0 ) {
        content->assign((const char *)data, size);
    }
}

int readFile(const string &file, LoadFileCallback callback, void *arg)
{
    struct zip_stat fstat;
    zip_stat_init(&fstat);
    /*
    int numFiles = zip_get_num_files(apkArchive);
    TRACE("file num:%d", numFiles); 
    for(int i=0; i<numFiles; i++)
    {
        const char *zipName = zip_get_name(apkArchive, i, 0);
        if( zipName != NULL )
        {
            zip_stat(apkArchive, zipName, 0, &fstat);
            TRACE("apk zip file:%s size:%d", fstat.name, fstat.size);
        }
    }
    */

    const char *fname = file.c_str();
    zip_file *zfile = zip_fopen(apkArchive, fname, 0);
    if( !zfile )
    {
        ERROR("Error opening %s from APK", fname);
        return -1;
    }

    zip_stat(apkArchive, fname, 0, &fstat);
    void *buffer = malloc(fstat.size+1);
    int numBytesRead = zip_fread(zfile, buffer, fstat.size);
    *((char *)buffer + numBytesRead) = 0;

    zip_fclose(zfile);

    if( numBytesRead != fstat.size ) {
        ERROR("Error read %s from APK", fname);
        free(buffer);
        return -1;
    }

    if( callback ) {
        callback(buffer, numBytesRead, arg);
    }

    free(buffer);

    return numBytesRead;
}

int JSCContext::loadLocalFile(const string &file, string &content)
{
    return readFile(file, onLocalFileLoad, &content);
}

void JSCContext::loadLocalFileAsync(const string &file, LoadFileCallback callback, void *arg)
{
    readFile(file, callback, arg);
}
