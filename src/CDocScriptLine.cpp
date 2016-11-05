#include "CDocI.h"

/*-------------------------------------------------------------------------*/

static std::string buffer;
static CDMacro*    current_macro         = NULL;
static int         current_macro_line    = 0;

/*-------------------------------------------------------------------------*/

static CDDotCommandData
dot_command_data[] = {
  {"*" , DOT_TYPE_NONE, false, false, true ,},
  {"..", DOT_TYPE_NONE, false, false, true ,},
  {"aa", DOT_TYPE_NONE, false, false, false,},
  {"am", DOT_TYPE_NONE, true , false, false,},
  {"an", DOT_TYPE_NONE, false, false, false,},
  {"ap", DOT_TYPE_NONE, false, false, false,},
  {"ar", DOT_TYPE_NONE, true , false, false,},
  {"bc", DOT_TYPE_NONE, false, false, false,},
  {"bf", DOT_TYPE_NONE, false, true , true ,},
  {"bl", DOT_TYPE_NONE, true , true , true ,},
  {"bm", DOT_TYPE_NONE, false, false, false,},
  {"br", DOT_TYPE_NONE, true , false, true ,},
  {"bx", DOT_TYPE_NONE, true , false, false,},
  {"cb", DOT_TYPE_NONE, true , false, false,},
  {"cc", DOT_TYPE_NONE, true , false, false,},
  {"cd", DOT_TYPE_NONE, true , false, false,},
  {"ce", DOT_TYPE_ONE , true , true , true ,},
  {"cg", DOT_TYPE_NONE, true , true , false,},
  {"cl", DOT_TYPE_NONE, true , false, false,},
  {"cm", DOT_TYPE_NONE, false, false, true ,},
  {"cp", DOT_TYPE_NONE, true , false, false,},
  {"cs", DOT_TYPE_NONE, false, false, false,},
  {"ct", DOT_TYPE_NONE, false, false, false,},
  {"da", DOT_TYPE_NONE, false, false, false,},
  {"dc", DOT_TYPE_NONE, false, false, false,},
  {"dd", DOT_TYPE_NONE, false, false, true ,},
  {"df", DOT_TYPE_NONE, false, false, true ,},
  {"dh", DOT_TYPE_NONE, false, false, true ,},
  {"dl", DOT_TYPE_NONE, false, false, false,},
  {"dm", DOT_TYPE_NONE, false, false, true ,},
  {"do", DOT_TYPE_NONE, false, false, false,},
  {"dr", DOT_TYPE_NONE, false, false, false,},
  {"ds", DOT_TYPE_NONE, false, false, false,},
  {"du", DOT_TYPE_NONE, false, false, true ,},
  {"dv", DOT_TYPE_NONE, false, false, true ,},
  {"ec", DOT_TYPE_NONE, false, false, false,},
  {"ef", DOT_TYPE_NONE, false, false, false,},
  {"el", DOT_TYPE_NONE, false, false, false,},
  {"em", DOT_TYPE_NONE, false, false, false,},
  {"eq", DOT_TYPE_NONE, false, false, false,},
  {"es", DOT_TYPE_NONE, false, false, false,},
  {"fl", DOT_TYPE_NONE, false, false, false,},
  {"fn", DOT_TYPE_NONE, false, false, false,},
  {"fo", DOT_TYPE_NONE, true , true , true ,},
  {"fv", DOT_TYPE_NONE, false, false, false,},
  {"go", DOT_TYPE_NONE, false, false, true ,},
  {"gr", DOT_TYPE_NONE, false, false, false,},
  {"gs", DOT_TYPE_NONE, false, false, false,},
  {"hr", DOT_TYPE_NONE, true , false, false,},
  {"hw", DOT_TYPE_NONE, false, false, false,},
  {"hy", DOT_TYPE_NONE, false, false, false,},
  {"h0", DOT_TYPE_NONE, false, true , true ,},
  {"h1", DOT_TYPE_NONE, true , true , true ,},
  {"h2", DOT_TYPE_NONE, true , true , true ,},
  {"h3", DOT_TYPE_NONE, true , true , true ,},
  {"h4", DOT_TYPE_NONE, true , true , true ,},
  {"h5", DOT_TYPE_NONE, true , true , true ,},
  {"h6", DOT_TYPE_NONE, true , true , true ,},
  {"ic", DOT_TYPE_NONE, false, false, false,},
  {"ie", DOT_TYPE_NONE, true , false, false,},
  {"if", DOT_TYPE_NONE, false, false, false,},
  {"il", DOT_TYPE_NONE, true , false, false,},
  {"im", DOT_TYPE_NONE, false, false, false,},
  {"in", DOT_TYPE_NONE, true , true , true ,},
  {"io", DOT_TYPE_NONE, false, false, false,},
  {"ir", DOT_TYPE_NONE, true , false, false,},
  {"is", DOT_TYPE_NONE, false, false, false,},
  {"it", DOT_TYPE_NONE, false, false, false,},
  {"ix", DOT_TYPE_NONE, true , false, false,},
  {"ju", DOT_TYPE_NONE, true , false, false,},
  {"kp", DOT_TYPE_NONE, false, false, false,},
  {"lb", DOT_TYPE_NONE, true , false, false,},
  {"li", DOT_TYPE_ONE , false, false, false,},
  {"ll", DOT_TYPE_NONE, true , false, false,},
  {"lo", DOT_TYPE_NONE, true , false, false,},
  {"ls", DOT_TYPE_NONE, true , false, false,},
  {"lt", DOT_TYPE_NONE, true , false, false,},
  {"ly", DOT_TYPE_NONE, false, false, false,},
  {"mc", DOT_TYPE_NONE, true , false, false,},
  {"me", DOT_TYPE_NONE, false, false, false,},
  {"mg", DOT_TYPE_NONE, false, false, false,},
  {"ms", DOT_TYPE_NONE, false, false, false,},
  {"nf", DOT_TYPE_NONE, true , false, false,},
  {"nl", DOT_TYPE_NONE, false, false, false,},
  {"nv", DOT_TYPE_NONE, false, false, false,},
  {"oc", DOT_TYPE_NONE, false, false, false,},
  {"of", DOT_TYPE_NONE, true , false, false,},
  {"oi", DOT_TYPE_NONE, true , false, false,},
  {"or", DOT_TYPE_NONE, false, false, false,},
  {"pa", DOT_TYPE_NONE, true , true , true ,},
  {"pf", DOT_TYPE_NONE, false, true , true ,},
  {"pi", DOT_TYPE_NONE, false, false, false,},
  {"pl", DOT_TYPE_NONE, false, false, false,},
  {"pm", DOT_TYPE_NONE, true , false, false,},
  {"pn", DOT_TYPE_NONE, false, false, false,},
  {"po", DOT_TYPE_NONE, true , false, false,},
  {"pt", DOT_TYPE_NONE, false, false, false,},
  {"pw", DOT_TYPE_NONE, false, false, false,},
  {"px", DOT_TYPE_NONE, false, false, false,},
  {"qq", DOT_TYPE_NONE, false, false, false,},
  {"qu", DOT_TYPE_NONE, false, false, false,},
  {"rc", DOT_TYPE_NONE, false, true , true ,},
  {"rd", DOT_TYPE_NONE, true , false, false,},
  {"re", DOT_TYPE_NONE, false, false, false,},
  {"rf", DOT_TYPE_NONE, true , false, false,},
  {"rh", DOT_TYPE_NONE, true , false, false,},
  {"ri", DOT_TYPE_ONE , true , false, false,},
  {"rn", DOT_TYPE_NONE, false, false, false,},
  {"rv", DOT_TYPE_NONE, false, false, false,},
  {"sa", DOT_TYPE_NONE, false, false, false,},
  {"sb", DOT_TYPE_NONE, false, false, false,},
  {"sc", DOT_TYPE_NONE, true , false, false,},
  {"sd", DOT_TYPE_NONE, false, false, false,},
  {"se", DOT_TYPE_NONE, false, false, true ,},
  {"si", DOT_TYPE_NONE, true , false, false,},
  {"sk", DOT_TYPE_NONE, true , false, false,},
  {"sm", DOT_TYPE_NONE, false, false, false,},
  {"so", DOT_TYPE_NONE, true , false, false,},
  {"sp", DOT_TYPE_NONE, true , false, true ,},
  {"ss", DOT_TYPE_NONE, false, false, false,},
  {"su", DOT_TYPE_ONE , false, false, false,},
  {"sv", DOT_TYPE_NONE, false, false, true ,},
  {"sx", DOT_TYPE_NONE, true , false, false,},
  {"ta", DOT_TYPE_NONE, true , false, false,},
  {"tb", DOT_TYPE_NONE, false, false, false,},
  {"tc", DOT_TYPE_NONE, true , false, false,},
  {"td", DOT_TYPE_NONE, false, false, false,},
  {"te", DOT_TYPE_NONE, false, false, false,},
  {"th", DOT_TYPE_NONE, false, false, false,},
  {"ti", DOT_TYPE_NONE, false, false, true ,},
  {"tm", DOT_TYPE_NONE, false, false, false,},
  {"tp", DOT_TYPE_NONE, false, false, false,},
  {"tr", DOT_TYPE_NONE, false, false, false,},
  {"ts", DOT_TYPE_NONE, false, false, false,},
  {"tu", DOT_TYPE_NONE, false, false, false,},
  {"ty", DOT_TYPE_NONE, false, false, false,},
  {"uc", DOT_TYPE_ONE , false, true , true ,},
  {"ud", DOT_TYPE_NONE, false, false, false,},
  {"un", DOT_TYPE_NONE, true , false, false,},
  {"up", DOT_TYPE_ONE , false, true , true ,},
  {"us", DOT_TYPE_ONE , false, true , true ,},
  {"uw", DOT_TYPE_NONE, false, false, false,},
  {"vr", DOT_TYPE_NONE, true , false, false,},
  {"vt", DOT_TYPE_NONE, false, false, false,},
  {"wf", DOT_TYPE_NONE, false, false, false,},
  {"ws", DOT_TYPE_NONE, false, false, false,},
  {"wz", DOT_TYPE_NONE, true , false, false,},
};

