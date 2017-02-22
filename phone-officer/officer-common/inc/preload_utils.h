/**/


typedef enum {PLC_LIB_NM, PLC_NEXT, PLC_INVLD}sym_lookup_policy_t;

typedef void (*func_ptr_t)();

static func_ptr_t bad_func_ptr = (func_ptr_t) 0;

func_ptr_t find_sym_by_name(const char* sym_name, 
                            sym_lookup_policy_t lookup_policy, 
                            ...);

// find an entry. this returns an emutable value
const char* find_entry_in_env(const char* const entry_key);


// this function will place the preloading
// env variable into the __current__ process'
// environment. 
int insert_pre_loading_to_env(const char* const);

int get_string_length(const char* const str);

