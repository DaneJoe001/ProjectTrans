/**
 * @file i_serialize_dictionary.hpp
 * @author DaneJoe (danejoe001.github)
 * @brief 字典接口
 * @version 0.2.0
 * @date 2025-12-17
 */
#pragma once

#include <cstdint>
#include <vector>

#include "danejoe/network/codec/serialize_field.hpp"

 /**
  * @namespace DaneJoe
  * @brief DaneJoe命名空间
  */
namespace DaneJoe
{
    /**
     * @brief 字典接口
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
         */
        virtual std::vector<uint8_t> serialize() = 0;
        /**
         * @brief 解析序列化数据
         * @param 序列化数据
         */
        virtual void deserialize(const std::vector<uint8_t>& data_to_parse) = 0;
    };
    /**
     * @brief 获取字典原始数据
     * @param field 字段
     */
    std::vector<uint8_t> to_dictionary(const DaneJoe::SerializeField& field);
}
