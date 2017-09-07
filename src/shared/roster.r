#ifndef __ROSTER_R__
#define __ROSTER_R__

struct Roster
{
  struct Student **arr;
  gsize size;
  guint nmemb;

  GQueue *must_call;

  GMutex m;
};

#endif /* __ROSTER_R__ */
