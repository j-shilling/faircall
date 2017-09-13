#include <glib.h>

#include "student.r"
#include "class.r"

int
faircall_strcmp (const void *_x, const void *_y)
{
  GError *error = NULL;
  gchar *x =
    (gchar *)(g_utf8_validate (*((gchar **)_x), -1, NULL) ?
      g_strdup (*((gchar **)_x)) :
      g_locale_to_utf8 (*((gchar **)_x), -1, NULL, NULL, &error));
  if (error)
    {
      g_warning ("Error converting string: %s", error->message);
      error = NULL;
    }

  gchar *y =
    (gchar *)(g_utf8_validate (*((gchar **)_y), -1, NULL) ?
      g_strdup (*((gchar **)_y)) :
      g_locale_to_utf8 (*((gchar **)_y), -1, NULL, NULL, &error));
  if (error)
    {
      g_warning ("Error converting string: %s", error->message);
      error = NULL;
    }

  gchar *str1 = x ? g_utf8_normalize (x, -1, G_NORMALIZE_DEFAULT) : NULL;
  gchar *str2 = y ? g_utf8_normalize (y, -1, G_NORMALIZE_DEFAULT) : NULL;

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
  struct Student *x = *((struct Student **)_x);
  struct Student *y = *((struct Student **)_y);
  return faircall_strcmp (&x->name,
			  &y);
}

int
faircall_student_cmp_str (const void *_x, const void *_y)
{
  struct Student *x = *((struct Student **)_x);
  gchar *y = *((gchar **)_y);
  return faircall_strcmp (&x->name,
			  &y);
}

int
faircall_class_cmp (const void *_x, const void *_y)
{
  struct Class *x = *((struct Class **)_x);
  struct Class *y = *((struct Class **)_y);
  return faircall_strcmp (&x->name,
			  &y->name);
}

int
faircall_class_cmp_str (const void *_x, const void *_y)
{
  struct Class *x = *((struct Class **)_x);
  gchar *y = *((gchar **)_y);
  return faircall_strcmp (&x->name,
			  &y);
}