/*-------------------------------------------------------------------------*/

static CDColonCommandData
colon_command_data[] = {
  /* Starter Set */

  {"a"       , false, 2, PARAMETER_VALUES, TEXT_NONE     , true , NULL      },
  {"abstract", false, 0, PARAMETER_NONE  , TEXT_NONE     , true , NULL      },
  {"address" , false, 0, PARAMETER_NONE  , TEXT_NONE     , true , "eaddress"},
  {"aline"   , false, 0, PARAMETER_NONE  , TEXT_LINE     , true , NULL      },
  {"appendix", false, 0, PARAMETER_NONE  , TEXT_NONE     , true , NULL      },
  {"author"  , false, 0, PARAMETER_NONE  , TEXT_LINE     , true , NULL      },
  {"backm"   , false, 0, PARAMETER_NONE  , TEXT_NONE     , true , NULL      },
  {"body"    , false, 0, PARAMETER_NONE  , TEXT_NONE     , true , NULL      },
  {"c"       , false, 1, PARAMETER_VALUES, TEXT_PARAGRAPH, true , "ec"      },
  {"cit"     , true , 0, PARAMETER_NONE  , TEXT_NONE     , true , "ecit"    },
  {"date"    , false, 0, PARAMETER_NONE  , TEXT_LINE     , true , NULL      },
  {"dd"      , false, 0, PARAMETER_NONE  , TEXT_PARAGRAPH, true , NULL      },
  {"ddhd"    , false, 0, PARAMETER_NONE  , TEXT_LINE     , true , NULL      },
  {"dl"      , false, 5, PARAMETER_MIXED , TEXT_NONE     , true , "edl"     },
  {"docnum"  , false, 0, PARAMETER_NONE  , TEXT_LINE     , true , NULL      },
  {"dt"      , false, 0, PARAMETER_NONE  , TEXT_LINE     , true , NULL      },
  {"dthd"    , false, 0, PARAMETER_NONE  , TEXT_LINE     , true , NULL      },
  {"fig"     , false, 6, PARAMETER_VALUES, TEXT_NONE     , true , "efig"    },
  {"figcap"  , false, 0, PARAMETER_NONE  , TEXT_LINE     , true , NULL      },
  {"figdesc" , false, 0, PARAMETER_NONE  , TEXT_PARAGRAPH, true , NULL      },
  {"figlist" , false, 0, PARAMETER_NONE  , TEXT_NONE     , true , NULL      },
  {"figref"  , true , 2, PARAMETER_VALUES, TEXT_NONE     , true , NULL      },
  {"fn"      , false, 1, PARAMETER_VALUES, TEXT_PARAGRAPH, false, "efn"     },
  {"fnref"   , true , 1, PARAMETER_VALUES, TEXT_NONE     , true , NULL      },
  {"frontm"  , false, 0, PARAMETER_NONE  , TEXT_NONE     , true , NULL      },
  {"gd"      , false, 0, PARAMETER_NONE  , TEXT_PARAGRAPH, true , NULL      },
  {"gdoc"    , false, 1, PARAMETER_VALUES, TEXT_NONE     , true , "egdoc"   },
  {"gl"      , false, 2, PARAMETER_MIXED , TEXT_NONE     , true , "egl"     },
  {"gt"      , false, 0, PARAMETER_NONE  , TEXT_LINE     , true , NULL      },
  {"hdref"   , true , 2, PARAMETER_VALUES, TEXT_NONE     , true , NULL      },
  {"hook"    , true , 3, PARAMETER_VALUES, TEXT_NONE     , true , NULL      },
  {"hp0"     , true , 0, PARAMETER_NONE  , TEXT_NONE     , true , "ehp0"    },
  {"hp1"     , true , 0, PARAMETER_NONE  , TEXT_NONE     , true , "ehp1"    },
  {"hp2"     , true , 0, PARAMETER_NONE  , TEXT_NONE     , true , "ehp2"    },
  {"hp3"     , true , 0, PARAMETER_NONE  , TEXT_NONE     , true , "ehp3"    },
  {"h0"      , false, 2, PARAMETER_VALUES, TEXT_LINE     , true , NULL      },
  {"h1"      , false, 2, PARAMETER_VALUES, TEXT_LINE     , true , NULL      },
  {"h2"      , false, 1, PARAMETER_VALUES, TEXT_LINE     , true , NULL      },
  {"h3"      , false, 1, PARAMETER_VALUES, TEXT_LINE     , true , NULL      },
  {"h4"      , false, 1, PARAMETER_VALUES, TEXT_LINE     , true , NULL      },
  {"h5"      , false, 1, PARAMETER_VALUES, TEXT_LINE     , true , NULL      },
  {"h6"      , false, 1, PARAMETER_VALUES, TEXT_LINE     , true , NULL      },
  {"i"       , false, 2, PARAMETER_VALUES, TEXT_NONE     , true , NULL      },
  {"ih1"     , false, 4, PARAMETER_VALUES, TEXT_LINE     , false, NULL      },
  {"ih2"     , false, 4, PARAMETER_VALUES, TEXT_LINE     , false, NULL      },
  {"ih3"     , false, 4, PARAMETER_VALUES, TEXT_LINE     , false, NULL      },
  {"index"   , false, 0, PARAMETER_NONE  , TEXT_NONE     , true , NULL      },
  {"iref"    , true , 4, PARAMETER_VALUES, TEXT_NONE     , false, NULL      },
  {"i1"      , false, 2, PARAMETER_VALUES, TEXT_LINE     , false, NULL      },
  {"i2"      , false, 3, PARAMETER_VALUES, TEXT_LINE     , false, NULL      },
  {"i3"      , false, 3, PARAMETER_VALUES, TEXT_LINE     , false, NULL      },
  {"li"      , false, 1, PARAMETER_VALUES, TEXT_PARAGRAPH, true , NULL      },
  {"liref"   , true , 2, PARAMETER_VALUES, TEXT_NONE     , true , NULL      },
  {"lp"      , false, 0, PARAMETER_NONE  , TEXT_PARAGRAPH, true , NULL      },
  {"lq"      , false, 0, PARAMETER_NONE  , TEXT_NONE     , true , "elq"     },
  {"note"    , false, 0, PARAMETER_NONE  , TEXT_PARAGRAPH, true , NULL      },
  {"ol"      , false, 1, PARAMETER_MIXED , TEXT_NONE     , true , "eol"     },
  {"p"       , false, 0, PARAMETER_NONE  , TEXT_PARAGRAPH, true , NULL      },
  {"pc"      , false, 0, PARAMETER_NONE  , TEXT_PARAGRAPH, true , NULL      },
  {"preface" , false, 0, PARAMETER_NONE  , TEXT_NONE     , true , NULL      },
  {"psc"     , false, 1, PARAMETER_VALUES, TEXT_NONE     , true , "epsc"    },
  {"q"       , true , 0, PARAMETER_NONE  , TEXT_NONE     , true , "eq"      },
  {"rdef"    , false, 9, PARAMETER_VALUES, TEXT_NONE     , true , NULL      },
  {"row"     , false, 2, PARAMETER_VALUES, TEXT_NONE     , true , "erow"    },
  {"sl"      , false, 1, PARAMETER_MIXED , TEXT_NONE     , true , "esl"     },
  {"table"   , false, 7, PARAMETER_VALUES, TEXT_NONE     , true , "etable"  },
  {"tcap"    , false, 0, PARAMETER_NONE  , TEXT_LINE     , true , NULL      },
  {"tdesc"   , false, 0, PARAMETER_NONE  , TEXT_PARAGRAPH, true , NULL      },
  {"tft"     , false, 1, PARAMETER_VALUES, TEXT_NONE     , true , "etft"    },
  {"thd"     , false, 1, PARAMETER_VALUES, TEXT_NONE     , true , "ethd"    },
  {"title"   , false, 1, PARAMETER_VALUES, TEXT_LINE     , true , NULL      },
  {"titlep"  , false, 0, PARAMETER_NONE  , TEXT_NONE     , true , "etitlep" },
  {"tlist"   , false, 0, PARAMETER_NONE  , TEXT_NONE     , true , NULL      },
  {"tnote"   , false, 0, PARAMETER_NONE  , TEXT_PARAGRAPH, true , "etnote"  },
  {"toc"     , false, 1, PARAMETER_NONE  , TEXT_NONE     , true , NULL      },
  {"tref"    , true , 2, PARAMETER_VALUES, TEXT_NONE     , true , NULL      },
  {"ul"      , false, 1, PARAMETER_MIXED , TEXT_NONE     , true , "eul"     },
  {"xmp"     , false, 1, PARAMETER_VALUES, TEXT_NONE     , true , "exmp"    },

  /* Extra Commands */

  {"br"      , true , 0, PARAMETER_NONE  , TEXT_NONE     , true , NULL      },
  {"nl"      , true , 0, PARAMETER_NONE  , TEXT_NONE     , true , NULL      },
 };

