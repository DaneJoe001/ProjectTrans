#include <gtest/gtest.h>

#include <cerrno>
#include <cstring>

#include "danejoe/common/status/status_code.hpp"
#include "danejoe/common/status/posix_status_detail.hpp"
#include "danejoe/network/status/posix_status_code.hpp"

namespace
{
    TEST(StatusCodeTest, DefaultConstructIsInvalid)
    {
        DaneJoe::StatusCode sc;
        EXPECT_FALSE(sc.is_valid());
        EXPECT_EQ(sc.get_status_level(), DaneJoe::StatusLevel::Unknown);
        EXPECT_FALSE(static_cast<bool>(sc));
        EXPECT_FALSE(sc.is_ok());
        EXPECT_FALSE(sc.is_branch());
        EXPECT_FALSE(sc.is_error());
        EXPECT_EQ(sc.message(), "Invalid Status");
    }

    TEST(StatusCodeTest, InvalidReturnsUnknownLevel)
    {
        DaneJoe::StatusCode sc(nullptr);
        EXPECT_FALSE(sc.is_valid());
        EXPECT_EQ(sc.get_status_level(), DaneJoe::StatusLevel::Unknown);
    }

    TEST(StatusCodeTest, InvalidDoesNotEqualNonOkStatus)
    {
        DaneJoe::StatusCode invalid;

        auto error_sc = DaneJoe::make_posix_status_code(false, "logic error");
        EXPECT_FALSE(invalid == error_sc);

        auto branch_sc = DaneJoe::make_posix_status_code(EAGAIN);
        EXPECT_FALSE(invalid == branch_sc);
    }

    TEST(StatusCodeTest, SharedPtrEqualityOverloadWorks)
    {
        auto a_detail = std::make_shared<DaneJoe::PosixStatusDetail>(EACCES);
        auto b_detail = std::make_shared<DaneJoe::PosixStatusDetail>(EACCES);
        auto c_detail = std::make_shared<DaneJoe::PosixStatusDetail>(ENOENT);

        DaneJoe::StatusCode sc(a_detail);
        EXPECT_TRUE(sc == b_detail);
        EXPECT_FALSE(sc == c_detail);
        EXPECT_FALSE(sc == std::shared_ptr<DaneJoe::IStatusDetail>(nullptr));
    }

    TEST(StatusCodeTest, NullptrDetailsCompareEqual)
    {
        DaneJoe::StatusCode a(nullptr);
        DaneJoe::StatusCode b(nullptr);
        EXPECT_TRUE(a == b);
        EXPECT_FALSE(a.is_valid());
    }

    TEST(PosixStatusDetailTest, CapturesErrnoAndMapsToError)
    {
        errno = EACCES;
        DaneJoe::PosixStatusDetail d;

        EXPECT_TRUE(d.is_error());
        EXPECT_EQ(d.get_status_level(), DaneJoe::StatusLevel::Error);
        EXPECT_EQ(d.message(), std::string(std::strerror(EACCES)));
        EXPECT_EQ(d.domain(), std::string_view("PosixStatusDetail"));
    }

    TEST(PosixStatusDetailTest, UserMessageOverridesStrerror)
    {
        DaneJoe::PosixStatusDetail d(EACCES, std::string("custom"));
        EXPECT_EQ(d.get_status_level(), DaneJoe::StatusLevel::Error);
        EXPECT_EQ(d.message(), "custom");
    }

    TEST(PosixStatusDetailTest, StatusLevelOnlyDoesNotReadErrno)
    {
        errno = EACCES;
        DaneJoe::PosixStatusDetail d(DaneJoe::StatusLevel::Branch, std::string("branch"));
        EXPECT_EQ(d.get_status_level(), DaneJoe::StatusLevel::Branch);
        EXPECT_EQ(d.message(), "branch");
    }

    TEST(PosixStatusDetailTest, BranchMappingWithDefaultSet)
    {
        DaneJoe::PosixStatusDetail d(EAGAIN);
        EXPECT_TRUE(d.is_branch());
        EXPECT_EQ(d.get_status_level(), DaneJoe::StatusLevel::Branch);
    }

    TEST(PosixStatusDetailTest, BranchSetAffectsMapping)
    {
        DaneJoe::PosixStatusSet empty_set(std::vector<int>{});
        DaneJoe::PosixStatusDetail d(EAGAIN, std::nullopt, empty_set);
        EXPECT_TRUE(d.is_error());
        EXPECT_EQ(d.get_status_level(), DaneJoe::StatusLevel::Error);
    }

    TEST(StatusCodeTest, MakePosixStatusCodeCapturesErrno)
    {
        errno = EACCES;
        auto sc = DaneJoe::make_posix_status_code();
        EXPECT_TRUE(sc.is_valid());
        EXPECT_TRUE(sc.is_error());
        EXPECT_EQ(sc.get_status_level(), DaneJoe::StatusLevel::Error);
        EXPECT_EQ(sc.message(), std::string(std::strerror(EACCES)));
    }

    TEST(StatusCodeTest, MakePosixStatusCodeFromErrnoMapsBranch)
    {
        auto sc = DaneJoe::make_posix_status_code(EAGAIN);
        EXPECT_TRUE(sc.is_valid());
        EXPECT_TRUE(sc.is_branch());
        EXPECT_EQ(sc.get_status_level(), DaneJoe::StatusLevel::Branch);
    }

    TEST(StatusCodeTest, BoolOperatorOnlyTrueForOk)
    {
        auto ok_sc = DaneJoe::make_posix_status_code(true, "ok");
        EXPECT_TRUE(ok_sc.is_ok());
        EXPECT_TRUE(static_cast<bool>(ok_sc));

        auto branch_sc = DaneJoe::make_posix_status_code(EAGAIN);
        EXPECT_FALSE(static_cast<bool>(branch_sc));

        auto err_sc = DaneJoe::make_posix_status_code(false, "err");
        EXPECT_FALSE(static_cast<bool>(err_sc));
    }

    TEST(StatusCodeTest, EqualityDelegatesToDetail)
    {
        auto a = DaneJoe::make_posix_status_code(EACCES);
        auto b = DaneJoe::make_posix_status_code(EACCES);
        auto c = DaneJoe::make_posix_status_code(ENOENT);

        EXPECT_TRUE(a == b);
        EXPECT_FALSE(a == c);
    }
}
