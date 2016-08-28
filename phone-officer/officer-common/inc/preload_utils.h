/**/


typedef enum {PLC_LIB_NM, PLC_NEXT, PLC_INVLD}sym_lookup_policy_t;

typedef void (*func_ptr_t)();

static func_ptr_t bad_func_ptr = (func_ptr_t) 0;

func_ptr_t find_sym_by_name(const char* sym_name, 
                            sym_lookup_policy_t lookup_policy, 
                            ...);

