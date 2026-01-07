/**
 * @file screen_util.hpp
 * @author DaneJoe001 (danejoe001.github)
 * @brief 屏幕工具类
 * @date 2025-12-28
 */
#pragma once

#include <cstdint>
#include <string>

#include <QPoint>
#include <QRect>

 /**
  * @class ScreenUtil
  * @brief 屏幕工具类
  * @details 提供与屏幕/窗口几何信息相关的静态工具方法。
  *          该类仅包含工具结构与静态函数，不维护状态。
  */
class ScreenUtil
{
public:
    /**
     * @struct PointInfo
     * @brief 点信息
     * @details 使用整数坐标表示点位置。
     */
    struct PointInfo
    {
        /// @brief x坐标
        int32_t x = -1;
        /// @brief y坐标
        int32_t y = -1;
        /**
         * @brief 构造函数
         */
        PointInfo();
        /**
         * @brief 构造函数
         * @param x x坐标
         * @param y y坐标
         */
        PointInfo(int x, int y);
        /**
         * @brief 构造函数
         * @param point 点
         * @details 从 QPoint 提取坐标。
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
         * @details 将 PointInfo 中的坐标转换为 QPoint。
         */
        QPoint to_qpoint() const;
    };
    /**
     * @struct RectInfo
     * @brief 矩形信息
     * @details 通过位置与大小描述矩形。
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
         * @param x x坐标
         * @param y y坐标
         * @param width 宽度
         * @param height 高度
         */
        RectInfo(int x, int y, int width, int height);
        /**
         * @brief 构造函数
         * @param rect 矩形
         * @details 从 QRect 提取位置与大小。
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
         * @details 将 RectInfo 转换为 QRect。
         */
        QRect to_qrect() const;
    };
    /**
     * @enum RealativePosition
     * @brief 相对位置
     * @details 用于描述目标窗口相对父窗口的放置位置。
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
     * @details 返回指定屏幕在桌面坐标系下的矩形信息。
     */
    static RectInfo get_screen_rect(uint32_t screen_index = 0);
    /**
     * @brief 获取目标点
     * @param parent_window_rect 父窗口矩形
     * @param window_rect 窗口矩形
     * @param position 相对位置
     * @return 目标点
     * @details 根据父窗口矩形与目标窗口矩形，计算目标窗口在指定相对位置时的左上角坐标。
     */
    static PointInfo get_destination_point(
        const RectInfo& parent_window_rect,
        const RectInfo& window_rect,
        RealativePosition position);
};