#include <gtest/gtest.h>

#include <cerrno>

#if DANEJOE_PLATFORM_LINUX==1
#include <unistd.h>
#endif

#include "danejoe/common/handle/unique_handle.hpp"

namespace
{
#if DANEJOE_PLATFORM_LINUX==1
TEST(UniqueHandleTest, DestructorClosesFd)
{
    int fds[2] = {-1, -1};
    ASSERT_EQ(::pipe(fds), 0);

    const int read_fd = fds[0];
    const int write_fd = fds[1];

    {
        DaneJoe::UniqueHandle<int> h(read_fd);
        EXPECT_TRUE(h.is_valid());
        EXPECT_EQ(h.get(), read_fd);
    }

    errno = 0;
    EXPECT_EQ(::close(read_fd), -1);
    EXPECT_EQ(errno, EBADF);

    ::close(write_fd);
}

TEST(UniqueHandleTest, MoveTransfersOwnership)
{
    int fds[2] = {-1, -1};
    ASSERT_EQ(::pipe(fds), 0);

    DaneJoe::UniqueHandle<int> a(fds[0]);
    DaneJoe::UniqueHandle<int> b(std::move(a));

    EXPECT_FALSE(a.is_valid());
    EXPECT_TRUE(b.is_valid());

    ::close(fds[1]);
}

TEST(UniqueHandleTest, ReleaseStopsAutomaticClose)
{
    int fds[2] = {-1, -1};
    ASSERT_EQ(::pipe(fds), 0);

    int released = -1;
    {
        DaneJoe::UniqueHandle<int> h(fds[0]);
        released = h.release();
        EXPECT_FALSE(h.is_valid());
        EXPECT_EQ(released, fds[0]);
    }

    errno = 0;
    EXPECT_EQ(::close(released), 0);

    ::close(fds[1]);
}

TEST(UniqueHandleTest, ResetClosesOldHandle)
{
    int fds1[2] = {-1, -1};
    ASSERT_EQ(::pipe(fds1), 0);

    int fds2[2] = {-1, -1};
    ASSERT_EQ(::pipe(fds2), 0);

    const int old_fd = fds1[0];
    const int new_fd = fds2[0];

    DaneJoe::UniqueHandle<int> h(old_fd);
    h.reset(new_fd);

    errno = 0;
    EXPECT_EQ(::close(old_fd), -1);
    EXPECT_EQ(errno, EBADF);

    ::close(fds1[1]);
    ::close(fds2[1]);
}
#endif
}
