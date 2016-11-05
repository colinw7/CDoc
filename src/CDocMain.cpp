#include <CDocMain.h>
#include <CQApp.h>
#include <sys/types.h>
#include <sys/stat.h>
#include <unistd.h>
#include <signal.h>

typedef void (*SignalHandler)(int sig);

// Converts and optional displays the specified help file from the command line.
//
//   cdoc [-if <input_format>]
//        [-of <output_format>]
//        [-out <output_file>]
//        [-dir <directory>]
//        [-show]
//        [-print]
//        [-application <name>]
//        [-class <name>]
//        [-title <title>]
//        [-iconbar]
//        [-noshowheader]
//        [-noshowfooter]
//        [-ragged]
//        [-h]
//        [-debug [<level>]
//        <script_options>
//        <filename>
//
// Use the '-h' option to display a complete list of options with a description
// of their use.
extern int
main(int argc, char **argv)
{
  CQApp app(argc, argv);

  //------

  int rc = 0;

  char **words1    = &argv[1];
  int    no_words1 = argc - 1;

  /* Initialise */

  std::string input_filename;

  show_output  = false;
  print_output = false;
  iconbar      = false;

  /* Get Default Program Options */

  char *prog_opts = getenv("CDOC_PROG_OPTS");

  if (prog_opts != NULL) {
    std::vector<std::string> words;

    CStrUtil::addWords(prog_opts, words);

    int no_words1 = words.size();

    if (no_words1 > 0) {
      char **words1 = new char * [no_words1];

      for (int i = 0; i < no_words1; i++)
        words1[i] = (char *) words[i].c_str();

      int rc = CDocProcessOptions(words1, &no_words1);

      if (rc != -1)
        quit(rc);

      if (no_words1 != 0)
        CDocWriteError("Some Options from 'CDOC_PROG_OPTS' %s",
                       "environment variable have not been Processed");

      delete [] words1;
    }
  }

  /* Extract Options */

  rc = CDocProcessOptions(words1, &no_words1);

  if (rc != -1)
    quit(rc);

  argc = no_words1 + 1;

  /* Set Default Input and/or Output Format if none specified */

  if      (input_format == "" && output_format == "") {
    input_format = CDocInputScript;

    if (show_output || print_output)
      output_format = CDocOutputCDoc;
    else
      output_format = CDocOutputRawCC;
  }
  else if (input_format == "")
    input_format = CDocInputScript;
  else if (output_format == "") {
    if      (strcmp(input_format.c_str(), CDocInputScript) == 0) {
      if (show_output || print_output)
        output_format = CDocOutputCDoc;
      else
        output_format = CDocOutputRawCC;
    }
    else if (strcmp(input_format.c_str(), CDocInputCDoc) == 0)
      output_format = CDocOutputCDoc;
    else if (strcmp(input_format.c_str(), CDocInputRaw) == 0) {
      if (show_output || print_output)
        output_format = CDocOutputCDoc;
      else
        output_format = CDocOutputRawCC;
    }
    else if (strcmp(input_format.c_str(), CDocInputShowcase) == 0)
      output_format = CDocOutputShowcase;
    else if (strcmp(input_format.c_str(), CDocInputPostscript) == 0)
      output_format = CDocOutputPostscript;
    else if (strcmp(input_format.c_str(), CDocInputXWD) == 0)
      output_format = CDocOutputXWD;
    else if (strcmp(input_format.c_str(), CDocInputHTML) == 0)
      output_format = CDocOutputHTML;
    else if (strcmp(input_format.c_str(), CDocInputILeaf) == 0)
      output_format = CDocOutputILeaf;
    else
      output_format = CDocOutputCDoc;
  }

  /* Set Application Name */

  if (application != "") {
    if (strcmp(output_format.c_str(), CDocOutputCDoc) != 0) {
      CDocWriteError("Invalid Output Format for -application - ignored");

      application = "";

      application_name = "cdoc";
    }
    else
      application_name = application;
  }
  else
    application_name = "cdoc";

  /* Set X Class Name */

  if (cclass != "")
    class_name = cclass;
  else
    class_name = "cdoc";

  /*--------------------------*/

  /* Initialise Return Value */

  rc = -1;

  /*--------------------------*/

  /* Initialise CDoc using the Class Name and base Directory */

  CDocInst->initProgramHelps(class_name, base_directory);

  /*--------------------------*/

  /* Extract and Process Script Options */

  CDocScriptProcessOptions((const char **) argv, &argc);

  /*--------------------------*/

  /* Get filename */

  input_filename = "";

  for (int i = 1; i < argc; i++) {
    if (argv[i][0] == '-' && argv[i][1] != '\0')
      continue;

    input_filename = argv[i];
  }

  if (input_filename == "") {
    CDocWriteError("Missing Filename\n");
    quit(rc);
  }

  if (strcmp(input_filename.c_str(), "-") == 0) {
    stdin_filename = CDocInst->getTempFileName();

    FILE *fp = fopen(stdin_filename.c_str(), "w");

    if (fp == NULL) {
      CDocWriteError("Failed to Open Temporary File '%s'", stdin_filename.c_str());
      quit(rc);
    }

    int c;

    while ((c = fgetc(stdin)) != EOF)
      fputc(c, fp);

    fclose(fp);

    input_filename = stdin_filename;
  }

  struct stat file_stat;

  if (stat(input_filename.c_str(), &file_stat) == -1) {
    CDocWriteError("File '%s' Does not Exist", input_filename.c_str());
    quit(rc);
  }

  if (! S_ISREG(file_stat.st_mode)) {
    CDocWriteError("'%s' is not a Regular File", input_filename.c_str());
    quit(rc);
  }

  /*--------------------------*/

  /* Set Title String */

  if (title == "")
    title = input_filename;

  /*--------------------------*/

  /* Initialise Application */

  if (show_output && application != "") {
    /* Ensure application does not already exist */

    CDocWriteError("CDoc already running for application '%s'",
                   application.c_str());
  }

  /*--------------------------*/

  /* Process File */

  if      (show_output) {
    int reference;

    /* Set Icon Bar Visibility */

    CDocInst->setIconBar(iconbar);

    /* Set Header and Footer Visibility */

    CDocInst->setShowHeader(show_header);
    CDocInst->setShowFooter(show_footer);

    /* Set Ragged */

    CDocInst->setRagged(ragged);

    /* Add File as a Formatted Help */

    reference = CDocAddFormattedHelp(class_name,
                                     title,
                                     input_filename,
                                     input_format,
                                     output_format);

    /* Add Signal Handlers to Exit Cleanly */

    if (strcmp(output_format.c_str(), CDocOutputCDoc) == 0) {
      signal(SIGINT , (SignalHandler) CDocTermProg);
      signal(SIGQUIT, (SignalHandler) CDocTermProg);
      signal(SIGTERM, (SignalHandler) CDocTermProg);
    }

    /* If Application defined for CDoc Output then
       Display Help (unpopped up) ... */

    if (application != "") {
      if (strcmp(output_format.c_str(), CDocOutputCDoc) == 0) {
        CDocSetPopupOnDisplay(false);

        CDocDisplayHelpsForWidget(class_name, toplevel, &reference, 1);

        CDocSetPopupOnDisplay(true);
      }
    }

    /* ... otherwise Display Help */

    else
      CDocDisplayHelpsForWidget(class_name, toplevel, &reference, 1);

    /* If Help is being displayed by another program then
       wait for this program to finish before exiting */

    if      ((strcmp(input_format.c_str(), CDocInputScript     ) == 0 ||
              strcmp(input_format.c_str(), CDocInputPostscript ) == 0) &&
             strcmp(output_format.c_str(), CDocOutputPostscript) == 0)
      CDocWaitForPSView();
    else if (strcmp(output_format.c_str(), CDocOutputXWD) == 0)
      CDocWaitForXWD();
  }
  else if (print_output) {
    if      (strcmp(output_format.c_str(), CDocOutputCDoc) == 0) {
      int  reference;
      char print_command[256];

      if (strcmp(input_format.c_str(), CDocInputScript) != 0 &&
          strcmp(input_format.c_str(), CDocInputCDoc  ) != 0 &&
          strcmp(input_format.c_str(), CDocInputRaw   ) != 0) {
        CDocWriteError("Invalid Input Format for Print Option");
        quit(rc);
      }

      /* Add File as a Formatted Help */

      reference = CDocAddFormattedHelp(class_name,
                                       title,
                                       input_filename,
                                       input_format,
                                       output_format);

      /* Add Signal Handlers to Exit Cleanly */

      signal(SIGINT , (SignalHandler) CDocTermProg);
      signal(SIGQUIT, (SignalHandler) CDocTermProg);
      signal(SIGTERM, (SignalHandler) CDocTermProg);

      /* Set Print Command */

      strcpy(print_command, "cat %s");

      if (output_filename != "") {
        strcat(print_command, " > ");
        strcat(print_command, output_filename.c_str());
      }

      CDocInst->setPrintCommand(print_command);

      /* Ensure Help and Print Panels are not displayed while printing */

      CDocSetPopupOnDisplay(false);
      CDocSetPrintOnDisplay(true);

      CDocDisplayHelpsForWidget(class_name, toplevel, &reference, 1);

      CDocSetPrintOnDisplay(false);
      CDocSetPopupOnDisplay(true);
    }
    else
      CDocWriteError("Invalid Output Format for Print Option");
  }
  else {
    /* If Converting from Script then process file outputting
       in the relevant format */

    if      (strcmp(input_format.c_str(), CDocInputScript) == 0) {
      CDocInst->setOutputFormat(output_format);

      CDocInst->setOutputFilename(output_filename);

      CDocScriptProcessFile(input_filename);
    }

    /* Conversion from CDoc is Invalid */

    else if (strcmp(input_format.c_str(), CDocInputCDoc) == 0) {
      CDocWriteError("Invalid Conversion of CDoc File");
    }

    /* Conversion from Raw is Invalid */

    else if (strcmp(input_format.c_str(), CDocInputRaw) == 0) {
      CDocWriteError("Invalid Conversion of Raw File");
    }
  }

  rc = 0;

  if (show_output)
    rc = app.exec();

  quit(0);
}

