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

    TaskEntity make_task_entity(
        int64_t file_id,
        std::string saved_path,
        std::string source_url,
        Operation operation,
        TaskState state)
    {
        TaskEntity t;
        t.file_id = file_id;
        t.saved_path = std::move(saved_path);
        t.source_url = std::move(source_url);
        t.operation = operation;
        t.state = state;
        t.start_time = std::chrono::system_clock::now();
        t.end_time = t.start_time;
        return t;
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

    TaskEntity t = make_task_entity(
        1,
        "/tmp/test.bin",
        "http://example.com/test.bin",
        Operation::Download,
        TaskState::Waiting);

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

    TaskEntity t = make_task_entity(
        2,
        "/tmp/test2.bin",
        "http://example.com/test2.bin",
        Operation::Download,
        TaskState::Waiting);

    ASSERT_TRUE(service.add(t));

    auto inserted = m_repo.get_by_task_id(101);
    ASSERT_TRUE(inserted.has_value());
    EXPECT_EQ(inserted->file_id, 2);
}

TEST_F(TaskServiceTest, AddAndGetByTaskId)
{
    TaskService service;
    service.init();

    TaskEntity t = make_task_entity(
        10,
        "/tmp/add_get.bin",
        "http://example.com/add_get.bin",
        Operation::Download,
        TaskState::Waiting);

    ASSERT_TRUE(service.add(t));
    ASSERT_GT(t.task_id, 0);

    auto found = service.get_by_task_id(t.task_id);
    ASSERT_TRUE(found.has_value());
    EXPECT_EQ(found->task_id, t.task_id);
    EXPECT_EQ(found->file_id, 10);
    EXPECT_EQ(found->saved_path, "/tmp/add_get.bin");
    EXPECT_EQ(found->source_url, "http://example.com/add_get.bin");
    EXPECT_EQ(found->operation, Operation::Download);
    EXPECT_EQ(found->state, TaskState::Waiting);
}

TEST_F(TaskServiceTest, UpdatePersistsChanges)
{
    TaskService service;
    service.init();

    TaskEntity t = make_task_entity(
        20,
        "/tmp/update.bin",
        "http://example.com/update.bin",
        Operation::Download,
        TaskState::Waiting);
    ASSERT_TRUE(service.add(t));

    auto before = service.get_by_task_id(t.task_id);
    ASSERT_TRUE(before.has_value());
    EXPECT_EQ(before->state, TaskState::Waiting);

    auto updated = before.value();
    updated.state = TaskState::Completed;
    updated.end_time = std::chrono::system_clock::now();
    ASSERT_TRUE(service.update(updated));

    auto after = service.get_by_task_id(t.task_id);
    ASSERT_TRUE(after.has_value());
    EXPECT_EQ(after->state, TaskState::Completed);
}

TEST_F(TaskServiceTest, RemoveDeletesTask)
{
    TaskService service;
    service.init();

    TaskEntity t = make_task_entity(
        30,
        "/tmp/remove.bin",
        "http://example.com/remove.bin",
        Operation::Download,
        TaskState::Waiting);
    ASSERT_TRUE(service.add(t));

    ASSERT_TRUE(service.remove(t.task_id));
    EXPECT_FALSE(service.get_by_task_id(t.task_id).has_value());
}

TEST_F(TaskServiceTest, UpdateNonExistentTaskReturnsTrueButDoesNotInsertOrModify)
{
    TaskService service;
    service.init();

    EXPECT_TRUE(m_repo.get_all().empty());

    TaskEntity non_existent = make_task_entity(
        40,
        "/tmp/non_existent.bin",
        "http://example.com/non_existent.bin",
        Operation::Download,
        TaskState::Completed);
    non_existent.task_id = 999;
    non_existent.end_time = std::chrono::system_clock::now();

    ASSERT_TRUE(service.update(non_existent));

    EXPECT_TRUE(m_repo.get_all().empty());
    EXPECT_FALSE(service.get_by_task_id(999).has_value());
}
