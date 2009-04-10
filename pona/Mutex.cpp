#include "Mutex.hpp"

namespace pona
{

Mutex::Mutex() { pthread_mutex_init(&mutex_, 0); }
Mutex::~Mutex() { pthread_mutex_destroy(&mutex_); }

void Mutex::acquire() { pthread_mutex_lock(&mutex_); }
void Mutex::release() { pthread_mutex_unlock(&mutex_); }

} // namespace pona
