#ifndef __ROSTER_H__
#define __ROSTER_H__

#include "student-priv.h"

struct Roster;

struct Roster *faircall_roster_new ();
void faircall_roster_delete (struct Roster *const restrict self);
void faircall_roster_add_student (struct Roster *const restrict self,
				  struct Student const *const restrict student);
void faircall_roster_del_student (struct Roster *const restrict self,
				  struct Student const *const restrict student);

void faircall_roster_set_must_call(struct Roster *const restrict self,
				  struct Student const *const restrict student);
void faircall_roster_set_may_call(struct Roster *const restrict self,
				  struct Student const *const restrict student);
void faircall_roster_set_cant_call(struct Roster *const restrict self,
			  	   struct Student const *const restrict student);
struct Student *faircall_roster_call_student (struct Roster const *const restrict self);
gsize
faircall_roster_length (struct Roster const *const self);
guint
faircall_roster_could_call (struct Roster const *const self);
void
faircall_roster_set_may_call_anyone (struct Roster *const restrict self);
void
faircall_roster_set_cant_call_anyone (struct Roster *const restrict self);
struct Student **
faircall_roster_as_array (struct Roster const *const restrict self);
gboolean
faircall_roster_is_student (struct Roster const *const restrict self,
			    gchar const *const _name);

#endif /* __ROSTER_H__ */
