#ifndef __CLASS_H__
#define __CLASS_H__

#include <glib.h>

struct Class *
faircall_class_new (gchar const *const restrict name,
		    GError **error);
void
faircall_class_delete (gpointer data);

void
faircall_class_new_student (struct Class *const restrict class,
			    gchar const *const restrict name,
			    GError **error);
void
faircall_class_del_student (struct Class *const restrict class,
			    gchar const *const restrict name,
			    GError **error);
gchar *
faircall_class_call_student (struct Class *const restrict class);
void
faircall_class_uncall_student (struct Class *const restrict class);
void
faircall_class_absent_student (struct Class *const restrict class);
void
faircall_class_set_forced_even (struct Class *const restrict class,
				 gboolean const val);
gchar **
faircall_class_call_n_students (struct Class *const restrict class,
				guint const n,
				GError **error);

#endif /* __CLASS_H__ */
