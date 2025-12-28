#include <gtest/gtest.h>

#include <chrono>
#include <filesystem>

#include <danejoe/database/sql_database_manager.hpp>
#include <danejoe/database/sqlite_driver.hpp>

#include "repository/task_repository.hpp"

namespace
{
    class TaskRepositoryTest : public ::testing::Test
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

            auto db_path = std::filesystem::temp_directory_path() / "task_repository_test.db";
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

        TaskRepository m_repo;
    };
}

TEST_F(TaskRepositoryTest, Crud)
{
    EXPECT_TRUE(m_repo.get_all().empty());

    TaskEntity t1;
    t1.task_id = 100;
    t1.file_id = 1;
    t1.operation = Operation::Download;
    t1.state = TaskState::Waiting;
    t1.start_time = std::chrono::system_clock::now();
    t1.end_time = t1.start_time;
    ASSERT_TRUE(m_repo.add(t1));

    TaskEntity t2;
    t2.task_id = 101;
    t2.file_id = 1;
    t2.operation = Operation::Upload;
    t2.state = TaskState::Waiting;
    t2.start_time = std::chrono::system_clock::now();
    t2.end_time = t2.start_time;
    ASSERT_TRUE(m_repo.add(t2));

    auto all = m_repo.get_all();
    ASSERT_EQ(all.size(), 2u);

    auto by_id = m_repo.get_by_task_id(100);
    ASSERT_TRUE(by_id.has_value());
    EXPECT_EQ(by_id->operation, Operation::Download);

    auto by_file = m_repo.get_by_file_id(1);
    ASSERT_EQ(by_file.size(), 2u);

    auto updated = *by_id;
    updated.state = TaskState::Completed;
    updated.end_time = std::chrono::system_clock::now();
    ASSERT_TRUE(m_repo.update(updated));

    auto by_id2 = m_repo.get_by_task_id(100);
    ASSERT_TRUE(by_id2.has_value());
    EXPECT_EQ(by_id2->state, TaskState::Completed);

    ASSERT_TRUE(m_repo.remove(101));
    EXPECT_FALSE(m_repo.get_by_task_id(101).has_value());
    auto all2 = m_repo.get_all();
    EXPECT_EQ(all2.size(), 1u);
}
