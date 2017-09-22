
#include <glib.h>
#include <glib/gprintf.h>


typedef struct _Col
{
  glong len;
  struct _Col *next;
} Col;

typedef struct
{
  gchar *str;
  glong len;
  Col *col;

} Cell;

typedef struct _Line
{
  Cell cell;
  struct _Line *next;

} Line;

typedef struct _Lines
{
  Line line;

  struct _Lines *next;
} Lines;

static Cell
faircall_get_cell (gchar const *const str, Col const *const col)
{
  Cell ret;

  ret.str = g_strdup (str);
  ret.len = g_utf8_strlen (ret.str, -1);
  ret.col = col;

  if (ret.len > ret.col->len)
    ret.col->len = ret.len;

  return ret;
}

static void
faircall_print_cell (Cell const cell)
{
  g_printf ("%s ", cell.str);

  for (glong i = cell.len; i < cell.col->len; i ++)
    g_printf (" ");
}

static void
faircall_print_line (Line const line)
{
  for (Line const *l = &line; l; l = l->next)
    faircall_print_cell (l->cell);

  g_printf ("\n");
}

void
faircall_print_table (gchar *const *const strv,
		      guint const headers)
{
  if (!strv || !*strv)
    return;

  for (guint i = 0; i < headers; i ++)
    {
      g_printf ("%s\n", strv[i]);
      gchar *str = strv[i];

      while (*(str + 1))
	str = g_utf8_next_char (str);

      if (*str != '\n')
	g_printf ("\n");
    }

  Lines *lines = NULL;
  Col *cols = g_malloc (sizeof (Col));
  cols->next = NULL;
  cols->len = 0;

  Lines *curlines = lines;
  for (gchar **ll = strv + headers; ll && *ll; ll ++)
    {
      gchar buf[256] = "";
      Line *line = NULL;
      Line *curline = line;
      Col *curcol = cols;

      for (gchar *str = *ll; *str; str = g_utf8_next_char (str))
	{
	  gunichar c = g_utf8_get_char (str);

	  if (g_unichar_isspace (c))
	    {
	      if (!line)
		{
		  line = g_malloc (sizeof (Line));
		  line->next = NULL;
		  line->cell = faircall_get_cell (buf, curcol);
		  curline = line;
		}
	      else
		{
		  curline->next = g_malloc (sizeof (Line));
		  curline->next->next = NULL;
		  curline->next->cell = faircall_get_cell (buf, curcol);
		  curline = curline->next;
		}

	      buf[0] = '\0';
	      if (!curcol->next)
		{
		  curcol->next = g_malloc (sizeof (Col));
		  curcol->next->next = NULL;
		  curcol->next->len = 0;
		}

	      curcol = curcol->next;
	    }
	  else
	    {
	      gchar utf8[7] = {0,0,0,0,0,0,0};
	      g_unichar_to_utf8 (c, utf8);

	      sprintf (buf, "%s%s", buf, utf8);
	    }
	}

      if (!line)
	{
	  line = g_malloc (sizeof (Line));
	  line->next = NULL;
	  line->cell = faircall_get_cell (buf, curcol);
	}
      else
	{
	  curline->next = g_malloc (sizeof (Line));
	  curline->next->next = NULL;
	  curline->next->cell = faircall_get_cell (buf, curcol);
	}


      if (!lines)
	{
	  lines = g_malloc (sizeof (Lines));
	  lines->line = *line;
	  lines->next = NULL;
	  curlines = lines;
	}
      else
	{
	  curlines->next = g_malloc (sizeof (Lines));
	  curlines->next->next = NULL;
	  curlines->next->line = *line;
	  curlines = curlines->next;
	}
    }

  if (lines)
    for (Lines *l = lines; l; l = l->next)
      faircall_print_line (l->line);

  while (lines)
    {
      Lines *tmp = lines->next;
      Line *ln = &lines->line;
      while (ln)
	{
	  g_free (ln->cell.str);
	  ln = ln->next;
	}
      g_free (lines);
      lines = tmp;
    }


  while (cols)
    {
      Col *tmp = cols->next;
      g_free (cols);
      cols = tmp;
    }
}
