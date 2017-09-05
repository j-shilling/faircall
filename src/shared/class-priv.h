#ifndef __CLASS_PRIV_H__
#define __CLASS_PRIV_H__

#include "error.h"

#include <glib.h>

struct Class
{
  gchar *name;
  gboolean forced_even;

  GMutex m;

  guint can_call_length;
  GList *can_call;
  GList *cant_call;
  GQueue *must_call;

  struct Student *last_called;
  struct Student *last_last_called;
};

#endif /* __CLASS_PRIV_H__ */
