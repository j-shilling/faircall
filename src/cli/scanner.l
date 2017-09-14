%{
#include <glib.h>
#include <glib/gprintf.h>

#include "parser.h"

void set_utf8_string(void);
%}

%option noyywrap

DIGIT	[0-9]
INT	{DIGIT}+
LETTER	[a-zA-Z]
NAME	{LETTER}+
TRUE	"t"|"T"|"true"|"TRUE"
FALSE	"f"|"F"|"false"|"FALSE"

ADD	"a"|"add"
DELETE	"d"|"delete"
INFO	"i"|"info"
LIST	"l"|"list"
OPEN	"o"|"open"
CLOSE	"c"|"close"
UNDO	"z"|"undo"
ABSENT	"a"|"absent"
EVEN	"e"|"even"
CALL	"call"
HELP	"h"|"help"
QUIT	"q"|"quit"

%x STRING
%%

<STRING>\"|\'	BEGIN(INITIAL);
<STRING>.+	set_utf8_string(); return NAME;

{ADD}		return ADD;
{DELETE}	return DELETE;
{INFO}		return INFO;
{LIST}		return LIST;
{OPEN}		return OPEN;
{CLOSE}		return CLOSE;
{UNDO}		return UNDO;
{ABSENT}	return ABSENT;
{EVEN}		return EVEN;
{CALL}		return CALL;
{HELP}		return HELP;
{QUIT}		return QUIT;


{INT}	{
	  guint64 val = g_ascii_strtoull (yytext, NULL, 10);
	  if (val > G_MAXUINT)
	    {
	      g_printf ("We don't work with numbers bigger than %u",
	          G_MAXUINT);
	      val = G_MAXUINT;
            }
	  yylval.integer = (guint) val;
	  return INT;
	}

{TRUE}	{ yylval.boolean = TRUE; return BOOL; }
{FALSE}	{ yylval.boolean = FALSE; return BOOL; }

{NAME}	{
	  GError *error = NULL;
	  yylval.string = g_locale_to_utf8 (yytext,
	                                   -1,
	                                   NULL, NULL,
	                                   &error);
	  if (error)
	    {
	      g_printf ("Error converting input: %s", error->message);
	      g_error_free (error);
	    }

	  return NAME;
	}

%%

void
set_utf8_string(void)
{
  GError *error = NULL;
  yylval.string = g_locale_to_utf8 (yytext,
				   -1,
				   NULL, NULL,
				   &error);
  if (error)
    {
      g_printf ("Error converting input: %s", error->message);
      g_error_free (error);
    }
}