void
quit(int rc)
{
  CDocInst->termProgramHelps(class_name);

  if (stdin_filename != "")
    remove(stdin_filename.c_str());

  if (application != "" && show_output) {
    if (strcmp(output_format.c_str(), CDocOutputCDoc) == 0) {
    }
  }

  exit(rc);
}

// Process a List of Option Strings.
static int
CDocProcessOptions(char **opts, int *no_opts)
{
  int i = 0;

  while (i < *no_opts) {
    if (opts[i][0] == '-' && opts[i][1] != '\0') {
      /* -if <filename> */

      if       (opts[i][1] == 'i' && opts[i][2] == 'f' && opts[i][3] == '\0' && i < *no_opts - 1) {
        if      (strcmp(opts[i + 1], "script") == 0)
          input_format = CDocInputScript;
        else if (strcmp(opts[i + 1], "cdoc") == 0)
          input_format = CDocInputCDoc;
        else if (strcmp(opts[i + 1], "raw") == 0)
          input_format = CDocInputRaw;
        else if (strcmp(opts[i + 1], "showcase") == 0)
          input_format = CDocInputShowcase;
        else if (strcmp(opts[i + 1], "postscript") == 0 || strcmp(opts[i + 1], "ps") == 0)
          input_format = CDocInputPostscript;
        else if (strcmp(opts[i + 1], "xwd") == 0 || strcmp(opts[i + 1], "x") == 0)
          input_format = CDocInputXWD;
        else if (strcmp(opts[i + 1], "html") == 0)
          input_format = CDocInputHTML;
        else if (strcmp(opts[i + 1], "ileaf") == 0)
          input_format = CDocInputILeaf;
        else
          CDocWriteError("Invalid Input Format %s - Ignored", opts[i + 1]);

        for (int j = i + 2; j < *no_opts; j++)
          opts[j - 2] = opts[j];

        *no_opts -= 2;
      }

      /* -of <filename> */

      else if (opts[i][1] == 'o' && opts[i][2] == 'f' && opts[i][3] == '\0' && i < *no_opts - 1) {
        if      (strcmp(opts[i + 1], "cdoc") == 0)
          output_format = CDocOutputCDoc;
        else if (strcmp(opts[i + 1], "troff") == 0)
          output_format = CDocOutputTroff;
        else if (strcmp(opts[i + 1], "rawcc") == 0)
          output_format = CDocOutputRawCC;
        else if (strcmp(opts[i + 1], "raw") == 0)
          output_format = CDocOutputRaw;
        else if (strcmp(opts[i + 1], "showcase") == 0)
          output_format = CDocOutputShowcase;
        else if (strcmp(opts[i + 1], "postscript") == 0 || strcmp(opts[i + 1], "ps") == 0)
          output_format = CDocOutputPostscript;
        else if (strcmp(opts[i + 1], "xwd") == 0 || strcmp(opts[i + 1], "x") == 0)
          output_format = CDocOutputXWD;
        else if (strcmp(opts[i + 1], "html") == 0)
          output_format = CDocOutputHTML;
        else if (strcmp(opts[i + 1], "ileaf") == 0)
          output_format = CDocOutputILeaf;
        else
          CDocWriteError("Invalid Output Format %s - Ignored",
                         opts[i + 1]);

        for (int j = i + 2; j < *no_opts; j++)
          opts[j - 2] = opts[j];

        *no_opts -= 2;
      }

      /* -out <filename> */

      else if (strcmp(&opts[i][1], "out") == 0 && i < *no_opts - 1) {
        output_filename = std::string(opts[i + 1]);
        show_output     = false;

        for (int j = i + 2; j < *no_opts; j++)
          opts[j - 2] = opts[j];

        *no_opts -= 2;
      }

      /* -dir <directory> */

      else if (strcmp(&opts[i][1], "dir") == 0 && i < *no_opts - 1) {
        base_directory = opts[i + 1];

        for (int j = i + 2; j < *no_opts; j++)
          opts[j - 2] = opts[j];

        *no_opts -= 2;
      }

      /* -show */

      else if (strcmp(&opts[i][1], "show") == 0) {
        output_filename = "";
        show_output     = true;
        print_output    = false;

        for (int j = i + 1; j < *no_opts; j++)
          opts[j - 1] = opts[j];

        *no_opts -= 1;
      }

      /* -print */

      else if (strcmp(&opts[i][1], "print") == 0) {
        show_output  = false;
        print_output = true;

        for (int j = i + 1; j < *no_opts; j++)
          opts[j - 1] = opts[j];

        *no_opts -= 1;
      }

      /* -noshowheader */

      else if (strcmp(&opts[i][1], "noshowheader") == 0) {
        show_header = false;

        for (int j = i + 1; j < *no_opts; j++)
          opts[j - 1] = opts[j];

        *no_opts -= 1;
      }

      /* -noshowfooter */

      else if (strcmp(&opts[i][1], "noshowfooter") == 0) {
        show_footer = false;

        for (int j = i + 1; j < *no_opts; j++)
          opts[j - 1] = opts[j];

        *no_opts -= 1;
      }

      /* -ragged */

      else if (strcmp(&opts[i][1], "ragged") == 0) {
        ragged = true;

        for (int j = i + 1; j < *no_opts; j++)
          opts[j - 1] = opts[j];

        *no_opts -= 1;
      }

      /* -application <name> */

      else if (strcmp(&opts[i][1], "application") == 0 && i < *no_opts - 1) {
        application = std::string(opts[i + 1]);

        for (int j = i + 2; j < *no_opts; j++)
          opts[j - 2] = opts[j];

        *no_opts -= 2;
      }

      /* -class <name> */

      else if (strcmp(&opts[i][1], "class") == 0 && i < *no_opts - 1) {
        cclass = opts[i + 1];

        for (int j = i + 2; j < *no_opts; j++)
          opts[j - 2] = opts[j];

        *no_opts -= 2;
      }

      /* -title <title> */

      else if (strcmp(&opts[i][1], "title") == 0 && i < *no_opts - 1) {
        title = opts[i + 1];

        for (int j = i + 2; j < *no_opts; j++)
          opts[j - 2] = opts[j];

        *no_opts -= 2;
      }

      /* -iconbar */

      else if (strcmp(&opts[i][1], "iconbar") == 0) {
        iconbar = true;

        for (int j = i + 1; j < *no_opts; j++)
          opts[j - 1] = opts[j];

        *no_opts -= 1;
      }

      /* -h */

      else if ((opts[i][1] == 'h' && opts[i][2] == '\0') ||
               strcmp(&opts[i][1], "help") == 0) {
        CDocUsage();

        return(0);
      }

      /* -debug */

      else if (strcmp(&opts[i][1], "debug") == 0) {
        if (i < *no_opts - 1) {
          int j = 0;

          while (opts[i + 1][j] != '\0' && isdigit(opts[i + 1][j]))
            j++;

          if (opts[i + 1][j] == '\0') {
            CDocInst->setDebug(atoi(opts[i + 1]));

            for (j = i + 2; j < *no_opts; j++)
              opts[j - 2] = opts[j];

            *no_opts -= 2;
          }
          else {
            CDocInst->setDebug(255);

            for (j = i + 1; j < *no_opts; j++)
              opts[j - 1] = opts[j];

            *no_opts -= 1;
          }
        }
        else {
          CDocInst->setDebug(255);

          for (int j = i + 1; j < *no_opts; j++)
            opts[j - 1] = opts[j];

          *no_opts -= 1;
        }
      }
      else
        i++;
    }
    else
      i++;
  }

  return(-1);
}

