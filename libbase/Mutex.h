/*
 * Copyright (C) 2015 UI project
 *
 * Licensed under the Apache License, Version 2.0 (the "License");
 * you may not use this file except in compliance with the License.
 * You may obtain a copy of the License at
 *
 *      http://www.apache.org/licenses/LICENSE-2.0
 *
 * Unless required by applicable law or agreed to in writing, software
 * distributed under the License is distributed on an "AS IS" BASIS,
 * WITHOUT WARRANTIES OR CONDITIONS OF ANY KIND, either express or implied.
 * See the License for the specific language governing permissions and
 * limitations under the License.
 */

#ifndef UI_LIBBASE_MUTEX_H_
#define UI_LIBBASE_MUTEX_H_

#include "libbase/macros.h"

#include <pthread.h>

namespace merck {

class Mutex {
 public:
  Mutex();
  ~Mutex();

  void ExclusiveLock() EXCLUSIVE_LOCK_FUNCTION();
  bool ExclusiveTryLock() EXCLUSIVE_TRYLOCK_FUNCTION(true);
  void ExclusiveUnlock() UNLOCK_FUNCTION();

 private:
  pthread_mutex_t mutex_;

  DISALLOW_COPY_AND_ASSIGN(Mutex);
};

class MutexLock {
 public:
  explicit MutexLock(Mutex& mu) EXCLUSIVE_LOCK_FUNCTION(mu)
    : mu_(mu) {
    mu_.ExclusiveLock();
  }

  ~MutexLock() UNLOCK_FUNCTION() {
    mu_.ExclusiveUnlock();
  }

 private:
  Mutex& mu_;
  DISALLOW_COPY_AND_ASSIGN(MutexLock);
};

}  // namespace merck

#endif  // UI_LIBBASE_MUTEX_H_
