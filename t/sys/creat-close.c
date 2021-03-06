/*
 * Copyright (c) 2018, Lawrence Livermore National Security, LLC.
 * Produced at the Lawrence Livermore National Laboratory.
 *
 * Copyright 2018, UT-Battelle, LLC.
 *
 * LLNL-CODE-741539
 * All rights reserved.
 *
 * This is the license for UnifyFS.
 * For details, see https://github.com/LLNL/UnifyFS.
 * Please read https://github.com/LLNL/UnifyFS/LICENSE for full license text.
 */

#include <errno.h>
#include <fcntl.h>
#include <string.h>
#include <sys/types.h>
#include <sys/stat.h>
#include "t/lib/tap.h"
#include "t/lib/testutil.h"

/* This function contains the tests for UNIFYFS_WRAP(creat) and
 * UNIFYFS_WRAP(close) found in client/src/unifyfs-sysio.c.
 *
 * Notice the tests are ordered in a logical testing order. Changing the order
 * or adding new tests in between two others could negatively affect the
 * desired results. */
int creat_close_test(char* unifyfs_root)
{
    /* Diagnostic message for reading and debugging output */
    diag("Starting UNIFYFS_WRAP(creat/close) tests");

    char path[64];
    int mode = 0600;
    int fd = -1;

    errno = 0;

    /* Create a random file name at the mountpoint path to test on */
    testutil_rand_path(path, sizeof(path), unifyfs_root);

    /* Verify closing a non-existent file fails with errno=EBADF */
    ok(close(fd) == -1 && errno == EBADF,
       "%s:%d close non-existing file %s should fail (errno=%d): %s",
       __FILE__, __LINE__, path, errno, strerror(errno));
    errno = 0; /* Reset errno after test for failure */

    /* Verify we can create a non-existent file. */
    fd = creat(path, mode);
    ok(fd >= 0, "%s:%d creat non-existing file %s (fd=%d): %s",
       __FILE__, __LINE__, path, fd, strerror(errno));

    /* Verify close succeeds. */
    ok(close(fd) == 0, "%s:%d close new file: %s",
       __FILE__, __LINE__, strerror(errno));

    /* Verify creating an already created file succeeds. */
    fd = creat(path, mode);
    ok(fd >= 0, "%s:%d creat existing file %s (fd=%d): %s",
       __FILE__, __LINE__, path, fd, strerror(errno));

    /* Verify close succeeds. */
    ok(close(fd) == 0, "%s:%d close %s: %s",
       __FILE__, __LINE__, path, strerror(errno));

    /* Verify closing already closed file fails with errno=EBADF */
    ok(close(fd) == -1 && errno == EBADF,
       "%s:%d close already closed file %s should fail (errno=%d): %s",
       __FILE__, __LINE__, path, errno, strerror(errno));
    errno = 0;

    /* CLEANUP
     *
     * Don't delete the file at path as the final test (9020-mountpoint-empty)
     * checks if anything files are found in the mountpoint, meaning creat
     * wasn't wrapped properly. */

    diag("Finished UNIFYFS_WRAP(creat/close) tests");

    return 0;
}
