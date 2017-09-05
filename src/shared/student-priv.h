#ifndef __STUDENT_PRIV_H__
#define __STUDENT_PRIV_H__

#include <glib.h>

struct Student
{
  gchar *name;
  guint called_on;
};

struct Student *
faircall_student_new (gchar const *const restrict name,
		      GError **error);
void
faircall_student_delete (gpointer data);

#endif /* __STUDENT_PRIV_H__ */
