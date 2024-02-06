#pragma once

#include <cstdlib>
#include <memory>

class FiberNoncopyable {
public:
    FiberNoncopyable() = default;

    virtual ~FiberNoncopyable() = default;

    FiberNoncopyable(const FiberNoncopyable &) = delete;

    FiberNoncopyable &operator=(const FiberNoncopyable &) = delete;
};

class FiberNomovable {
public:
    FiberNomovable() = default;

    virtual ~FiberNomovable() = default;

    FiberNomovable(const FiberNomovable &) = delete;

    FiberNomovable &operator=(const FiberNomovable &) = delete;
    
    FiberNomovable(FiberNomovable &&) = delete;

    FiberNomovable &&operator=(FiberNomovable &&) = delete;
};