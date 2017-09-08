#ifndef __FAIRCALL_H__
#define __FAIRCALL_H__

#include "error.h"

#include <glib.h>

struct Class;
extern struct Class *class;

gboolean faircall_open_class (gchar const *const name,
			      gboolean *const restrict new,
			      GError **error);
gboolean faircall_close_class (GError **error);
gboolean faircall_add_student (gchar const *const name, GError **error);
gboolean faircall_del_student (gchar const *const name, GError **error);
gboolean faircall_undo_call (GError **error);
gboolean faircall_mark_absent (GError **error);
gboolean faircall_set_forced_even (gboolean const value, GError **error);

gchar *  faircall_call_student (GError **error);
gchar ** faircall_call_n_students (guint n, GError **error);

gchar ** faircall_info (GError **error);

#endif /* __FAIRCALL_H__ */
