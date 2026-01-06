#include <gtest/gtest.h>

#include <cerrno>
#include <cstring>
#include <memory>

#include "danejoe/common/status/status_code.hpp"
#include "danejoe/common/status/posix_status_detail.hpp"

namespace
{
    TEST(PosixErrorDetailTest, CapturesErrno)
    {
        errno = EACCES;
        DaneJoe::PosixStatusDetail d;

        EXPECT_TRUE(d.is_error());
        EXPECT_FALSE(static_cast<bool>(d));
        EXPECT_STREQ(d.message().c_str(), std::strerror(EACCES));
    }

    TEST(PosixErrorDetailTest, EqualityUsesCapturedCode)
    {
        errno = EACCES;
        DaneJoe::PosixStatusDetail a;

        errno = EACCES;
        DaneJoe::PosixStatusDetail b;

        errno = ENOENT;
        DaneJoe::PosixStatusDetail c;

        EXPECT_TRUE(a == static_cast<const DaneJoe::IStatusDetail&>(b));
        EXPECT_FALSE(a == static_cast<const DaneJoe::IStatusDetail&>(c));
    }

    TEST(ErrorCodeTest, EqualityDelegatesToDetail)
    {
        errno = EACCES;
        DaneJoe::StatusCode ec1(std::make_shared<DaneJoe::PosixStatusDetail>());

        errno = EACCES;
        DaneJoe::StatusCode ec2(std::make_shared<DaneJoe::PosixStatusDetail>());

        errno = ENOENT;
        DaneJoe::StatusCode ec3(std::make_shared<DaneJoe::PosixStatusDetail>());

        EXPECT_TRUE(ec1 == ec2);
        EXPECT_FALSE(ec1 == ec3);
    }

    TEST(ErrorCodeTest, NullptrDetailsCompareEqual)
    {
        DaneJoe::StatusCode a(nullptr);
        DaneJoe::StatusCode b(nullptr);

        EXPECT_TRUE(a == b);
        EXPECT_FALSE(static_cast<bool>(a));
        EXPECT_FALSE(a.is_error());
    }
}
