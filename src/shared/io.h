#ifndef __IO_H__
#define __IO_H__

#include <glib.h>

const gchar *
faircall_io_get_filename();
gboolean
faircall_io_save_class(GError **error);
struct Class *
faircall_io_get_class (gchar const *const restrict name,
		       GError **error);
gboolean
faircall_io_is_class (gchar const *const restrict name,
		      GError **error);

#endif /* __IO_H__ */