#include <gtest/gtest.h>

#include <chrono>
#include <filesystem>
#include <iostream>
#include <string>
#include <variant>

#include <danejoe/database/sql_database_manager.hpp>
#include <danejoe/database/sql_query.hpp>
#include <danejoe/database/sqlite_driver.hpp>

#include "repository/block_repository.hpp"

namespace
{
    static std::string cell_to_string(const DaneJoe::SqlCell& cell)
    {
        std::string value;
        std::visit(
            [&value](auto&& arg)
            {
                using T = std::decay_t<decltype(arg)>;
                if constexpr (std::is_same_v<T, std::nullptr_t>)
                {
                    value = "null";
                }
                else if constexpr (std::is_same_v<T, std::string>)
                {
                    value = arg;
                }
                else if constexpr (std::is_same_v<T, std::vector<uint8_t>>)
                {
                    value = "<blob>";
                }
                else if constexpr (std::is_same_v<T, bool>)
                {
                    value = arg ? "true" : "false";
                }
                else
                {
                    value = std::to_string(arg);
                }
            },
            cell.data);
        return cell.column_name + "=" + value;
    }

    static void dump_block_table()
    {
        auto db = DaneJoe::SqlDatabaseManager::get_instance().get_database("client_database");
        if (!db)
        {
            std::cerr << "[dump] database not found\n";
            return;
        }
        auto driver = db->get_driver();
        if (!driver)
        {
            std::cerr << "[dump] driver not found\n";
            return;
        }

        DaneJoe::SqlQuery query(driver);
        query.prepare("SELECT block_id, task_id, offset, block_size, state, start_time, end_time FROM block ORDER BY block_id;");
        auto data = query.execute_query();
        std::cerr << "[dump] block rows=" << data.size() << "\n";
        for (const auto& row : data)
        {
            for (const auto& cell : row)
            {
                std::cerr << cell_to_string(cell) << " ";
            }
            std::cerr << "\n";
        }
    }

    class BlockRepositoryTest : public ::testing::Test
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

            auto db_path = std::filesystem::temp_directory_path() / "client_block_repository_test.db";
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

        BlockRepository m_repo;
    };
}

TEST_F(BlockRepositoryTest, Crud)
{
    EXPECT_TRUE(m_repo.get_all().empty());

    BlockEntity a;
    a.task_id = 1;
    a.offset = 0;
    a.block_size = 128;
    a.state = BlockState::Waiting;
    a.start_time = std::chrono::system_clock::now();
    a.end_time = a.start_time;
    ASSERT_TRUE(m_repo.add(a));

    BlockEntity b;
    b.task_id = 1;
    b.offset = 128;
    b.block_size = 256;
    b.state = BlockState::Waiting;
    b.start_time = std::chrono::system_clock::now();
    b.end_time = b.start_time;
    ASSERT_TRUE(m_repo.add(b));

    auto all = m_repo.get_all();
    ASSERT_EQ(all.size(), 2u);
    EXPECT_GT(all[0].block_id, 0);
    EXPECT_GT(all[1].block_id, 0);

    auto by_task = m_repo.get_by_task_id(1);
    ASSERT_EQ(by_task.size(), 2u);

    auto waiting_count = m_repo.get_count_by_task_id_and_task_state(1, BlockState::Waiting);
    EXPECT_EQ(waiting_count, 2);

    auto found_opt = m_repo.get_by_block_id(all[0].block_id);
    ASSERT_TRUE(found_opt.has_value());
    EXPECT_EQ(found_opt->block_id, all[0].block_id);
    EXPECT_EQ(found_opt->task_id, 1);

    auto updated = *found_opt;
    updated.state = BlockState::Completed;
    updated.end_time = std::chrono::system_clock::now();
    if (!m_repo.update(updated))
    {
        dump_block_table();
        FAIL();
    }

    auto waiting_after = m_repo.get_count_by_task_id_and_task_state(1, BlockState::Waiting);
    auto completed_after = m_repo.get_count_by_task_id_and_task_state(1, BlockState::Completed);
    EXPECT_EQ(waiting_after, 1);
    EXPECT_EQ(completed_after, 1);

    auto completed = m_repo.get_by_task_id_and_task_state(1, BlockState::Completed);
    ASSERT_EQ(completed.size(), 1u);
    EXPECT_EQ(completed[0].block_id, updated.block_id);

    auto found2_opt = m_repo.get_by_block_id(updated.block_id);
    ASSERT_TRUE(found2_opt.has_value());
    EXPECT_EQ(found2_opt->state, BlockState::Completed);

    ASSERT_TRUE(m_repo.remove(updated.block_id));
    auto all2 = m_repo.get_all();
    ASSERT_EQ(all2.size(), 1u);
    EXPECT_FALSE(m_repo.get_by_block_id(updated.block_id).has_value());
}