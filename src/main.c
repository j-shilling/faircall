#include <stdlib.h>
#include <stdio.h>
#include <stdbool.h>
#include <signal.h>
#include <unistd.h>
#include <string.h>
#include <errno.h>
#include <getopt.h>

#include <libxml/parser.h>

#include "student.h"
#include "list.h"
#include "index.h"

#include "config.h"

/*
 * CONST STRING MACROS
 */
#define ARGSTR		"?1a:f:i:"

#define DOCNAME		"roster"
#define TYPE		"student"
#define NAME		"name"
#define CALLED		"called"
#define INDEX		"index"

#define NEEDS_CLASS_NAME							\
	do {									\
		if (!class)							\
		{								\
			fprintf (stderr, "ERROR: no class name provided\n");	\
			exit (EXIT_FAILURE);					\
		}								\
	}									\
	while (0)

/*
 * external vars
 */
extern int errno;

extern int opterr;
extern int optopt;
extern int optind;
extern char *optarg;

/*
 * STATIC FUNCTIONS
 */
// create a new student in doc
static int add_student (char * name, char * class);
// save new data to doc
static void on_kill (int sig);
// open the doc
static xmlDocPtr open_doc();
// get the node for the current class
static xmlNodePtr get_class (xmlDocPtr doc);
// get the root of the doc
static xmlNodePtr get_root (xmlDocPtr doc);
// get student from xmlNodePtr
static student_t *parse_student (xmlNodePtr node);
// print student info
static void print_stats (student_t *cur, student_t *prev, student_t *last);

/*
 * GLOBAL VARS
 */
list_node_t *list = NULL;
char *class = NULL;
unsigned int last_index = 0;


