#include "roster.h"
#include <glib.h>

struct Roster
{
  struct Student **arr;
  gsize size;
  guint nmemb;

  GQueue *must_call;

  GMutex m;
};

struct Roster *faircall_roster_new ()
{
  struct Roster *ret = g_malloc (sizeof (struct Roster));
  ret->arr = NULL;
  ret->size = 0;
  ret->nmemb = 0;
  ret->must_call = g_queue_new();

  g_mutex_init (&ret->m);
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
  new[self->nmemb] = student;
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
  g_queue_push_tail (self->must_call, student);
  g_mutex_unlock (&self->m);
}

void faircall_roster_set_may_call(struct Roster *const restrict self,
				  struct Student const *const restrict student)
{
  if (!self || !student)
    return;

  g_mutex_lock (&self->m);
  g_mutex_unlock (&self->m);
}
void faircall_roster_set_cant_call(struct Roster *const restrict self,
			  	   struct Student const *const restrict student)
{
  if (!self || !student)
    return;

  g_mutex_lock (&self->m);
  g_mutex_unlock (&self->m);
}

