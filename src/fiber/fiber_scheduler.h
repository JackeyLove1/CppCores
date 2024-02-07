#pragma once
#include <memory>
#include <vector>
#include <list>
#include <iostream>
#include "fibers.h"
#include "fiber_thread.h"

/**
 * @brief Fiber Scheduler
 * @details N:M Fiber:Thread Model
 */

class Scheduler {
public:
    typedef std::shared_ptr<Fiber> ptr;


};