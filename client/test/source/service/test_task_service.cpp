#include <gtest/gtest.h>

#include <chrono>
#include <filesystem>

#include <danejoe/database/sql_database_manager.hpp>
#include <danejoe/database/sqlite_driver.hpp>

#include "repository/task_repository.hpp"
#include "service/task_service.hpp"

namespace
{
    void setup_temp_client_database(const std::filesystem::path& db_path)
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

        std::error_code ec;
        std::filesystem::remove(db_path, ec);

        DaneJoe::SqlConfig config;
        config.database_name = "client_database";
        config.path = db_path.string();
        db->set_config(config);
        ASSERT_TRUE(db->connect());
    }

    class TaskServiceTest : public ::testing::Test
    {
    protected:
        void SetUp() override
        {
            auto db_path = std::filesystem::temp_directory_path() / "task_service_test.db";
            setup_temp_client_database(db_path);

            // Ensure table exists before service init.
            m_repo.init();
            ASSERT_TRUE(m_repo.ensure_table_exists());
        }

        TaskRepository m_repo;
    };
}

TEST_F(TaskServiceTest, InitEmptyDbStartsFromOne)
{
    TaskService service;
    service.init();

    TaskEntity t;
    t.file_id = 1;
    t.saved_path = "/tmp/test.bin";
    t.source_url = "http://example.com/test.bin";
    t.operation = Operation::Download;
    t.state = TaskState::Waiting;
    t.start_time = std::chrono::system_clock::now();
    t.end_time = t.start_time;

    ASSERT_TRUE(service.add(t));

    auto all = m_repo.get_all();
    ASSERT_EQ(all.size(), 1u);
    EXPECT_EQ(all[0].task_id, 1);
}

TEST_F(TaskServiceTest, InitUsesMaxTaskIdPlusOne)
{
    // Seed DB with a task_id=100.
    TaskEntity seed;
    seed.task_id = 100;
    seed.file_id = 1;
    seed.saved_path = "/tmp/seed.bin";
    seed.source_url = "http://example.com/seed.bin";
    seed.operation = Operation::Download;
    seed.state = TaskState::Waiting;
    seed.start_time = std::chrono::system_clock::now();
    seed.end_time = seed.start_time;
    ASSERT_TRUE(m_repo.add(seed));

    TaskService service;
    service.init();

    TaskEntity t;
    t.file_id = 2;
    t.saved_path = "/tmp/test2.bin";
    t.source_url = "http://example.com/test2.bin";
    t.operation = Operation::Download;
    t.state = TaskState::Waiting;
    t.start_time = std::chrono::system_clock::now();
    t.end_time = t.start_time;

    ASSERT_TRUE(service.add(t));

    auto inserted = m_repo.get_by_task_id(101);
    ASSERT_TRUE(inserted.has_value());
    EXPECT_EQ(inserted->file_id, 2);
}
