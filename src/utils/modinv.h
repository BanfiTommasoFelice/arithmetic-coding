#pragma once

#include <assert.h>
#include <stdio.h>

#include "type.h"

u32      u32_mod_inv_odd(u32 b);
u32_pair u32_mod_inv(u32 b);
u64      u64_mod_inv_odd(u64 b);
u64_pair u64_mod_inv(u64 b);
