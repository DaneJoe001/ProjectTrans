/**
 * @file screen_util.hpp
 * @author DaneJoe001 (danejoe001.github)
 * @brief 屏幕工具类
 * @date 2026-01-06
 */

#pragma once

#include <cstdint>
#include <string>

#include <QPoint>
#include <QRect>

/**
 * @class ScreenUtil
 * @brief 屏幕工具类
 * @details 提供屏幕/窗口几何信息的辅助计算与类型转换工具。
 */
class ScreenUtil
{
public:
    /**
     * @struct PointInfo
     * @brief 点信息
     * @details 用于描述二维坐标点。
     */
    struct PointInfo
    {
        /// @brief x 坐标
        int32_t x = -1;
        /// @brief y 坐标
        int32_t y = -1;
        /**
         * @brief 构造函数
         */
        PointInfo();
        /**
         * @brief 构造函数
         * @param x x 坐标
         * @param y y 坐标
         */
        PointInfo(int x, int y);
        /**
         * @brief 构造函数
         * @param point 点
         */
        PointInfo(QPoint point);
        /**
         * @brief 转换为字符串
         * @return 字符串
         */
        std::string to_string() const;
        /**
         * @brief 转换为QPoint
         * @return QPoint
         */
        QPoint to_qpoint() const;
    };
    /**
     * @struct RectInfo
     * @brief 矩形信息
     * @details 用于描述矩形的位置与大小。
     */
    struct RectInfo
    {
        /// @brief 位置
        PointInfo pos;
        /// @brief 大小
        PointInfo size;
        /**
         * @brief 构造函数
         */
        RectInfo();
        /**
         * @brief 构造函数
         * @param pos 位置
         * @param size 大小
         */
        RectInfo(PointInfo pos, PointInfo size);
        /**
         * @brief 构造函数
         * @param x x 坐标
         * @param y y 坐标
         * @param width 宽度
         * @param height 高度
         */
        RectInfo(int x, int y, int width, int height);
        /**
         * @brief 构造函数
         * @param rect 矩形
         */
        RectInfo(QRect rect);
        /**
         * @brief 转换为字符串
         * @return 字符串
         */
        std::string to_string() const;
        /**
         * @brief 转换为QRect
         * @return QRect
         */
        QRect to_qrect() const;
    };
    /**
     * @enum RealativePosition
     * @brief 相对位置
     * @details 用于描述子窗口相对父窗口的放置方位。
     */
    enum class RealativePosition
    {
        /// @brief 中心
        Center,
        /// @brief 左上
        TopLeft,
        /// @brief 右上
        TopRight,
        /// @brief 左下
        BottomLeft,
        /// @brief 右下
        BottomRight
    };
public:
    /**
     * @brief 获取屏幕矩形
     * @param screen_index 屏幕索引
     * @return 屏幕矩形
     * @details 返回指定屏幕的矩形信息。
     */
    static RectInfo get_screen_rect(uint32_t screen_index = 0);
    /**
     * @brief 获取目标点
     * @param parent_window_rect 父窗口矩形
     * @param window_rect 窗口矩形
     * @param position 相对位置
     * @return 目标点
     * @details 根据 parent_window_rect 与 window_rect 计算 window_rect 在指定 position 下的目标放置坐标。
     */
    static PointInfo get_destination_point(const RectInfo& parent_window_rect, const RectInfo& window_rect, RealativePosition position);
};