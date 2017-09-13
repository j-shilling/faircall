#include "student.r"
#include "roster-priv.h"
#include "roster.r"
#include "compare.h"

#include <string.h>
#include <glib.h>


struct Roster *
faircall_roster_new ()
{
  struct Roster *ret = g_malloc (sizeof (struct Roster));
  ret->arr = NULL;
  ret->size = 0;
  ret->nmemb = 0;
  ret->must_call = g_queue_new();

  g_mutex_init (&ret->m);
  return ret;
}

void
faircall_roster_delete (struct Roster *const restrict self)
{
  if (!self)
    return;

  g_mutex_lock (&self->m);

  for (int i = 0; i < self->size; i++)
    faircall_student_delete (self->arr[i]);

  g_free (self->arr);
  g_queue_free (self->must_call);

  g_mutex_unlock (&self->m);
  g_mutex_clear (&self->m);
  g_free (self);
}

gsize
faircall_roster_length (struct Roster const *const self)
{
  if (!self)
    return 0;

  g_mutex_lock ((GMutex *)&self->m);
  gsize ret =  self->size;
  g_mutex_unlock ((GMutex *)&self->m);

  return ret;
}

guint
faircall_roster_could_call (struct Roster const *const self)
{
  if (!self)
    return 0;

  g_mutex_lock ((GMutex *)&self->m);
  gsize ret =  self->nmemb;
  g_mutex_unlock ((GMutex *)&self->m);

  return ret;
}
void
faircall_roster_add_student (struct Roster *const restrict self,
				  struct Student const *const restrict student)
{
  if (!self || !student)
    return; 

  g_mutex_lock (&self->m);
  self->size ++;
  struct Student **new = g_malloc0 (sizeof (struct Student *) * self->size);

  memcpy (new, self->arr, self->nmemb * sizeof (struct Student *));
  new[self->nmemb] = (struct Student *)student;
  self->nmemb ++;

  memcpy (new + (self->nmemb), self->arr + (self->nmemb - 1),
	  sizeof (struct Student *) * (self->size - self->nmemb));

  g_free (self->arr);
  self->arr = new;
  g_mutex_unlock (&self->m);
}

void
faircall_roster_del_student (struct Roster *const restrict self,
			     struct Student const *const restrict student)
{
  if (!self || !student)
    return;

  g_mutex_lock (&self->m);
  g_queue_remove (self->must_call, student);
  for (int i = 0; i < self->size; i++)
    {
      if (self->arr[i] == student)
	{
	  memmove (self->arr + i, self->arr + i + 1,
		   (self->size - 1 - i) * sizeof (struct Student *));
	  self->arr[self->size - 1] = NULL;
	  break;
	}
    }
  g_mutex_unlock (&self->m);
}

void
faircall_roster_set_must_call(struct Roster *const restrict self,
				  struct Student const *const restrict student)
{
  if (!self || !student)
    return;

  g_mutex_lock (&self->m);
  g_queue_push_tail (self->must_call, (gpointer)student);
  g_mutex_unlock (&self->m);
}

void
faircall_roster_set_may_call_anyone (struct Roster *const restrict self)
{
  if (!self)
    return;

  g_mutex_lock (&self->m);
  self->nmemb = self->size;
  g_mutex_unlock (&self->m);
}

void
faircall_roster_set_cant_call_anyone (struct Roster *const restrict self)
{
  if (!self)
    return;

  g_mutex_lock (&self->m);
  self->nmemb = 0;
  g_mutex_unlock (&self->m);
}
void
faircall_roster_set_may_call(struct Roster *const restrict self,
				  struct Student const *const restrict student)
{
  if (!self || !student)
    return;

  g_mutex_lock (&self->m);
  for (int i = (self->size - 1); i >= 0; i--)
    {
      if (self->arr[i] == student)
	{
	  if (i < self->nmemb)
	    goto cleanup;

	  self->arr[i] = self->arr[self->nmemb];
	  self->arr[self->nmemb] = (struct Student *)student;
	  self->nmemb ++;

	  goto cleanup;
	}
    }

  struct Student **new = g_malloc ((1 + self->size) * sizeof (struct Student *));
  memcpy (new, self->arr + 1, sizeof (struct Student *) * self->size);
  g_free (self->arr);
  self->arr = new;
  self->arr[0] = (struct Student *)student;
  self->size ++;
  self->nmemb ++;
  
cleanup:
  g_mutex_unlock (&self->m);
}

void faircall_roster_set_cant_call(struct Roster *const restrict self,
			  	   struct Student const *const restrict student)
{
  if (!self || !student)
    return;

  g_mutex_lock (&self->m);
  for (int i = 0; i < self->size; i ++)
    {
      if (self->arr[i] == student)
	{
	  if (i >= self->nmemb)
	    goto cleanup;

	  self->arr[i] = self->arr[self->nmemb - 1];
	  self->arr[self->nmemb - 1] = (struct Student *)student;
	  self->nmemb --;

	  goto cleanup;
	}
    }

  struct Student **new = g_malloc ((1 + self->size) * sizeof (struct Student *));
  memcpy (new, self->arr, sizeof (struct Student *) * self->size);
  g_free (self->arr);
  self->arr[self->size] = (struct Student *)student;
  self->size ++;

cleanup:
  g_mutex_unlock (&self->m);
}

struct Student *
faircall_roster_call_student (struct Roster const *const restrict self)
{
  if (!self)
    {
      g_warning ("faricall_roster_call_student given a null arg");
      return NULL;
    }

  g_mutex_lock ((GMutex *)&self->m);
  struct Student *ret = NULL;

  if (!g_queue_is_empty (self->must_call))
    {
      ret = g_queue_pop_head (self->must_call);
      if (!ret)
	g_warning ("Somehow a null pointer ended up in must_call");
    }
  else if (self->nmemb > 0)
    {
      ret = self->arr[g_random_int_range (0, self->nmemb)];
      if (!ret)
	g_warning ("Somehow self->arr has a null pointer.");
    }
  else
    {
      g_warning ("This roster has no one to call.");
    }

  g_mutex_unlock ((GMutex *)&self->m);
  return ret;
}

struct Student **
faircall_roster_as_array (struct Roster const *const restrict self)
{
  if (!self)
    return NULL;

  g_mutex_lock ((GMutex *)&self->m);
  struct Student **ret = g_malloc ((self->size + 1) * sizeof (struct Student *));
  memcpy (ret, self->arr, (self->size + 1) * sizeof (struct Student *));

  ret[self->size] = 0;

  g_mutex_unlock ((GMutex *)&self->m);
  return ret;
}

gboolean
faircall_roster_is_student (struct Roster const *const restrict self,
			    gchar const *const _name)
{
  if (!self || !_name)
    return FALSE;

  for (int i = 0; i < self->size; i++)
    if (faircall_student_cmp_str (self->arr[i], _name) == 0)
      return TRUE;

  return FALSE;
}
