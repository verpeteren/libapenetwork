/*
   Copyright 2016 Nidium Inc. All rights reserved.
   Use of this source code is governed by a MIT license
   that can be found in the LICENSE file.
*/

#include <stdlib.h>
#include <string.h>
#include <stdio.h>

#include "unittest.h"

#include <ape_netlib.h>
#include <ape_log.h>


static void loglog(void *ctx, void *cb_args, ape_log_lvl_t lvl, const char * tag,
    const char * buffer )
{
    FILE *file_h = (FILE *)cb_args;
    if (file_h) {
        int datelen;
        char date[32];
        time_t log_ts;

        log_ts = time(NULL);
        datelen = strftime(&date[0], 32, "%Y-%m-%d %H:%M:%S - ", localtime(&log_ts));
        fprintf(file_h, "%s\t%s\n", date, buffer);
    }
}

static void *loginit(void *ctx)
{
    struct args * args;
    const char * tmp_file = "/tmp/test_ape_logging.log";

    FILE *file_h = fopen(tmp_file, "wb");

    fprintf(file_h, "writing stuff\n");

    return file_h;
}

static void logclear (void *ctx, void * cb_args)
{
    FILE *file_h = (FILE *)cb_args;

    if (file_h) {
        fprintf(file_h, "clearing log\n");
        fflush(file_h);
        fclose(file_h);
    }
}

TEST(Logger,  Logger)
{
    ape_global *g_ape;
    g_ape = APE_init();
    ape_logger_t logger;
    int fwd;

    memset(&logger, 0, sizeof(logger));
    APE_setlogger(&logger, APE_LOG_ERROR, loginit, loglog, logclear, NULL);

    EXPECT_TRUE(logger.lvl == APE_LOG_ERROR);
    EXPECT_TRUE(logger.init == loginit);
    EXPECT_TRUE(logger.log == loglog);
    EXPECT_TRUE(logger.clear == logclear);
    EXPECT_TRUE(logger.cb_args == &args);

    fwd = APE_logf(&logger, APE_LOG_ERROR, "tag", "should %s print", "indeed");
    EXPECT_TRUE(fwd == 1);
    fwd = APE_logf(&logger, APE_LOG_INFO, "tag", "should %s print", "not");
    EXPECT_TRUE(fwd == 0);
    fwd = APE_log(&logger, APE_LOG_ERROR, "tag", "should print");
    EXPECT_TRUE(fwd == 1);
    fwd = APE_log(&logger, APE_LOG_INFO, "tag", "should not print");
    EXPECT_TRUE(fwd == 0);

    logger.cleanup(logger.ctx, logger.cb_args);
}

