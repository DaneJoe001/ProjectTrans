/**
 * @file i_serialize_dictionary.hpp
 * @author DaneJoe (danejoe001.github)
 * @brief 字典接口
 * @version 0.2.0
 * @date 2025-12-17
 * @details 定义 ISerializeDictionary，用于描述可序列化/可反序列化的“字典”数据结构抽象接口。
 *          该接口通常用于将结构化字段（SerializeField）编码为字节序列，并从字节序列恢复内部状态。
 */
#pragma once

#include <cstdint>
#include <vector>

#include "danejoe/network/codec/serialize_field.hpp"

 /**
  * @namespace DaneJoe
  * @brief DaneJoe 命名空间
  */
namespace DaneJoe
{
    /**
     * @class ISerializeDictionary
     * @brief 字典接口
     * @details 提供序列化与反序列化的抽象接口。
     * @note 线程安全性由实现决定；除非实现内部同步，否则不应在多个线程中并发读写同一实例。
     */
    class ISerializeDictionary
    {
    public:
        /**
         * @brief 虚析构
         */
        virtual ~ISerializeDictionary() = default;
        /**
         * @brief 获取序列化数据
         * @return 序列化数据
         * @details 将当前对象状态编码为字节序列。
         */
        virtual std::vector<uint8_t> serialize() = 0;
        /**
         * @brief 解析序列化数据
         * @param data_to_parse 序列化数据
         * @details 从字节序列解析并更新当前对象状态。
         */
        virtual void deserialize(const std::vector<uint8_t>& data_to_parse) = 0;
    };
    /**
     * @brief 获取字典原始数据
     * @param field 字段
     * @return 字典的序列化字节序列
     */
    std::vector<uint8_t> to_dictionary(const DaneJoe::SerializeField& field);
}
