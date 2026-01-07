/**
  * @file variable_util.hpp
  * @brief 变量工具
  * @author DaneJoe001
  * @version 0.2.0
  * @date 2026-01-06
  * @details 提供常用的变量/宏工具，用于减少编译器告警并提升代码可读性。
  */
#pragma once


  /// @brief 显式标记未使用变量，避免编译器警告
#define DANEJOE_UNUSED(x) (void)(x);