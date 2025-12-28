#include <gtest/gtest.h>

#include <filesystem>

#include <danejoe/database/sql_database_manager.hpp>
#include <danejoe/database/sqlite_driver.hpp>

#include "repository/client_file_repository.hpp"

namespace
{
    class ClientFileRepositoryTest : public ::testing::Test
    {
    protected:
        void SetUp() override
        {
            auto& database_manager = DaneJoe::SqlDatabaseManager::get_instance();
            if (!database_manager.get_database("client_database"))
            {
                database_manager.add_database("client_database", std::make_shared<DaneJoe::SqliteDriver>());
            }

            auto db = database_manager.get_database("client_database");
            ASSERT_TRUE(db);
            if (auto driver = db->get_driver())
            {
                driver->close();
            }

            auto db_path = std::filesystem::temp_directory_path() / "client_file_repository_test.db";
            std::error_code ec;
            std::filesystem::remove(db_path, ec);

            DaneJoe::SqlConfig config;
            config.database_name = "client_database";
            config.path = db_path.string();
            db->set_config(config);
            ASSERT_TRUE(db->connect());

            m_repo.init();
            ASSERT_TRUE(m_repo.ensure_table_exists());
        }

        ClientFileRepository m_repo;
    };
}

TEST_F(ClientFileRepositoryTest, Crud)
{
    EXPECT_TRUE(m_repo.get_all().empty());

    ClientFileEntity a;
    a.file_id = 1;
    a.saved_name = "a.bin";
    a.source_path = "/tmp/a.bin";
    a.saved_path = "/tmp/saved/a.bin";
    a.file_size = 123;
    a.md5_code = "md5_a";
    ASSERT_TRUE(m_repo.add(a));

    ClientFileEntity b;
    b.file_id = 2;
    b.saved_name = "b.bin";
    b.source_path = "/tmp/b.bin";
    b.saved_path = "/tmp/saved/b.bin";
    b.file_size = 456;
    b.md5_code = "md5_b";
    ASSERT_TRUE(m_repo.add(b));

    auto all = m_repo.get_all();
    ASSERT_EQ(all.size(), 2u);

    auto by_id = m_repo.get_by_id(1);
    ASSERT_TRUE(by_id.has_value());
    EXPECT_EQ(by_id->md5_code, "md5_a");

    auto by_md5 = m_repo.get_by_md5("md5_b");
    ASSERT_TRUE(by_md5.has_value());
    EXPECT_EQ(by_md5->file_id, 2);

    auto updated = *by_id;
    updated.file_size = 999;
    updated.saved_path = "/tmp/saved/a2.bin";
    ASSERT_TRUE(m_repo.update(updated));

    auto by_id2 = m_repo.get_by_id(1);
    ASSERT_TRUE(by_id2.has_value());
    EXPECT_EQ(by_id2->file_size, 999);
    EXPECT_EQ(by_id2->saved_path, "/tmp/saved/a2.bin");

    ASSERT_TRUE(m_repo.remove(2));
    EXPECT_FALSE(m_repo.get_by_id(2).has_value());
    auto all2 = m_repo.get_all();
    EXPECT_EQ(all2.size(), 1u);
}
