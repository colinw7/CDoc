#include <CDoc.h>
#include <CStrUtil.h>

#define PERR(a) fprintf(stderr, a)

static std::string cclass;
static std::string title;
static bool        iconbar         = false;
static bool        show_output     = false;
static bool        print_output    = false;
static bool        show_header     = true;
static bool        show_footer     = true;
static bool        ragged          = false;
static std::string application;
static std::string input_format;
static std::string output_format;
static std::string base_directory;
static std::string output_filename;
static std::string stdin_filename;

static QWidget *toplevel = NULL;

static std::string class_name;
static std::string application_name;

int         main(int, char **);
static void quit(int);

static int   CDocProcessOptions(char **, int *);
static void  CDocWaitForPSView (void);
static void  CDocWaitForXWD    (void);
static void  CDocUsage         (void);
static void  CDocTermProg      (void);