/*-------------------------------------------------------------------------*/

static int                 CDocScriptReadLine
                            ();
static int                 CDocScriptIsTextLine
                            (const std::string &line);
static CDColonCommand     *CDocScriptCreateColonCommand
                            ();
static CDDotCommandData   *CDocScriptGetDotCommandData
                            (const std::string &command);
static CDColonCommandData *CDocScriptGetColonCommandData
                            (const std::string &command);

// Get the next Colon Command, Dot Command or Text Line from the input dataset.
//
// If we are executing a macro the macro's next line is returned.
extern CDScriptLine *
CDocScriptGetNextLine()
{
  typedef std::vector<std::string> LineList;
  typedef std::vector<std::string> ParameterList;
  typedef std::vector<std::string> ValueList;

  if      (buffer != "") {
    cdoc_line     = buffer;
    cdoc_line_len = cdoc_line.size();

    buffer = "";
  }
  else if (current_macro != NULL) {
    CDScriptLine *script_line = CDocScriptGetMacroLine(current_macro, current_macro_line);

    current_macro_line++;

    if (CDocScriptGetMacroLine(current_macro, current_macro_line) == NULL)
      current_macro = NULL;

    CDScriptLine *script_line1 = script_line->dup();

    return script_line1;
  }
  else {
    if (! CDocScriptReadLine())
      return NULL;
  }

  /* Check for Comment Line (not a Normal Dot Command) */

  if (cdoc_line.substr(0, 2) == ".*") {
    CDDotCommand *dot_command = new CDDotCommand("*", cdoc_line.substr(2).c_str());

    CDScriptLine *script_line = new CDScriptLine(dot_command);

    return script_line;
  }

  /* Parse Dot Command */

  int i = 0;

  if (cdoc_line_len > 0 && cdoc_line[0] == '.') {
    /* If '.' is followed by an end of line or space then it is not a Dot Command
       and is treated as ordinary text */

    if (cdoc_line_len == 1 || isspace(cdoc_line[1]))
      goto CDocScriptGetNextLine_1;

    CDDotCommand *dot_command = new CDDotCommand;

    /* Check for Leading Quote */

    int i = 1;

    if (i < cdoc_line_len && cdoc_line[i] == '\'') {
      dot_command->setLiteral(true);

      i++;
    }

    /* Get Command String (Characters up to the next space or end of line) */

    std::string command;

    while (i < cdoc_line_len && ! isspace(cdoc_line[i]))
      command += cdoc_line[i++];

    /* Make sure Command String is in Lower Case */

    CStrUtil::toLower(command);

    /* Check if Macro */

    CDMacro *macro = CDocScriptGetMacro(command);

    if (macro != NULL) {
      current_macro      = macro;
      current_macro_line = 1;

      if (CDocScriptGetMacroLine(current_macro, current_macro_line) == NULL)
        current_macro = NULL;

      delete dot_command;

      CDScriptLine *script_line = new CDScriptLine(CDOC_NORMAL_TEXT, "");

      return script_line;
    }

    /* Set Command Data */

    if (command.size() > 2)
      command = command.substr(0, 2);

    CDDotCommandData *command_data = CDocScriptGetDotCommandData(command);

    /* Check if valid Dot Command */

    if (command_data == NULL) {
      /* If not valid Dot Command clean up and process line as a line of ordinary text */
      goto CDocScriptGetNextLine_1;
    }

    /* Skip optional space after Command String */

    i = command.size() + 1;

    if (dot_command->getLiteral())
      i++;

    CStrUtil::skipSpace(cdoc_line, &i);

    /* Check for Type 1 Dot Command where Parameters are known */

    std::string text;

    if (command_data->type == DOT_TYPE_ONE) {
      std::vector<std::string> words;

      CDocStringToWords(cdoc_line.substr(i), words);

      uint no_words = words.size();

      if ((no_words == 1 && CStrUtil::casecmp(words[0], "on" ) == 0) ||
          (no_words == 1 && CStrUtil::casecmp(words[0], "off") == 0) ||
          (no_words == 1 && isdigit(words[0][0])))
        text = cdoc_line.substr(i);
      else {
        buffer = cdoc_line.substr(i);

        text = "";
      }
    }
    else {
      /* Check for Extra Commands split by Semi-Colons and store this text in
         the buffer for next invocation */
      std::string::size_type pos = cdoc_line.find(';', i);

      if (pos != std::string::npos) {
        buffer = cdoc_line.substr(pos + 1);

        cdoc_line     = cdoc_line.substr(0, pos);
        cdoc_line_len = cdoc_line.size();
      }

      text = cdoc_line.substr(i);
    }

    dot_command->setCommand    (command);
    dot_command->setText       (text);
    dot_command->setCommandData(command_data);

    /* Set the Return Script Line Structure */

    CDScriptLine *script_line = new CDScriptLine(dot_command);

    return script_line;
  }

  /* Skip Leading Space */

  i = 0;

  CStrUtil::skipSpace(cdoc_line, &i);

  /* Parse Colon Command */

  if (i < cdoc_line_len && cdoc_line[i] == ':') {
    /* If ':' is followed by an end of line or space then it is not a Colon Command
       and is treated as ordinary text */

    if (i + 1 >= cdoc_line_len || isspace(cdoc_line[i + 1]))
      goto CDocScriptGetNextLine_1;

    ++i;

    CDColonCommand *colon_command = CDocScriptCreateColonCommand();

    /* Get Command String (Characters up to the next space a '.' character or end of line) */

    std::string command_str;

    while (i < cdoc_line_len && cdoc_line[i] != '.' && ! isspace(cdoc_line[i]))
      command_str += cdoc_line[i++];

    /* Make sure Command String is in Lower Case */

    CStrUtil::toLower(command_str);

    colon_command->setCommand(command_str);

    /* Set Command Data */

    CDColonCommandData *command_data = CDocScriptGetColonCommandData(colon_command->getCommand());

    colon_command->setCommandData(command_data);

    /* Check if inline Colon Command */

    if (command_data != NULL && command_data->in_line) {
      /* If not valid Colon Command clean up and process line as a line of ordinary text */

      delete colon_command;

      goto CDocScriptGetNextLine_1;
    }

    /* Skip Space after Command String */

    CStrUtil::skipSpace(cdoc_line, &i);

    /* Set Colon Commands Number of Options */

    int no_options = 1;

    if (command_data != NULL)
      no_options = command_data->no_parameters;

    /* If Command has no Options then rest of Line is text */

    if (no_options == 0) {
      if (i < cdoc_line_len && cdoc_line[i] == '.')
        ++i;
    }
    else {
      bool terminator_found = false;

      /* If Command has a single line of text and only <name>=<value> type arguments
         then if no arguments of this type are present assume line is text */

      if (command_data != NULL &&
          command_data->text_type == TEXT_LINE &&
          command_data->parameter_types == PARAMETER_VALUES) {
        int j = i;

        /* Extract Parameter String (Characters up to a '=' character, a space or
           the end of the line) */

        while (j < cdoc_line_len && cdoc_line[j] != '=' && ! isspace(cdoc_line[j]))
          ++j;

        if (j >= cdoc_line_len || cdoc_line[j] != '=')
          terminator_found = true;
      }

      /* If Command has one option then only current line can contain options. If more
         than one option then read lines until terminating '.' character is found. */

      std::string args;

      while (! terminator_found) {
        if      (i < cdoc_line_len && cdoc_line[i] == '.')
          terminator_found = true;
        else if (i >= cdoc_line_len) {
          if (no_options == 1)
            terminator_found = true;
          else {
            if (! CDocScriptReadLine())
              break;

            /* Check if Line is a Text Line */

            if (! CDocScriptIsTextLine(cdoc_line)) {
              /* Save Line in Buffer for Next Read */

              buffer = cdoc_line;

              i = 0;

              cdoc_line     = "";
              cdoc_line_len = 0;

              /* Terminate Read */

              terminator_found = true;
            }
            else {
              /* Add a Space between concatenated lines and Restart with New Line */

              args += ' ';

              i = 0;

              /* Skip leading Space */

              CStrUtil::skipSpace(cdoc_line, &i);
            }
          }
        }

        /* Skip quoted text */

        else if (i < cdoc_line_len && cdoc_line[i] == '\'') {
          args += cdoc_line[i++];

          while (i < cdoc_line_len && cdoc_line[i] != '\'')
            args += cdoc_line[i++];

          if (i < cdoc_line_len && cdoc_line[i] == '\'')
            args += cdoc_line[i++];
        }

        /* Skip potential symbol */

        else if (i < cdoc_line_len && cdoc_line[i] == '&') {
          args += cdoc_line[i++];

          while (i < cdoc_line_len && isalnum(cdoc_line[i]))
            args += cdoc_line[i++];

          if (i < cdoc_line_len && cdoc_line[i] == '.')
            args += cdoc_line[i++];
        }

        else if (i < cdoc_line_len)
          args += cdoc_line[i++];
      }

      if (i < cdoc_line_len && cdoc_line[i] == '.')
        i++;

      /* Create Lists for Parameters and Values */

      ParameterList parameter_list;
      ValueList     value_list;

      int args_len = args.size();

      int j = 0;

      while (j < args_len) {
        /* Extract Parameter String (Characters up to a '=' character, a space or
           the end of the line) */

        std::string parameter;

        while (j < args_len && args[j] != '=' && ! isspace(args[j]))
          parameter += args[j++];

        /* Skip Space After Parameter */

        CStrUtil::skipSpace(args, &j);

        /* Make sure Parameter is in Lower Case */

        CStrUtil::toLower(parameter);

        /* If Parameter has a Value (followed by '=' character) then get the Value
           String otherwise set the Value String to an Empty String */

        std::string value;

        if (j < args_len && args[j] == '=') {
          j++;

          /* Skip Space After '=' character */

          CStrUtil::skipSpace(args, &j);

          /* Get Value String (Characters up to the next space or end of line
             including all characters in quotes */

          /* Include all characters in quotes */

          if (j < args_len && args[j] == '\'') {
            j++;

            while (j < args_len && args[j] != '\'')
              value += args[j++];

            if (j < args_len && args[j] == '\'')
              j++;
          }
          else {
            while (j < args_len && ! isspace(args[j]))
              value += args[j++];
          }
        }

        /* Add Parameter and Value to Lists */

        parameter_list.push_back(parameter);
        value_list    .push_back(value    );

        /* Skip space after Value String */

        CStrUtil::skipSpace(args, &j);
      }

      /* Convert Parameter and Value Lists to Arrays of Parameters and Values */

      int no_parameters = parameter_list.size();

      for (int j = 0; j < no_parameters; ++j)
        colon_command->addParameter(parameter_list[j], value_list[j]);

      /* Delete the Parameter and Value Lists */

      parameter_list.clear();
      value_list    .clear();
    }

    /* Set Colon Command Text */

    int text_type = TEXT_LINE;

    if (command_data != NULL)
      text_type = command_data->text_type;

    if      (text_type == TEXT_PARAGRAPH) {
      LineList line_list;

      std::string line1 = cdoc_line.substr(i);

      line_list.push_back(line1);

      int paragraph_length = line1.size();

      while (CDocScriptReadLine()) {
        /* Check if Line is a Text Line, an Empty Line
           or a Line staring with a Space */

        if (! CDocScriptIsTextLine(cdoc_line) || cdoc_line == "" || isspace(cdoc_line[0])) {
           /* Terminate Paragraph */

           buffer = cdoc_line;

           cdoc_line     = "";
           cdoc_line_len = 0;

           break;
        }

        /* Add line to Paragraph */

        line1 = cdoc_line;

        line_list.push_back(line1);

        paragraph_length += line1.size() + 1;
      }

      std::string text;

      int no_lines = line_list.size();

      for (int i = 1; i <= no_lines; i++) {
        line1 = line_list[i - 1];

        if (i > 1) {
          colon_command->addBreak(text.size());

          if (text != "" && text[text.size() - 1] != '\n')
            text += " ";
        }

        text += line1;
      }

      colon_command->setText(text);
    }
    else if (text_type == TEXT_LINE) {
      /* If Colon Command requiring text line has no text following it then get the next line */

      if (i >= cdoc_line_len ) {
        if (CDocScriptReadLine()) {
          /* Check if Line is a Text Line, an Empty Line or a Line staring with a Space */

          if (! CDocScriptIsTextLine(cdoc_line) || cdoc_line == "" || isspace(cdoc_line[0])) {
             /* Terminate Paragraph */

             buffer = cdoc_line;

             colon_command->setText("");
          }
          else {
            /* Check for and extract embedded Colon Command */
            std::string::size_type pos = cdoc_line.find(':');

            if (pos != std::string::npos) {
              if (! CDocScriptIsTextLine(cdoc_line.substr(pos))) {
                buffer = cdoc_line.substr(pos);

                cdoc_line     = cdoc_line.substr(0, pos - 1);
                cdoc_line_len = cdoc_line.size();
              }
            }

            /* Use the Line of Text */

            colon_command->setText(cdoc_line);
          }
        }
        else
          colon_command->setText("");
      }
      else {
        /* Check for and extract embedded Colon Command */
        std::string::size_type pos = cdoc_line.find(':', i);

        if (pos != std::string::npos) {
          if (! CDocScriptIsTextLine(cdoc_line.substr(pos))) {
            buffer = cdoc_line.substr(pos);

            cdoc_line     = cdoc_line.substr(0, pos - 1);
            cdoc_line_len = cdoc_line.size();
          }
        }

        /* Use the Line of Text */

        colon_command->setText(cdoc_line.substr(i));
      }
    }
    else {
      /* If Command has no Text then rest of line might contain more commands */

      if (buffer == "" && i < cdoc_line_len)
        buffer = cdoc_line.substr(i);

      colon_command->setText("");
    }

    /* Replace Embedded Symbols in Parameter Values */

    uint no_parameters = colon_command->getNumParameters();

    for (uint i = 0; i < no_parameters; ++i) {
      std::string name, value;

      colon_command->getParameter(i, name, value);

      if (value != "") {
        value = CDocScriptReplaceSymbolsInString(value);

        colon_command->setParameter(i, name, value);
      }
    }

    /* Return Script Line */

    CDScriptLine *script_line = new CDScriptLine(colon_command);

    return script_line;
  }

 CDocScriptGetNextLine_1:
  /* Parse raw text (not belonging to a Script Command) */

  /* Add Underscore Escape Code if Required */

  std::string line1;

  if (cdoc_no_uscore != 0) {
    if (cdoc_no_uscore > 0)
      --cdoc_no_uscore;

    if (cdoc_line != "")
      std::string line1 = CDocStartUnderline() + cdoc_line + CDocEndUnderline();
    else
      line1 = cdoc_line;
  }
  else
    line1 = cdoc_line;

  /* Capitalize Line if Required */

  if (cdoc_no_cap != 0) {
    int no;

    if (cdoc_no_cap > 0)
      cdoc_no_cap--;

    int i = 0;

    int line1_len = line1.size();

    while (i < line1_len) {
      if      ((no = CDocIsEscapeCode(line1.substr(i).c_str())) != 0)
        i += no;
      else {
        if (islower(line1[i]))
          line1[i] = toupper(line1[i]);

        ++i;
      }
    }
  }

  /* Add line to Paragraph */

  LineList line_list;

  line_list.push_back(line1);

  int paragraph_length = line1.size();

  /* Label the Line as Centred if Required */

  CDocLineType type = CDOC_NORMAL_TEXT;

  if      (cdoc_no_centred != 0) {
    if (cdoc_no_centred > 0)
      cdoc_no_centred--;

    type = CDOC_CENTRED_TEXT;
  }

  /* If the line starts with a space or is empty then
     Label the Line as Raw Text */

  else if (line1 == "" || isspace(line1[0])) {
    type = CDOC_RAW_TEXT;
  }

  /* If Formatting then concatenate following Text lines
     to current line */

  else if (cdoc_formatting) {
    while (CDocScriptReadLine()) {
      /* Check if Line is a Text Line, Empty Line or a
         Line starting with a Space */

      if (! CDocScriptIsTextLine(cdoc_line) || cdoc_line == "" || isspace(cdoc_line[0])) {
        /* Terminate Paragraph */

        buffer = cdoc_line;

        cdoc_line     = "";
        cdoc_line_len = 0;

        break;
      }

      /* Add Underscore Escape Codes line if required */

      std::string line1;

      if (cdoc_no_uscore != 0) {
        if (cdoc_no_uscore  > 0)
          --cdoc_no_uscore;

        std::string line1 = CDocStartUnderline() + cdoc_line + CDocEndUnderline();
      }
      else
        line1 = cdoc_line;

      /* Capitalize line if required */

      if (cdoc_no_cap != 0) {
        int no;

        if (cdoc_no_cap > 0)
          cdoc_no_cap--;

        int i = 0;

        int line1_len = line1.size();

        while (i < line1_len) {
          if      ((no = CDocIsEscapeCode(line1.substr(i).c_str())) != 0)
            i += no;
          else {
            if (islower(line1[i]))
              line1[i] = toupper(line1[i]);

            ++i;
          }
        }
      }

      /* Add line to Paragraph */

      line_list.push_back(line1);

      paragraph_length += line1.size() + 1;
    }

    type = CDOC_NORMAL_TEXT;
  }

  /* If not Formatting then return single Text Line */

  else
    type = CDOC_NORMAL_TEXT;

  /* Concatenate all lines to produce the final line */

  std::string data;

  int len = 0;

  int no_lines = line_list.size();

  for (int i = 1; i <= no_lines; i++) {
    if (len > 0 && data[len - 1] != '\n')
      data += ' ';

    data += line_list[i - 1];

    len = data.size();
  }

  line_list.clear();

  CDScriptLine *script_line = new CDScriptLine(type, data);

  return script_line;
}

