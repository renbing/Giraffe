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

int JSCContext::loadLocalFile(const string &file, string &content)
{
    return 0;
}

void JSCContext::loadLocalFileAsync(const string &file, LoadFileCallback callback, void *arg)
{
    const char *fname = file.c_str();
    zip_file *zfile = zip_fopen(apkArchive, fname, 0);
    if( !zfile )
    {
        ERROR("Error opening %s from APK", fname);
        return;
    }

    struct zip_stat fstat;
    zip_stat_init(&fstat);

    zip_stat(apkArchive, fname, 0, &fstat);
    void *buffer = malloc(fstat.size);
    int numBytesRead = zip_fread(zfile, buffer, fstat.size);
    zip_fclose(zfile);

    if( numBytesRead != fstat.size ) {
        ERROR("Error read %s from APK", fname);
        return;
    }
}
