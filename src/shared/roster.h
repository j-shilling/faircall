#ifndef __ROSTER_H__
#define __ROSTER_H__

#include "student-priv.h"

struct Roster;

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

#endif /* __ROSTER_H__ */