// Read Next Line from Input File. Lines ending in the current concatenation
// character are combined.
static int
CDocScriptReadLine()
{
  int length = 0;

  cdoc_line     = "";
  cdoc_line_len = 0;

  bool continued = false;

  do {
    continued = false;

    std::string line1;

    bool rc = CDocReadLineFromFile(cdoc_input_fp, line1);

    if (! rc && length > 0)
      CDocScriptError("Uncompleted Continuation Line");

    while (! rc && ! cdoc_input_fp_list.empty()) {
      fclose(cdoc_input_fp);

      cdoc_input_fp = cdoc_input_fp_list.back();

      cdoc_input_fp_list.pop_back();

      rc = CDocReadLineFromFile(cdoc_input_fp, line1);
    }

    if (! rc)
      return false;

    /* Translate Any Characters on Line */

    CDocScriptTranslateLine(line1);

    /* Increment the Current Input Line Number */

    cdoc_input_line_no++;

    /* Check New Line Length */

    int l = line1.size();

    /* Check for Continuation Character */

    if (cdoc_continuation_char != (int) '\0') {
      if (l > 0 && (int) line1[l - 1] == cdoc_continuation_char) {
        line1 = line1.substr(0, l - 1);

        continued = true;
      }
    }

    /* If Current Line is a Comment and adding to comment
       then remove '.*' characters ... */

    if (line1.substr(0, 2) == ".*" && cdoc_line.substr(0, 2) == ".*") {
      cdoc_line += line1.substr(2);

      cdoc_line_len = cdoc_line.size();

      length += l - 2;
    }

    /* ... otherwise add Current Line to Master Line */

    else {
      cdoc_line += line1;

      cdoc_line_len = cdoc_line.size();

      length += l;
    }
  }
  while (continued);

  /* Remove Trailing Space */

  length = cdoc_line.size();

  while (length >= 0 && isspace(cdoc_line[length]))
    --length;

  if (length < int(cdoc_line.size())) {
    cdoc_line = cdoc_line.substr(0, length);
  }

  cdoc_line_len = cdoc_line.size();

  return true;
}

