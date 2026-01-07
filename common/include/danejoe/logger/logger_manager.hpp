/**
 * @file logger_manager.hpp
 * @brief 日志管理类头文件
 * @author DaneJoe001
 * @version 0.2.0
 * @date 2025-12-17
 * @details 定义 LoggerManager，用于集中管理不同类型的 ILogger 与其创建器（ILoggerCreator）。
 *          可通过 logger_type 注册创建器，并按 log_name 获取/创建对应日志器实例。
 */
#pragma once

#include <unordered_map>
#include <string>
#include <memory>
#include <mutex>

#include "danejoe/logger/i_logger.hpp"

 /**
  * @namespace DaneJoe
  * @brief DaneJoe 命名空间
  */
namespace DaneJoe
{
    /**
     * @class LoggerManager
     * @brief 日志管理单例类
     * @details 以单例方式维护日志器实例表与创建器表。
     *          - get_logger() 按名称获取日志器（必要时创建）
     *          - add_creator() 注册某类日志器的创建器
     *          - add_logger() 通过类型与配置创建并注册日志器
     * @note 线程安全性取决于实现的同步策略；该类包含互斥锁用于保护内部映射。
     */
    class LoggerManager
    {
    public:
        /**
         * @brief 获取单例对象
         * @return 单例对象引用
         */
        static LoggerManager& get_instance();
        /**
         * @brief 获取日志对象
         * @details 当日志对象不存在时，会创建一个默认配置的日志对象
         * @param log_name 日志名称
         * @return 日志对象
         */
        std::shared_ptr<DaneJoe::ILogger> get_logger(const std::string& log_name);
        /**
         * @brief 添加日志对象
         * @param logger_type 日志类型
         * @param log_config 日志配置
         * @details 根据 logger_type 对应的创建器创建日志器，并注册到内部表。
         */
        void add_logger(
            const std::string& logger_type,
            const DaneJoe::LoggerConfig& log_config);
        /**
         * @brief 添加日志创建器
         * @param logger_type 日志类型
         * @param creator 日志创建器
         * @details 注册 logger_type 对应的创建器，用于后续创建日志器实例。
         */
        void add_creator(
            const std::string& logger_type,
            std::shared_ptr<DaneJoe::ILoggerCreator> creator);
    private:
        /**
         * @brief 构造函数
         */
        LoggerManager();
        /**
         * @brief 析构函数
         */
        ~LoggerManager() = default;
        /**
         * @brief 删除拷贝构造函数
         */
        LoggerManager(const LoggerManager&) = delete;
        /**
         * @brief 删除移动构造函数
         */
        LoggerManager(const LoggerManager&&) = delete;
        /**
         * @brief 删除拷贝赋值运算符
         */
        LoggerManager& operator=(const LoggerManager&) = delete;
        /**
         * @brief 删除移动赋值运算符
         */
        LoggerManager& operator=(const LoggerManager&&) = delete;
    private:
        /// @brief 互斥锁
        std::mutex m_mutex;
        /// @brief 日志记录器表
        std::unordered_map<std::string, std::shared_ptr<DaneJoe::ILogger>> m_logger_map;
        /// @brief 日志创建器表
        std::unordered_map<std::string, std::shared_ptr<ILoggerCreator>> m_logger_creator_map;
        /// @brief 默认日志配置
        DaneJoe::LoggerConfig m_default_log_config;
    };
}
/**
 * @brief 日志宏
 * @param log_name 日志名称
 * @param module 模块名称
 * @param fmt 格式化字符串
 * @param ... 可变参数
 */
#define DANEJOE_LOG_TRACE(log_name,module,fmt,...)\
do\
{\
auto logger=DaneJoe::LoggerManager::get_instance().get_logger(log_name);\
    if(logger)\
    {\
        logger->trace(\
        std::string(module),\
        std::string(__FILE__), \
        std::string(__FUNCTION__), \
        __LINE__, \
        fmt, \
        ##__VA_ARGS__, "");\
    }\
}while (0)

 /**
  * @brief 日志宏
  * @param log_name 日志名称
  * @param module 模块名称
  * @param fmt 格式化字符串
  * @param ... 可变参数
  */
#define DANEJOE_LOG_DEBUG(log_name,module,fmt,...)\
do\
{\
auto logger=DaneJoe::LoggerManager::get_instance().get_logger(log_name);\
    if(logger)\
    {\
        logger->debug(\
    std::string(module),\
    std::string(__FILE__), \
    std::string(__FUNCTION__), \
    __LINE__, \
    fmt, \
    ##__VA_ARGS__, "");\
    }\
}while (0)

  /**
   * @brief 日志宏
   * @param log_name 日志名称
   * @param module 模块名称
   * @param fmt 格式化字符串
   * @param ... 可变参数
   */
#define DANEJOE_LOG_INFO(log_name,module,fmt,...)\
do\
{\
auto logger=DaneJoe::LoggerManager::get_instance().get_logger(log_name);\
    if(logger)\
    {\
        logger->info(\
    std::string(module),\
    std::string(__FILE__), \
    std::string(__FUNCTION__), \
    __LINE__, \
    fmt, \
    ##__VA_ARGS__, "");\
    }\
}while (0)

   /**
    * @brief 日志宏
    * @param log_name 日志名称
    * @param module 模块名称
    * @param fmt 格式化字符串
    * @param ... 可变参数
    */
#define DANEJOE_LOG_WARN(log_name,module,fmt,...)\
do\
{\
auto logger=DaneJoe::LoggerManager::get_instance().get_logger(log_name);\
    if(logger)\
    {\
        logger->warn(\
    std::string(module),\
    std::string(__FILE__), \
    std::string(__FUNCTION__), \
    __LINE__, \
    fmt, \
    ##__VA_ARGS__, "");\
    }\
}while (0)

    /**
     * @brief 日志宏
     * @param log_name 日志名称
     * @param module 模块名称
     * @param fmt 格式化字符串
     * @param ... 可变参数
     */
#define DANEJOE_LOG_ERROR(log_name,module,fmt,...)\
do\
{\
auto logger=DaneJoe::LoggerManager::get_instance().get_logger(log_name);\
    if(logger)\
    {\
        logger->error(\
    std::string(module),\
    std::string(__FILE__), \
    std::string(__FUNCTION__), \
    __LINE__, \
    fmt, \
    ##__VA_ARGS__, "");\
    }\
}while (0)

     /**
      * @brief 日志宏
      * @param log_name 日志名称
      * @param module 模块名称
      * @param fmt 格式化字符串
      * @param ... 可变参数
      */
#define DANEJOE_LOG_FATAL(log_name,module,fmt,...)\
do\
{\
auto logger=DaneJoe::LoggerManager::get_instance().get_logger(log_name);\
    if(logger)\
    {\
        logger->fatal(\
    std::string(module),\
    std::string(__FILE__), \
    std::string(__FUNCTION__), \
    __LINE__, \
    fmt, \
    ##__VA_ARGS__, "");\
    }\
}while (0)
