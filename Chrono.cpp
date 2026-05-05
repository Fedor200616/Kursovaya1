#include "Chrono.h"

std::chrono::steady_clock::time_point chrono() {
	return std::chrono::high_resolution_clock::now();
}