// Checks whether the specified str constitutes a line of text
// i.e. is not a Colon or Dot Command.
static int
CDocScriptIsTextLine(const std::string &line)
{
  /* If Blank line then not a Text Line */

  int line_len = line.size();

  if (line_len == 0)
    return false;

  /* Check for Comment */

  if (line.substr(0, 2) == ".*")
    return false;

  /* If valid Dot Command then not a Text Line */

  if (line_len > 0 && line[0] == '.') {
    if (line_len == 1 || isspace(line[1]))
      return true;

    /* Skip any Leading Quote */

    int i = 1;

    if (i < line_len && line[i] == '\'')
      ++i;

    /* Get Command String (Characters up to the next space or end of line) */

    std::string command;

    while (i < line_len && ! isspace(line[i]))
      command += line[i++];

    /* Make sure Command String is in Lower Case */

    CStrUtil::toLower(command);

    /* Check if Macro */

    CDMacro *macro = CDocScriptGetMacro(command);

    if (macro != NULL)
      return false;

    /* Check if Dot Command */

    if (command.size() > 2)
      command = command.substr(0, 2);

    CDDotCommandData *dot_command_data1 = CDocScriptGetDotCommandData(command);

    if (dot_command_data1 != NULL)
      return false;

    return true;
  }

  /* Skip Leading Space */

  int i = 0;

  CStrUtil::skipSpace(line, &i);

  /* If valid Colon Command (not inline) then not a Text Line */

  if (i < line_len && line[i] == ':') {
    ++i;

    if (i >= line_len || isspace(line[i]) || ! isalpha(line[i]))
      return true;

    /* Get Command String (Characters up to the next space a '.' character or end of line) */

    std::string command;

    while (i < line_len && line[i] != '.' && ! isspace(line[i]))
      command += line[i++];

    /* Make sure Command String is in Lower Case */

    CStrUtil::toLower(command);

    /* Set Command Data */

    CDColonCommandData *colon_command_data1 = CDocScriptGetColonCommandData(command);

    /* Check if inline Colon Command */

    if (colon_command_data1 != NULL && colon_command_data1->in_line)
      return true;

    return false;
  }

  return true;
}

