#ifndef __CLASS_PRIV_H__
#define __CLASS_PRIV_H__

#include "error.h"
#include "roster-priv.h"

#include <glib.h>

struct Class
{
  gchar *name;
  gboolean forced_even;

  GMutex m;

  struct Roster *r;
  struct Student *last_called;
  struct Student *last_last_called;
};

#endif /* __CLASS_PRIV_H__ */