// Wait for the Postscript Viewer Program to Terminate while displaying a
// Postscript File via the Viewer.
static void
CDocWaitForPSView()
{
  /* Add Signal Handlers for a Controlled Exit */

  signal(SIGINT , (SignalHandler) CDocTermProg);
  signal(SIGQUIT, (SignalHandler) CDocTermProg);
  signal(SIGTERM, (SignalHandler) CDocTermProg);

  /* Monitor PSView by checking the process list for the PSView
     process. When this disappears PSView has finished */

  char *p1 = getenv("CDOC_PSVIEW");

  char system_string[256];

  if (p1 != NULL) {
    char *p2;

    while ((p2 = strchr(p1, '/')) != NULL)
      p1 = p2 + 1;

    sprintf(system_string, "ps -a | grep %s", p1);
  }
  else
    sprintf(system_string, "ps -a | grep ghostview");

  bool psview_running = true;

  while (psview_running) {
    FILE *fp = popen(system_string, "r");

    char *p1;

    char line[256];

    if ((p1 = fgets(line, 256, fp)) == NULL)
      psview_running = false;
    else {
      if (p1[0] == '\0' || p1[0] == '\n')
        psview_running = false;
    }

    pclose(fp);

    sleep(1);
  }
}

// Wait for the X Image Viewer Program to Terminate while displaying a X Image
// File via the Viewer.
static void
CDocWaitForXWD()
{
  /* Add Signal Handlers for a Controlled Exit */

  signal(SIGINT , (SignalHandler) CDocTermProg);
  signal(SIGQUIT, (SignalHandler) CDocTermProg);
  signal(SIGTERM, (SignalHandler) CDocTermProg);

  /* Monitor Xwd by checking the process list for the Xwd
     process. When this disappears XWd has finished */

  char *p1 = getenv("CDOC_XWD");

  char system_string[256];

  if (p1 != NULL) {
    char *p2;

    while ((p2 = strchr(p1, '/')) != NULL)
      p1 = p2 + 1;

    sprintf(system_string, "ps -a | grep %s", p1);
  }
  else
    sprintf(system_string, "ps -a | grep xwud");

  bool xwd_running = true;

  while (xwd_running) {
    FILE *fp = popen(system_string, "r");

    char *p1;

    char line[256];

    if ((p1 = fgets(line, 256, fp)) == NULL)
      xwd_running = false;
    else {
      if (p1[0] == '\0' || p1[0] == '\n')
        xwd_running = false;
    }

    pclose(fp);

    sleep(1);
  }
}