// Create a Colon Command Structure.
static CDColonCommand *
CDocScriptCreateColonCommand()
{
  CDColonCommand *colon_command = new CDColonCommand;

  return colon_command;
}

// Gets the Dot Command Data Structure for the given Dot Command String.
static CDDotCommandData *
CDocScriptGetDotCommandData(const std::string &command)
{
  CDDotCommandData *command_data = NULL;

  int no_commands = sizeof(dot_command_data)/sizeof(CDDotCommandData);

  for (int i = 0; i < no_commands; i++) {
    if (strcmp(dot_command_data[i].command, command.c_str()) == 0) {
      command_data = &dot_command_data[i];
      break;
    }
  }

  return command_data;
}

// Gets the Colon Command Data Structure for the given Colon Command String.
static CDColonCommandData *
CDocScriptGetColonCommandData(const std::string &command)
{
  CDColonCommandData *command_data = NULL;

  int no_commands = sizeof(colon_command_data)/sizeof(CDColonCommandData);

  for (int i = 0; i < no_commands; i++) {
    if (strcmp(colon_command_data[i].command, command.c_str()) == 0 ||
        (colon_command_data[i].end_command != NULL &&
         strcmp(colon_command_data[i].end_command, command.c_str()) == 0)) {
      command_data = &colon_command_data[i];

      break;
    }
  }

  return command_data;
}

