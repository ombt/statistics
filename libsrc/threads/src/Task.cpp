//
// Copyright (C) 2010, OMBT LLC and Mike A. Rumore
// All rights reserved.
// Contact: Mike A. Rumore, (mike.a.rumore@gmail.com)
//
// headers
#include "hdr/Task.h"

namespace ombt {

// ctor and dtor
Task::Task() { }
Task::Task(const Task &src) { }
Task::~Task() { }

// assignment
Task &Task::operator=(const Task &rhs) { return(*this); }

// make copies
Task *Task::clone() const { return NULL; }

// actual work functions
int Task::init() { return 0; }
int Task::run() { return 0; }
int Task::finish() { return 0; }

}
