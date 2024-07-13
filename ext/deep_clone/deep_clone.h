#ifndef DEEP_CLONE_H
#define DEEP_CLONE_H

#include <ruby.h>
#include <ruby/st.h>
#include <ruby/version.h>

struct dump_call_arg
{
  VALUE obj;
  VALUE tracker;
  VALUE src;
};

#define DC_DEBUG 0
#define OBJ_UNFREEZE(x) FL_UNSET((x), FL_FREEZE)

#ifdef SA_EMPTY
struct rb_classext_struct
{
  sa_table m_tbl;
  sa_table iv_tbl;
  sa_table const_tbl;
  sa_table iv_index_tbl;
};

#define TABLE_FOREACH sa_foreach
#define DC_RCLASS_EXT(c) (RCLASS(c)->ptr)
#define DC_RCLASS_IV_INDEX_TBL(c) (&DC_RCLASS_EXT(c)->iv_index_tbl)
#else
#define TABLE_FOREACH st_foreach

#if RUBY_API_VERSION_CODE >= 20100
#if RUBY_API_VERSION_CODE >= 20300
// Ruby 2.3 and above internal structures
typedef struct rb_classext_struct rb_classext_t;

struct RClass
{
  struct RBasic basic;
  VALUE super;
  rb_classext_t *ptr;
  struct method_table_wrapper *m_tbl_wrapper;
};
#endif

typedef struct dc_iv_tbl_classext_struct
{
  struct st_table *iv_index_tbl;
} dc_iv_tbl_classext_t;

#define DC_RCLASS_EXT(c) ((dc_iv_tbl_classext_t*) RCLASS(c)->ptr)
#define DC_RCLASS_IV_INDEX_TBL(c) (DC_RCLASS_EXT(c)->iv_index_tbl)
#else
#define DC_RCLASS_IV_INDEX_TBL(c) (RCLASS(c)->iv_index_tbl)
#endif
#endif

#define DC_ROBJECT_IV_INDEX_TBL(o) ((RBASIC(o)->flags & ROBJECT_EMBED) ?\
                                    (struct st_table *)DC_RCLASS_IV_INDEX_TBL(rb_obj_class(o)) :\
                                    (struct st_table *)ROBJECT(o)->as.heap.iv_index_tbl)

extern VALUE DeepClone;

void Init_deep_clone();
static int clone_variable(st_data_t key, st_data_t index, st_data_t arg);
static int hash_each(st_data_t key, st_data_t value, st_data_t arg);
static VALUE clone_object(VALUE object, VALUE tracker);
VALUE deep_clone(int argc, VALUE *argv);

#endif