//---------------

CDScriptLine::
CDScriptLine(CDocLineType type, const std::string &data)
{
  assert(type != CDOC_DOT_COMMAND && type != CDOC_COLON_COMMAND);

  type_ = type;
  data_ = (void *) CStrUtil::strdup(data);
}

CDScriptLine::
CDScriptLine(CDDotCommand *dot_command)
{
  type_ = CDOC_DOT_COMMAND;
  data_ = (void *) dot_command;
}

CDScriptLine::
CDScriptLine(CDColonCommand *colon_command)
{
  type_ = CDOC_COLON_COMMAND;
  data_ = (void *) colon_command;
}

CDScriptLine::
~CDScriptLine()
{
  reset();
}

CDScriptLine::
CDScriptLine(const CDScriptLine &script_line)
{
  type_ = script_line.type_;

  if      (type_ == CDOC_DOT_COMMAND) {
    CDDotCommand *dot_command = (CDDotCommand *) script_line.data_;

    data_ = (void *) dot_command->dup();
  }
  else if (type_ == CDOC_COLON_COMMAND) {
    CDColonCommand *colon_command = (CDColonCommand *) script_line.data_;

    data_ = (void *) colon_command->dup();
  }
  else
    data_ = CStrUtil::strdup((char *) script_line.data_);
}