int
main (int argc, char** argv)
{

	bool run = true;
	bool loop = true;

	/*
	 * Get the class identifier
	 */
	while (getopt (argc, argv, ARGSTR) != -1) { } 
	
	if (optind < argc)
	{

		class = strdup(argv[optind]);
	}

	// reset getopt
	optind = 0;

	/*
	 * Parse cmd line options
	 */
	int c;
	while ( (c = getopt(argc, argv, ARGSTR)) != -1 )
	{
		switch (c)
		{
		case 'f':
			{ /* Read input file and save it to XML file */
				NEEDS_CLASS_NAME;

				FILE *file = fopen(optarg, "r");
				char line[256];

				if (!file)
				{
					fprintf(stderr, "WARNING: Could not open \"%s\" -- %s\n",
							optarg, strerror(errno));
				}
				else
				{
					while (fgets(line, sizeof(line), file))
					{
						add_student (line, class);
					}
					fclose(file);
				}
			}
			run = false;
			continue;

		case 'a':
			{ /* Add a single name */
				NEEDS_CLASS_NAME;

				add_student (optarg, class);
			}
			run = false;
			continue;

		case 'i':
			{ /* print student info */
				bool all = (class == NULL);

				if (all)
					class = optarg;

				xmlDocPtr doc = open_doc();
				xmlNodePtr class_node = get_class(doc);

				char *li = xmlGetProp (class_node, CALLED);
				last_index = atoi (li);

				student_t * student = NULL;
				student_t * prev = NULL;
				student_t * last = NULL;

				xmlNodePtr cur = class_node->xmlChildrenNode;
				while (cur != NULL)
				{
					if (!xmlStrcmp (cur->name, TYPE))
					{
						if (!student)
							prev = last;

						last = parse_student(cur);

						if (!all)
						{
							if (!xmlStrcmp (optarg, last->name))
							{
								student = last;
							}
						}

					}
					cur = cur->next;
				}

				if (!all)
				{
					if (student == NULL)
					{
						fprintf (stderr, "WARNING: no student named %s in %s\n", optarg, class);
					}
					else
					{
						print_stats (student, prev, last);
					}
				}
				else
				{
					cur = class_node->xmlChildrenNode;
					while (cur != NULL)
					{
						if (!xmlStrcmp (cur->name, TYPE))
						{
							prev = student;
							student = parse_student (cur);

							print_stats (student, prev, last);
						}

						cur = cur->next;
					}
				}

			}
			run = false;
			continue;
		case '1':
			loop = false;
			continue;
			

		case '?':
			{
				printf("Usage: %s [mode] <class>\n", PACKAGE_NAME);
				printf("\n");
				printf("%s -a <name> [-a <name> [-a <name> ...]] <class>\t\tadd a name to a class.\n", PACKAGE_NAME);
				printf("%s -f <path> [-f <path> [-f <path> ...]] <class>\t\tadd names from file to class -- assumes one name per line.\n", PACKAGE_NAME);
				printf("%s -i <name> [-i <name> [-i <name> ...]] <class>\t\tshow information about a student in the specified class.\n", PACKAGE_NAME);
				printf("%s -i <class>\t\t\t\t\t\tshow information about all the students in the specified class.\n", PACKAGE_NAME);
				printf("%s -1 <class>\t\t\t\t\t\tcall on a random student from the specified class.\n", PACKAGE_NAME);
				printf("%s <class>\t\t\t\t\t\t\tcall on random students form the specified class until the program is terminated.\n", PACKAGE_NAME);
			}
			run = false;
			return EXIT_SUCCESS;

		default:
			run = false;
			return EXIT_FAILURE;
		}
	}

	/*
	 * If no options given, go to the main prog loop
	 */
	if (run)
	{
		NEEDS_CLASS_NAME;
		/*
		 * register termination handlers
		 */

		signal (SIGTERM, on_kill);
		signal (SIGINT, on_kill);
		signal (SIGQUIT, on_kill);
		signal (SIGHUP, on_kill);

		/*
		 * Get class node
		 */
		xmlDocPtr doc = open_doc();
		xmlNodePtr class_node = get_class (doc);

		if (class_node == NULL)
		{
			fprintf (stderr, "ERROR: no such class \'%s\'\n", class);
			return EXIT_FAILURE;
		}

		/*
		 * Get all students in class
		 */
		list = NULL;
		xmlNodePtr cur = class_node->xmlChildrenNode;
		while (cur != NULL)
		{
			if (!xmlStrcmp (cur->name, TYPE))
			{
				student_t *student = parse_student (cur);
				int i = student->max_index;

				if (!list)
					list = new_list_node (student);
				else
					add (list, student);

				if (i != 0)
					student->max_index = i;
			}

			cur = cur->next;
		}

		last_index = atoi(xmlGetProp (class_node, CALLED));

		xmlFreeDoc (doc);

		if (!list)
		{
			fprintf (stderr, "ERROR: class \'%s\' is empty\n", class);
			return EXIT_FAILURE;
		}

		/*
		 * Main loop
		 */
		student_t *student = NULL;
		do
		{
			if (loop)
			{
				printf ("Press enter: ");
				getchar();
			}
			
			last_index = call_student (list, last_index, &student);

			if (student == NULL)
			{
				fprintf (stderr, "ERROR: index %d did not return a student.\n", last_index);
				return EXIT_FAILURE;
			}

			printf ("\n%s\n\n", student->name);

		} while (loop);

		on_kill(0);
	}

	return EXIT_SUCCESS;
}

