#include "py/dynruntime.h"

STATIC int bisect_left_uint16(uint16_t *arr, int len, int x) {
    uint32_t lo = 0; uint32_t mid;
	uint32_t hi = (uint32_t)len;
    
	while (lo < hi) {
        mid = (lo + hi) >> 1; // 右移一位等价于地板除2
        if (arr[mid] < x) {
            lo = mid + 1;
        } else {
            hi = mid;
        }
    }
    
	return (int)lo;
}