CDScriptLine *
CDScriptLine::
dup()
{
  return new CDScriptLine(*this);
}

void
CDScriptLine::
setData(CDocLineType type, const std::string &data)
{
  if (type == type_) {
    if      (type == CDOC_DOT_COMMAND) {
      CDDotCommand *dot_command = (CDDotCommand *) data_;

      dot_command->setText(data);
    }
    else if (type_ == CDOC_COLON_COMMAND) {
      CDColonCommand *colon_command = (CDColonCommand *) data_;

      colon_command->setText(data);
    }
    else {
      delete [] (char *) data_;

      data_ = CStrUtil::strdup(data.c_str());
    }
  }
  else {
    assert(type != CDOC_DOT_COMMAND && type != CDOC_COLON_COMMAND);

    reset();

    type_ = type;
    data_ = CStrUtil::strdup(data.c_str());
  }
}

void
CDScriptLine::
setData(CDDotCommand *dot_command)
{
  reset();

  type_ = CDOC_DOT_COMMAND;
  data_ = (char *) dot_command;
}

void
CDScriptLine::
setData(CDColonCommand *colon_command)
{
  reset();

  type_ = CDOC_COLON_COMMAND;
  data_ = (char *) colon_command;
}

std::string
CDScriptLine::
getData()
{
  return std::string((char *) data_);
}

CDDotCommand *
CDScriptLine::
getDotCommand()
{
  return (CDDotCommand *) data_;
}

CDColonCommand *
CDScriptLine::
getColonCommand()
{
  return (CDColonCommand *) data_;
}

void
CDScriptLine::
reset()
{
  if      (type_ == CDOC_DOT_COMMAND)
    delete (CDDotCommand *) data_;
  else if (type_ == CDOC_COLON_COMMAND)
    delete (CDColonCommand *) data_;
  else
    delete [] (char *) data_;

  data_ = NULL;
}

void
CDScriptLine::
print() const
{
  if      (type_ == CDOC_DOT_COMMAND) {
    CDDotCommand *dot_command = (CDDotCommand *) data_;

    dot_command->print();
  }
  else if (type_ == CDOC_COLON_COMMAND) {
    CDColonCommand *colon_command = (CDColonCommand *) data_;

    colon_command->print();
  }
  else
    printf("%3d> %s\n", cdoc_input_line_no, (char *) data_);
}

//------------

CDDotCommand::
CDDotCommand(const char *command, const char *text)
{
  literal_      = false;
  command_      = command;
  text_         = text;
  command_data_ = NULL;
}

CDDotCommand::
CDDotCommand(const CDDotCommand &dot_command)
{
  literal_      = dot_command.literal_;
  command_      = dot_command.command_;
  text_         = dot_command.text_;
  command_data_ = dot_command.command_data_;
}

CDDotCommand *
CDDotCommand::
dup()
{
  return new CDDotCommand(*this);
}

void
CDDotCommand::
print() const
{
  printf("%3d> ", cdoc_input_line_no);

  printf(".%s %s\n", command_.c_str(), text_.c_str());
}

//------------

CDColonCommand::
CDColonCommand() :
 command_data_(NULL)
{
}

CDColonCommand::
CDColonCommand(const CDColonCommand &colon_command)
{
  command_      = colon_command.command_;
  parameters_   = colon_command.parameters_;
  values_       = colon_command.values_;
  text_         = colon_command.text_;
  breaks_       = colon_command.breaks_;
  command_data_ = colon_command.command_data_;
}

CDColonCommand *
CDColonCommand::
dup()
{
  return new CDColonCommand(*this);
}

void
CDColonCommand::
print() const
{
  printf("%3d> ", cdoc_input_line_no);

  printf(":%s", command_.c_str());

  uint no_parameters = parameters_.size();

  for (uint i = 0; i < no_parameters; ++i) {
    printf(" %s", parameters_[i].c_str());

    if (values_[i] != "")
      printf("=%s", values_[i].c_str());
  }

  printf(".");

  printf("%s\n", text_.c_str());
}
