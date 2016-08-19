/**/


enum sym_lookup_policy_t {PLC_LIB_NM, PLC_NEXT, PLC_INVLD};

typedef void (*func_ptr_t)();

static func_ptr_t bad_func_ptr = (func_ptr_t) 0;