// Display Usage Information for the 'cdoc' program.
//
// Uses CDocScriptListOptions() to display current list of Script Options.
static void
CDocUsage()
{
  PERR("CDoc Usage :\n");
  PERR("\n");
  PERR("  cdoc [-if <input_format>]\n");
  PERR("       [-of <output_format>]\n");
  PERR("       [-out <output_file>]\n");
  PERR("       [-dir <base_directory>]\n");
  PERR("       [-show]\n");
  PERR("       [-print]\n");
  PERR("       [-application <name>]\n");
  PERR("       [-class <name>]\n");
  PERR("       [-title <title>]\n");
  PERR("       [-h]\n");
  PERR("       [-debug [<level>]]\n");
  PERR("       <script_options>\n");
  PERR("       <X_options>\n");
  PERR("       <filename>\n");
  PERR("\n");
  PERR("    -if <input_format>    : Specify Format of Input File\n");
  PERR("                          :\n");
  PERR("                          :   script     - Script\n");
  PERR("                          :   cdoc       - CDoc\n");
  PERR("                          :   raw        - Raw Text\n");
  PERR("                          :   showcase   - Showcase\n");
  PERR("                          :   postscript - Postscript\n");
  PERR("                          :   xwd        - X Image\n");
  PERR("                          :   html       - HTML\n");
  PERR("                          :   ileaf      - Interleaf\n");
  PERR("\n");
  PERR("    -of <output_format>   : Specify Format of Output File or\n");
  PERR("                          : Display Format\n");
  PERR("                          :\n");
  PERR("                          :   cdoc       - Output or Display\n");
  PERR("                          :                CDoc\n");
  PERR("                          :   troff      - Output Troff\n");
  PERR("                          :   rawcc      - Output Raw Text with\n");
  PERR("                          :                Control Codes\n");
  PERR("                          :   raw        - Output Raw Text\n");
  PERR("                          :   showcase   - Display Showcase\n");
  PERR("                          :   postscript - Display Postscript\n");
  PERR("                          :   xwd        - X Image\n");
  PERR("                          :   html       - HTML\n");
  PERR("                          :   ileaf      - Interleaf\n");
  PERR("\n");
  PERR("    -out <output_file>    : Define Output File.\n");
  PERR("    -dir <base_directory> : Define Base Directory for embedded\n");
  PERR("                          : File References.\n");
  PERR("    -show                 : Display Converted File.\n");
  PERR("    -print                : Print Converted File.\n");
  PERR("    -application <name>   : Define application name to be used\n");
  PERR("                          : for the CDoc display\n");
  PERR("                          : programs. This causes these programs\n");
  PERR("                          : to wait for a property on the root\n");
  PERR("                          : window to change before they pop up\n");
  PERR("                          : and display the specified part of the\n");
  PERR("                          : document\n");
  PERR("    -class <name>         : Define class name to be used when\n");
  PERR("                          : initialising X. This class name\n");
  PERR("                          : is used when selecting X resources.\n");
  PERR("    -title <title>        : The string to show on the title\n");
  PERR("                          : line of CDoc's help panel.\n");
  PERR("    -iconbar              : Whether CDoc's help panel should\n");
  PERR("                          : include the icon bar.\n");
  PERR("    -noshowheader         : Whether CDoc's help panel should\n");
  PERR("                          : not display the document's header.\n");
  PERR("    -noshowfooter         : Whether CDoc's help panel should\n");
  PERR("                          : not display the document's footer.\n");
  PERR("    -ragged               : Whether CDoc's help panel should\n");
  PERR("                          : display paragraphs with ragged or\n");
  PERR("                          : justfied right margins.\n");
  PERR("    -h                    : Display this help text.\n");
  PERR("\n");
  PERR("    <filename>            : The filename to be processed.\n");
  PERR("\n");
  PERR("  Debug Options :-\n");
  PERR("\n");
  PERR("    -debug [<level>]      : Display CDoc Debugging Output.\n");
  PERR("\n");

  CDocScriptListOptions(stderr);
}

// Terminate the CDoc Program.
static void
CDocTermProg()
{
  if (application != "" && show_output) {
    if (strcmp(output_format.c_str(), CDocOutputCDoc) == 0) {
    }
  }

  exit(0);
}
