// 
// slice.h
// GraceInfra
//
// Created by DaR1ng on 26-9-18

#pragma once

#include <cstdint>

struct Slice{
    int64_t start_;
    int64_t end_;
    int64_t step_ = 1;
};

struct All {};

inline constexpr All all{};