static int
add_student (char * name, char *class)
{
	for (int i = 0; i < strlen (name); i++)
	{
		if (name[i] == '\n')
		{
			name[i] = '\0';
			break;
		}
	}

	xmlDocPtr doc = open_doc();

	/*
	 * Create doc and root
	 */
	if (doc == NULL)
	{
		/* create doc */
		doc = xmlNewDoc (BAD_CAST "1.0");
		xmlNodePtr root = xmlNewNode (NULL, BAD_CAST "root");	
		xmlDocSetRootElement (doc, root);
	}

	/*
	 * Find or create class
	 */
	
	xmlNodePtr class_node = get_class(doc);

	if (class_node == NULL)
	{
		class_node = xmlNewChild (get_root (doc), NULL, class, NULL);
		xmlNewProp (class_node, CALLED, "0");
	}

	/*
	 * Check if student already exists in class
	 */
	xmlNodePtr cur = class_node->xmlChildrenNode;
	student_t *last = NULL;

	while (cur != NULL)
	{
		if (!xmlStrcmp (cur->name, TYPE))
		{
			last = parse_student (cur);

			if (!xmlStrcmp (cur->name, name))
			{
				fprintf (stderr, "WARNING: class %s already has a student named %s\n", class, name);
				return EXIT_FAILURE;
			}
		}
		cur = cur->next;
	}

	/*
	 * Add student to class
	 */
	student_t *student = new_student (name);
	xmlNodePtr node = xmlNewChild (class_node, NULL, TYPE, NULL);

	xmlNewProp (node, NAME, name);
	char buf[256];
	sprintf (buf, "%d", last ? (last->max_index + 1) : 0);
	xmlNewProp(node, INDEX, buf);
	xmlNewProp(node, CALLED, "0"); 

	xmlSaveFormatFile (DOCNAME, doc, 0);
	xmlFreeDoc (doc);
}

static _Noreturn void
on_kill (int sig)
{
	/*
	 * Update saved info
	 */
	xmlDocPtr doc = open_doc();
	list_node_t *l = list;

	while (l != NULL)
	{
		student_t *val = l->item;

		xmlNodePtr class_node = get_class (doc);

		char buf[256];
		sprintf (buf, "%i", last_index);
		xmlSetProp (class_node, CALLED, buf);

		xmlNodePtr cur = get_class(doc)->xmlChildrenNode;
		while (cur != NULL)
		{
			if (!xmlStrcmp (cur->name, TYPE))
			{
				if (!xmlStrcmp (xmlGetProp (cur, NAME), val->name))
				{
					char buf[256];
					sprintf(buf, "%d", val->times_called_on);
					xmlSetProp (cur, CALLED, buf);
					sprintf(buf, "%d", val->max_index);
					xmlSetProp (cur, INDEX, buf);
					break;
				}
			}

			cur = cur->next;
		}

		l = l->next;
	}

	xmlSaveFormatFile (DOCNAME, doc, 0);
	xmlFreeDoc (doc);
	exit (EXIT_SUCCESS);
}

static xmlDocPtr
open_doc()
{

	if (access (DOCNAME, F_OK) != 0)
	{
		return NULL;
	}
	else
	{
		LIBXML_TEST_VERSION;

		xmlDocPtr doc = xmlParseFile (DOCNAME);

		if (doc == NULL) {
			fprintf (stderr, "Failed to parse xml document: %s\n", DOCNAME);
			exit (EXIT_FAILURE);
		}
		
		return doc;
	}
}

static xmlNodePtr
get_class (xmlDocPtr doc)
{
	xmlNodePtr cur = get_root(doc)->xmlChildrenNode;

	while (cur != NULL)
	{
		if (!xmlStrcmp (cur->name, class))
			return cur;

		cur = cur->next;
	}

	return NULL;
}

static xmlNodePtr
get_root (xmlDocPtr doc)
{
	xmlNodePtr root = xmlDocGetRootElement(doc);

	if (root == NULL) {
		fprintf (stderr, "Failed to get the root element of xml document: %s\n", DOCNAME);
		xmlFreeDoc(doc);
		exit (EXIT_FAILURE);
	}

	return root;
}

static student_t *
parse_student (xmlNodePtr node)
{
	student_t *student = new_student (xmlGetProp (node, NAME));

	student->times_called_on = atoi (xmlGetProp (node, CALLED));

	student->max_index = atoi (xmlGetProp (node, INDEX));

	return student;
}

static void
print_stats (student_t *cur, student_t *prev, student_t *last)
{
	printf ("name: %s\n", cur->name);
	printf ("times called on: %d\n", cur->times_called_on);

	int pind = prev ? prev->max_index : -1;
	int lind = last ? last->max_index : 0;

	int slots = cur->max_index - pind;

	float odds = ((last_index >= (pind + 1)) && (last_index <= cur->max_index)) ?
		0 : ((float) slots / (float) lind) * 100;


	printf ("odds of getting called next: %3.2f %%\n", odds);

}
