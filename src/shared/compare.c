#include <glib.h>

#include "student.r"
#include "class.r"

int
faircall_strcmp (const void *_x, const void *_y)
{
  GError *error = NULL;
  gchar *x = (gchar *)(g_utf8_validate (_x, -1, NULL) ?
    _x : g_locale_to_utf8 (_x, -1, NULL, NULL, &error));
  if (error)
    {
      g_warning ("Error converting string: %s", error->message);
      error = NULL;
    }

  gchar *y = (gchar *)(g_utf8_validate ((gchar *)_y, -1, NULL) ?
    _y : g_locale_to_utf8 (_y, -1, NULL, NULL, &error));
  if (error)
    {
      g_warning ("Error converting string: %s", error->message);
      error = NULL;
    }

  gchar *str1 = g_utf8_normalize (x, -1, G_NORMALIZE_DEFAULT);
  gchar *str2 = g_utf8_normalize (y, -1, G_NORMALIZE_DEFAULT);

  int ret = g_strcmp0 (str1, str2);

  g_free (x);
  g_free (y);
  g_free (str1);
  g_free (str2);

  return ret;
}

int
faircall_student_cmp (const void *_x, const void *_y)
{
  return faircall_strcmp (((struct Student *)_x)->name,
			  ((struct Student *)_y)->name);
}

int
faircall_student_cmp_str (const void *_x, const void *_y)
{
  return faircall_strcmp (((struct Student *)_x)->name,
			  (gchar *)_y);
}

int
faircall_class_cmp (const void *_x, const void *_y)
{
  return faircall_strcmp (((struct Class *)_x)->name,
			  ((struct Class *)_y)->name);
}

int
faircall_class_cmp_str (const void *_x, const void *_y)
{
  return faircall_strcmp (((struct Class *)_x)->name,
			  (gchar *)_y);
}
