#include "CDocI.h"

#define CHARS_PER_INCH 12
#define LINES_PER_INCH 6

// Set the values in a structure from a set of parameters and values where the
// type and structure offset corresponding to each parameter is given in the
// parameter definition structure.
extern void
CDocExtractParameterValues(const std::vector<std::string> &parameters,
                           const std::vector<std::string> &values,
                           CDParameterData *parameter_data, char *structure)
{
  uint no_parameters = parameters.size();

  for (uint i = 0; i < no_parameters; ++i) {
    int j = 0;

    while (parameter_data[j].name != NULL) {
      if (strcmp(parameters[i].c_str(), parameter_data[j].name) == 0) {
        CDocExtractParameterValue(values[i], &parameter_data[j], structure);

        break;
      }

      j++;
    }

    if (parameter_data[j].name == NULL)
      CDocScriptError("Invalid Parameter '%s'", parameters[i].c_str());
  }
}

// Set the values in a structure from a set of parameters where the type and
// structure offset corresponding to each parameter is given in the parameter
// definition structure.
//
// This routine is similar to CDocExtractParameterValues() except that the parameters
// and associated values are not separated i.e. if a parameter requires a value
// the next parameter is used as a value and skipped.
extern void
CDocExtractParameters(const std::vector<std::string> &parameters,
                      CDParameterData *parameter_data, char *structure)
{
  int i = 0;

  while (parameter_data[i].name != NULL) {
    uint no_parameters = parameters.size();

    for (uint j = 0; j < no_parameters; j++) {
      if (strcmp(parameter_data[i].name, parameters[j].c_str()) == 0)
        CDocExtractParameterValue(parameters[j + 1], &parameter_data[i], structure);
    }

    ++i;
  }
}

// Set the values in a structure from a set of UNIX style options where the type and
// structure offset corresponding to each option is given in the option definition
// structure.
//
// This routine is similar to CDocExtractParameters() except that the options must
// conform to the UNIX style i.e. prefixed by a '-' character.
extern void
CDocExtractOptions(char **args, int *no_args,
                   CDOptionData *option_data, char *structure)
{
  int i = 0;

  while (i < *no_args) {
    if (args[i][0] != '-' || args[i][1] == '\0') {
      ++i;

      continue;
    }

    int j = 0;

    while (option_data[j].long_name != NULL) {
      if (strcmp(&args[i][1], option_data[j].long_name ) == 0 ||
          strcmp(&args[i][1], option_data[j].short_name) == 0) {
        CDParameterData parameter_data;

        parameter_data.type   = option_data[j].type;
        parameter_data.data   = option_data[j].data;
        parameter_data.offset = option_data[j].offset;

        CDocExtractParameterValue(args[i + 1], &parameter_data, structure);

        /* Remove 1 Arg */

        if (option_data[j].type == PARM_FLAG || option_data[j].type == PARM_NFLAG) {
          for (int k = i + 1; k < *no_args; k++)
            args[k - 1] = args[k];

          (*no_args)--;
        }

        /* Remove Two Args */

        else if (option_data[j].type != PARM_IGNORE) {
          for (int k = i + 2; k < *no_args; k++)
            args[k - 2] = args[k];

          *no_args -= 2;
        }

        /* Ignore 1 Arg */

        else
          ++i;

        break;
      }

      j++;
    }

    if (option_data[j].long_name == NULL)
      ++i;
  }
}

// Interpret the supplied value str using the supplied parameter definition and
// store the resultant value in the supplied structure.
//
// This routine is used by CDocExtractParameterValues(), CDocExtractParameters()
// and CDocExtractOptions().
extern void
CDocExtractParameterValue(const std::string &value, CDParameterData *parameter_data,
                          char *structure)
{
  char *address  = structure + parameter_data->offset;
  char *address1 = address   + sizeof(char *);

  if      (parameter_data->type == PARM_STR)
    *((char **) address) = (char *) value.c_str();
  else if (parameter_data->type == PARM_NEW_STR)
    *((char **) address) = CStrUtil::strdup(value.c_str());
  else if (parameter_data->type == PARM_INT)
    *((int *) address) = CStrUtil::toInteger(value);
  else if (parameter_data->type == PARM_REAL)
    *((double *) address) = CStrUtil::toReal(value);
  else if (parameter_data->type == PARM_CHOICE) {
    CDParameterChoiceData *parameter_choice_data = (CDParameterChoiceData *) parameter_data->data;

    CStrUtil::toLower(value);

    int i = 0;

    while (parameter_choice_data[i].name != NULL && parameter_choice_data[i].name[0] != '\0') {
      if (strcmp(value.c_str(), parameter_choice_data[i].name) == 0)
        break;

      ++i;
    }

    if (parameter_choice_data[i].name != NULL && parameter_choice_data[i].name[0] != '\0')
      *((char **) address) = (char *) parameter_choice_data[i].data;
    else if (parameter_choice_data[i].name != NULL) {
      int type = (long) parameter_choice_data[i].data;

      if      (type == PARM_STR)
        *((char **) address) = (char *) value.c_str();
      else if (type == PARM_NEW_STR)
        *((char **) address) = CStrUtil::strdup(value.c_str());
      else if (type == PARM_INT)
        *((int *) address) = CStrUtil::toInteger(value);
      else if (type == PARM_REAL)
        *((double *) address) = CStrUtil::toReal(value);
      else if (type == PARM_CLENSTR)
        *((int *) address) = CDocLengthStringToChars(value);
      else if (type == PARM_LLENSTR)
        *((int *) address) = CDocLengthStringToLines(value);
      else
        CDocScriptError("Parameter Type Invalid");
    }
    else
      CDocScriptError("Invalid Parameter Value '%s'", value.c_str());
  }
  else if (parameter_data->type == PARM_CLENSTR)
    *((int *) address) = CDocLengthStringToChars(value);
  else if (parameter_data->type == PARM_LLENSTR)
    *((int *) address) = CDocLengthStringToLines(value);
  else if (parameter_data->type == PARM_BOOLEAN) {
    CStrUtil::toLower(value);

    if      (value == "yes")
      *((int *) address) = true;
    else if (value == "no")
      *((int *) address) = false;
    else
      CDocScriptError("String '%s' is not Yes or No", value.c_str());
  }
  else if (parameter_data->type == PARM_FLAG)
    *((int *) address) = true;
  else if (parameter_data->type == PARM_NFLAG)
    *((int *) address) = false;
  else if (parameter_data->type == PARM_VALUE)
    *((char **) address) = (char *) parameter_data->data;
  else if (parameter_data->type == PARM_USER_PROC) {
    std::vector<std::string> words;

    CDocStringToWords(value, words);

    (*(CDocExtractParmProc *) address)(words);
  }
  else if (parameter_data->type == PARM_STR_ARRAY) {
    std::vector<std::string> words;

    CDocStringToWords(value, words);

    uint no_words = words.size();

    char **words1 = NULL;

    if (no_words > 0) {
      words1 = new char * [no_words];

      for (uint i = 0; i < no_words; ++i)
        words1[i] = CStrUtil::strdup(words[i]);
    }

    *((char ***) address ) = words1;
    *((int  *  ) address1) = no_words;
  }
  else if (parameter_data->type == PARM_INT_ARRAY) {
    std::vector<std::string> words;

    CDocStringToWords(value, words);

    uint no_words = words.size();

    int *integers = NULL;

    if (no_words > 0) {
      integers = new int [no_words];

      for (uint i = 0; i < no_words; ++i)
        integers[i] = CStrUtil::toInteger(words[i]);
    }

    *((int **) address ) = integers;
    *((int * ) address1) = no_words;
  }
  else if (parameter_data->type == PARM_REAL_ARRAY) {
    std::vector<std::string> words;

    CDocStringToWords(value, words);

    uint no_words = words.size();

    double *reals = NULL;

    if (no_words > 0) {
      reals = new double [no_words];

      for (uint i = 0; i < no_words; ++i)
        reals[i] = CStrUtil::toReal(words[i]);
    }

    *((double **) address ) = reals;
    *((int    * ) address1) = no_words;
  }
  else if (parameter_data->type == PARM_CHOICE_ARRAY) {
    CDParameterChoiceData *parameter_choice_data = (CDParameterChoiceData *) parameter_data->data;

    std::vector<std::string> words;

    CDocStringToWords(value, words);

    uint no_words = words.size();

    char **values = NULL;

    if (no_words > 0)
      values = new char * [no_words];

    for (uint i = 0; i < no_words; ++i) {
      CStrUtil::toLower(words[i]);

      int j = 0;

      while (parameter_choice_data[j].name != NULL && parameter_choice_data[j].name[0] != '\0') {
        if (strcmp(words[i].c_str(), parameter_choice_data[j].name) == 0)
          break;

        j++;
      }

      if (parameter_choice_data[i].name != NULL && parameter_choice_data[i].name[0] != '\0')
        *((char **) values[i]) = (char *) parameter_choice_data[i].data;
      else if (parameter_choice_data[i].name != NULL) {
        int type = (long) parameter_choice_data[i].data;

        if      (type == PARM_STR)
          *((char **) values[i]) = CStrUtil::strdup(words[i]);
        else if (type == PARM_INT)
          *((int *) values[i]) = CStrUtil::toInteger(words[i]);
        else if (type == PARM_REAL)
          *((double *) values[i]) = CStrUtil::toReal(words[i]);
        else if (type == PARM_CLENSTR)
          *((int *) values[i]) = CDocLengthStringToChars(words[i]);
        else if (type == PARM_LLENSTR)
          *((int *) values[i]) = CDocLengthStringToLines(words[i]);
        else
          CDocScriptError("Parameter Type Invalid");
      }
      else
        CDocScriptError("Invalid Parameter Value '%s'", words[i].c_str());
    }

    *((char ***) address ) = values;
    *((int  *  ) address1) = no_words;
  }
  else if (parameter_data->type == PARM_CLENSTR_ARRAY) {
    std::vector<std::string> words;

    CDocStringToWords(value, words);

    uint no_words = words.size();

    int *integers = NULL;

    if (no_words > 0) {
      integers = new int [no_words];

      for (uint i = 0; i < no_words; ++i)
        integers[i] = CDocLengthStringToChars(words[i]);
    }

    *((int **) address ) = integers;
    *((int * ) address1) = no_words;
  }
  else if (parameter_data->type == PARM_LLENSTR_ARRAY) {
    std::vector<std::string> words;

    CDocStringToWords(value, words);

    uint no_words = words.size();

    int *integers = NULL;

    if (no_words > 0) {
      integers = new int [no_words];

      for (uint i = 0; i < no_words; ++i)
        integers[i] = CDocLengthStringToLines(words[i]);
    }

    *((int **) address ) = integers;
    *((int * ) address1) = no_words;
  }
  else if (parameter_data->type == PARM_BOOL_ARRAY) {
    std::vector<std::string> words;

    CDocStringToWords(value, words);

    uint no_words = words.size();

    int *integers = NULL;

    if (no_words > 0) {
      integers = new int [no_words];

      for (uint i = 0; i < no_words; ++i) {
        CStrUtil::toLower(words[i]);

        if      (words[i] == "yes")
          integers[i] = true;
        else if (words[i] == "no")
          integers[i] = false;
        else
          CDocScriptError("String '%s' is not Yes or No", words[i].c_str());
      }
    }

    *((int **) address ) = integers;
    *((int * ) address1) = no_words;
  }
  else if (parameter_data->type == PARM_IGNORE)
    ;
  else
    CDocScriptError("Parameter Type Invalid");
}

// Format a str inside the specified width with each line aligned either to
// the left, right or centre.
extern void
CDocFormatStringInWidth(const std::string &text, int width, int align, char ***lines, int *no_lines)
{
  int   l;
  int   no;
  char  word[256];
  char  temp_string[256];
  char  temp_string1[256];

  if (width > 255)
    width = 255;

  int length = text.size();

  int max_lines = length + 1;

  *lines    = new char * [max_lines];
  *no_lines = 0;

  int i = 0;
  int j = 0;
  int k = 0;

  char last_char = '\0';

  while (i < length) {
    /* If we have an Escape Code in the Text then add it to the
       word but do not process it as it is not counted as a
       character */

    if      ((no = CDocIsEscapeCode(&text[i])) != 0) {
      for (; no > 0; no--)
        word[j++] = text[i++];
    }

    /* If we have a Page Reference then add to Current Word */

    else if ((no = CDocIsPageReference(&text[i])) != 0) {
      for (; no > 0; no--)
        word[j++] = text[i++];
    }

    /* If we have an inline Newline character then we want
       to break the text and start a new line */

    else if (text[i] == '\n') {
      /* Skip Newline Character */

      ++i;

      /* Terminate Word */

      word[j] = '\0';

      j = 0;

      /* If word does not fit on current line then add to
         to a new one, otherwise add word to line and start
         a new one */

      temp_string[k] = '\0';

      if ((last_char != '\0' &&
           CDocStringDisplayLength(temp_string) + CDocStringDisplayLength(word) + 1 > width) ||
          (last_char == '\0' &&
           CDocStringDisplayLength(temp_string) + CDocStringDisplayLength(word)     > width)) {
        /* If word does not fit on a line then chop it into
           pieces which will fit on lines. */

        if (CDocStringDisplayLength(word) > width) {
          if (last_char != '\0')
            temp_string[k++] = last_char;

          temp_string[k] = '\0';

          /* Add as much as possible of word to current line */

          l = 0;

          while (width - CDocStringDisplayLength(temp_string) > 0) {
            if      ((no = CDocIsEscapeCode(&word[l])) != 0) {
              for (; no > 0; no--)
                temp_string[k++] = word[l++];
            }
            else if ((no = CDocIsPageReference(&word[l])) != 0) {
              for (; no > 0; no--)
                temp_string[k++] = word[l++];
            }
            else
              temp_string[k++] = word[l++];

            temp_string[k] = '\0';
          }

          /* Start new line */

          (*lines)[*no_lines] = CStrUtil::strdup(temp_string);

          (*no_lines)++;

          /* Reset word */

          strcpy(temp_string, &word[l]);
          strcpy(word       , temp_string);

          /* Keep adding parts of word while longer than format
             width */

          temp_string[0] = '\0';

          while (CDocStringDisplayLength(word) > width) {
            /* Add as much as possible of word to current line */

            k = 0;
            l = 0;

            while (width - CDocStringDisplayLength(temp_string) > 0) {
              if      ((no = CDocIsEscapeCode(&word[l])) != 0) {
                for (; no > 0; no--)
                  temp_string[k++] = word[l++];
              }
              else if ((no = CDocIsPageReference(&word[l])) != 0) {
                for (; no > 0; no--)
                  temp_string[k++] = word[l++];
              }
              else
                temp_string[k++] = word[l++];

              temp_string[k] = '\0';
            }

            /* Start new line */

            (*lines)[*no_lines] = CStrUtil::strdup(temp_string);

            (*no_lines)++;

            /* Reset word */

            strcpy(temp_string, &word[l]);
            strcpy(word       , temp_string);

            temp_string[0] = '\0';
          }
        }
        else {
          /* Align line */

          CDocAlignStringInWidth(temp_string1, temp_string,
                                 width, align);

          /* Start new line */

          (*lines)[*no_lines] = CStrUtil::strdup(temp_string1);

          (*no_lines)++;
        }

        strcpy(temp_string, word);
      }
      else {
        if (last_char != '\0')
          temp_string[k++] = last_char;

        strcpy(&temp_string[k], word);
      }

      /* Align line */

      CDocAlignStringInWidth(temp_string1, temp_string,
                             width, align);

      /* Start new line */

      (*lines)[*no_lines] = CStrUtil::strdup(temp_string1);

      (*no_lines)++;

      k = 0;

      /* Reset last Space Character */

      last_char = '\0';
    }

    /* If we have a character at which we can split the line
       (punctuation or space) then check against current
       word whether the line should be continued or a new
       line should be started */

    else if (CDocIsSplitter(&text[i])) {
      /* If not space then add to word (space is collapsed) */

      if (! isspace(text[i]))
        word[j++] = text[i];

      /* Terminate Word */

      word[j] = '\0';

      j = 0;

      /* If word does not fit on current line then add to
         to a new one, otherwise add previous space character
         and word to current line */

      if ((last_char != '\0' && k + CDocStringDisplayLength(word) + 1 > width) ||
          (last_char == '\0' && k + CDocStringDisplayLength(word)     > width)) {
        /* If word does not fit on a line then chop it into
           pieces which will fit on lines. */

        if (CDocStringDisplayLength(word) > width) {
          if (last_char != '\0')
            temp_string[k++] = last_char;

          temp_string[k] = '\0';

          /* Add as much as possible of word to current line */

          l = 0;

          while (width - CDocStringDisplayLength(temp_string) > 0) {
            if      ((no = CDocIsEscapeCode(&word[l])) != 0) {
              for (; no > 0; no--)
                temp_string[k++] = word[l++];
            }
            else if ((no = CDocIsPageReference(&word[l])) != 0) {
              for (; no > 0; no--)
                temp_string[k++] = word[l++];
            }
            else
              temp_string[k++] = word[l++];

            temp_string[k] = '\0';
          }

          /* Start new line */

          (*lines)[*no_lines] = CStrUtil::strdup(temp_string);

          (*no_lines)++;

          /* Reset word */

          strcpy(temp_string, &word[l]);
          strcpy(word       , temp_string);

          /* Keep adding parts of word while longer than format
             width */

          temp_string[0] = '\0';

          while (CDocStringDisplayLength(word) > width) {
            /* Add as much as possible of word to current line */

            k = 0;
            l = 0;

            while (width - CDocStringDisplayLength(temp_string) > 0) {
              if      ((no = CDocIsEscapeCode(&word[l])) != 0) {
                for (; no > 0; no--)
                  temp_string[k++] = word[l++];
              }
              else if ((no = CDocIsPageReference(&word[l])) != 0) {
                for (; no > 0; no--)
                  temp_string[k++] = word[l++];
              }
              else
                temp_string[k++] = word[l++];

              temp_string[k] = '\0';
            }

            /* Start new line */

            (*lines)[*no_lines] = CStrUtil::strdup(temp_string);

            (*no_lines)++;

            /* Reset word */

            strcpy(temp_string, &word[l]);
            strcpy(word       , temp_string);

            temp_string[0] = '\0';
          }
        }
        else {
          /* Align line */

          temp_string[k] = '\0';

          CDocAlignStringInWidth(temp_string1, temp_string,
                                 width, align);

          /* Start new line */

          (*lines)[*no_lines] = CStrUtil::strdup(temp_string1);

          (*no_lines)++;
        }

        strcpy(temp_string, word);

        k = strlen(word);
      }
      else {
        if (last_char != '\0')
          temp_string[k++] = last_char;

        strcpy(&temp_string[k], word);

        k += strlen(word);
      }

      /* Save last Space Character */

      if (isspace(text[i]) && k > 0 && k < width)
        last_char = text[i];
      else
        last_char = '\0';

      ++i;
    }

    /* Any other character is just added to the Current Word */

    else
      word[j++] = text[i++];
  }

  /* Terminate Word */

  word[j] = '\0';

  /* If word does not fit on current line then add to to a new one,
     otherwise add previous space character and word to current line */

  if ((last_char != '\0' && k + CDocStringDisplayLength(word) + 1 > width) ||
      (last_char == '\0' && k + CDocStringDisplayLength(word)     > width)) {
    /* If word does not fit on a line then chop it into
       pieces which will fit on lines. */

    if (CDocStringDisplayLength(word) > width) {
      if (last_char != '\0')
        temp_string[k++] = last_char;

      /* Add as much as possible of word to current line */

      temp_string[k] = '\0';

      l = 0;

      while (width - CDocStringDisplayLength(temp_string) > 0) {
        if      ((no = CDocIsEscapeCode(&word[l])) != 0) {
          for (; no > 0; no--)
            temp_string[k++] = word[l++];
        }
        else if ((no = CDocIsPageReference(&word[l])) != 0) {
          for (; no > 0; no--)
            temp_string[k++] = word[l++];
        }
        else
          temp_string[k++] = word[l++];

        temp_string[k] = '\0';
      }

      /* Start new line */

      (*lines)[*no_lines] = CStrUtil::strdup(temp_string);

      (*no_lines)++;

      /* Reset word */

      strcpy(temp_string, &word[l]);
      strcpy(word       , temp_string);

      /* Keep adding parts of word while longer than format width */

      temp_string[0] = '\0';

      while (CDocStringDisplayLength(word) > width) {
        /* Add as much as possible of word to current line */

        k = 0;
        l = 0;

        while (width - CDocStringDisplayLength(temp_string) > 0) {
          if      ((no = CDocIsEscapeCode(&word[l])) != 0) {
            for (; no > 0; no--)
              temp_string[k++] = word[l++];
          }
          else if ((no = CDocIsPageReference(&word[l])) != 0) {
            for (; no > 0; no--)
              temp_string[k++] = word[l++];
          }
          else
            temp_string[k++] = word[l++];

          temp_string[k] = '\0';
        }

        /* Start new line */

        (*lines)[*no_lines] = CStrUtil::strdup(temp_string);

        (*no_lines)++;

        /* Reset word */

        strcpy(temp_string, &word[l]);
        strcpy(word       , temp_string);

        temp_string[0] = '\0';
      }

      strcpy(temp_string, word);
    }
    else {
      /* Align line */

      temp_string[k] = '\0';

      CDocAlignStringInWidth(temp_string1, temp_string,
                             width, align);

      /* Start new line */

      (*lines)[*no_lines] = CStrUtil::strdup(temp_string1);

      (*no_lines)++;

      strcpy(temp_string, word);
    }
  }
  else {
    if (last_char != '\0')
      temp_string[k++] = last_char;

    strcpy(&temp_string[k], word);

    k += strlen(word);

    temp_string[k] = '\0';
  }

  if (CDocStringDisplayLength(temp_string) > 0) {
    /* Align line */

    CDocAlignStringInWidth(temp_string1, temp_string,
                           width, align);

    /* Start new line */

    (*lines)[*no_lines] = CStrUtil::strdup(temp_string1);

    (*no_lines)++;
  }
}

// Free off the strings allocated by the CDocFormatStringInWidth() routine.
extern void
CDocFreeFormattedStrings(char **lines, int no_lines)
{
  if (lines != NULL) {
    for (int i = 0; i < no_lines; ++i)
      delete lines[i];

    delete lines;
  }
}

// Align a str to the left, right, centre or left and right of a fixed width str.
extern void
CDocAlignStringInWidth(char *str, char *align_string, int width, int align)
{
  int    i;
  int    j;
  char  *p;
  int    no_spaces;

  if (align == CHALIGN_TYPE_JUSTIFY) {
    int         no;
    int         *spaces;
    int         no_chars;
    int         no_words;
    static int  flag = 0;

    no_chars = 0;
    no_words = 1;

    p = align_string;

    while (*p != '\0') {
      if      ((no = CDocIsEscapeCode(p)) != 0)
        p += no;
      else if ((no = CDocIsPageReference(p)) != 0) {
        p += no;

        no_chars += 2;
      }
      else if (! isspace(*p)) {
        no_chars++;

        p++;
      }
      else {
        p++;

        CStrUtil::skipSpace(&p);

        no_words++;
      }
    }

    no_spaces = width - no_chars;

    if (no_words > 1) {
      spaces = new int [no_words - 1];

      for (i = 0; i < no_words - 1; ++i)
        spaces[i] = 0;

      if (flag == 0) {
        i = 0;

        while (no_spaces > 0) {
          spaces[i++]++;

          no_spaces--;

          if (i >= no_words - 1)
            i = 0;
        }
      }
      else {
        i = no_words - 2;

        while (no_spaces > 0) {
          spaces[i--]++;

          no_spaces--;

          if (i < 0)
            i = no_words - 2;
        }
      }

      i = 0;

      no_words = 0;

      p = align_string;

      while (*p != '\0') {
        if (isspace(*p)) {
          CStrUtil::skipSpace(&p);

          for (j = 0; j < spaces[no_words]; j++)
            str[i++] = ' ';

          no_words++;
        }
        else
          str[i++] = *p++;
      }

      str[i] = '\0';

      delete spaces;
    }
    else {
      i = 0;

      p = align_string;

      while (*p != '\0')
        str[i++] = *p++;

      while (no_spaces > 0) {
        str[i++] = ' ';

        no_spaces--;
      }

      str[i] = '\0';
    }
  }
  else {
    int  spaces1;
    int  spaces2;

    no_spaces = width - CDocStringDisplayLength(align_string);

    if      (align == CHALIGN_TYPE_LEFT) {
      spaces1 = 0;
      spaces2 = no_spaces;
    }
    else if (align == CHALIGN_TYPE_CENTRE) {
      spaces1 = no_spaces/2;
      spaces2 = no_spaces - no_spaces/2;
    }
    else if (align == CHALIGN_TYPE_RIGHT) {
      spaces1 = no_spaces;
      spaces2 = 0;
    }
    else {
      spaces1 = 0;
      spaces2 = no_spaces;
    }

    i = 0;

    for (j = 0; j < spaces1; j++)
      str[i++] = ' ';

    p = align_string;

    while (*p != '\0')
      str[i++] = *p++;

    for (j = 0; j < spaces2; j++)
      str[i++] = ' ';

    str[i] = '\0';
  }

  return;

}

// Extract strings from the supplied source str where each str is separated
// by the supplied character.
extern void
CDocStringToFields(char *str, char ***fields, int *no_fields, int field_separator)
{
  int  j;
  char temp_string[256];

  *no_fields = 0;

  int i = 0;

  while (str[i] != '\0') {
    while (str[i] != '\0' && str[i] == field_separator)
      ++i;

    while (str[i] != '\0' && str[i] != field_separator)
      ++i;

    (*no_fields)++;
  }

  if (*no_fields != 0)
    *fields = new char * [*no_fields];
  else
    *fields = NULL;

  *no_fields = 0;

  i = 0;

  while (str[i] != '\0') {
    while (str[i] != '\0' && str[i] == field_separator)
      ++i;

    j = 0;

    while (str[i] != '\0' && str[i] != field_separator)
      temp_string[j++] = str[i++];
    temp_string[j] = '\0';

    (*fields)[(*no_fields)++] = CStrUtil::strdup(temp_string);
  }
}

// Free off the strings allocated by the CDocStringToFields() routine.
extern void
CDocFreeStringFields(char **fields, int no_fields)
{
  if (no_fields > 0) {
    for (int i = 0; i < no_fields; ++i)
      delete fields[i];

    delete fields;
  }
}

// Extract words from the supplied source str.
//
// The definition of a word is a space separated str not in single or double quotes.
extern void
CDocStringToWords(const std::string &str, std::vector<std::string> &words)
{
  uint i   = 0;
  uint len = str.size();

  while (i < len) {
    CStrUtil::skipSpace(str, &i);

    std::string word;

    if (i < len && str[i] == '\"') {
      ++i;

      while (i < len && str[i] != '\"')
        word += str[i++];

      if (i < len && str[i] == '\"')
        ++i;
    }
    else {
      while (i < len && ! isspace(str[i]))
        word += str[i++];
    }

    words.push_back(word);
  }
}

// Count the number of characters in a str which will actually be displayed when the
// str is output i.e. Escape Code combinations are not counted.
extern int
CDocStringDisplayLength(const std::string &str)
{
  int length = 0;

  if (str != "") {
    const char *p = str.c_str();

    /* Count all characters excluding Escape Codes */

    while (*p != '\0') {
      int no;

      if      ((no = CDocIsEscapeCode(p)) != 0)
        p += no;
      else if ((no = CDocIsPageReference(p)) != 0) {
        p += no;

        length += 2;
      }
      else {
        length++;

        p++;
      }
    }
  }

  return length;
}

// Remove any embedded escape codes from a str.
extern char *
CDocStringRemoveEscapeCodes(char *str)
{
  char *string1 = CStrUtil::strdup(str);

  int i = 0;
  int j = 0;

  while (str[i] != '\0') {
    int no;

    if ((no = CDocIsEscapeCode(&str[i])) != 0) {
      if (CDOC_IS_GOTO_ESC(&str[i])) {
        i += no;

        while (str[i] != '\0') {
          if (CDOC_IS_GOTO_ESC(&str[i]))
            break;

          ++i;
        }

        if (str[i] != '\0') {
          i += 2;

          if (str[i] != '\0')
            ++i;
        }
      }
      else
        i += no;
    }
    else
      string1[j++] = str[i++];
  }

  string1[j] = '\0';

  return string1;
}

// Convert and IBM Script Width Specification String into a (reasonably) equivalent
// number of characters for the current output device.
//
// Does a very simplistic conversion.
extern int
CDocLengthStringToChars(const std::string &str)
{
  int len = str.size();

  char *value_string = new char [len + 1];

  int i = 0;
  int j = 0;

  CStrUtil::skipSpace(str, &i);

  if (str[i] == 'c' || str[i] == 'C' ||
      str[i] == 'p' || str[i] == 'P')
    ++i;

  while (i < len && isdigit(str[i]))
    value_string[j++] = str[i++];

  if (str[i] == '.' || str[i] == ',') {
    ++i;

    value_string[j++] = '.';

    while (i < len && isdigit(str[i]))
      value_string[j++] = str[i++];
  }

  value_string[j] = '\0';

  if (value_string[0] == '\0') {
    CDocScriptError("Invalid Character Length String '%s'", str.c_str());

    return -1;
  }

  int length;

  if      (CStrUtil::casencmp(str.substr(i), "i", 1) == 0) {
    length = int(CStrUtil::toReal(value_string)*CHARS_PER_INCH);

    ++i;
  }
  else if (CStrUtil::casencmp(str.substr(i), "cm", 2) == 0) {
    length = int(CStrUtil::toReal(value_string)*(CHARS_PER_INCH/2.54));

    i += 2;
  }
  else if (CStrUtil::casencmp(str.substr(i), "c", 1) == 0) {
    length = int(CStrUtil::toReal(value_string)*(CHARS_PER_INCH/6.0));

    ++i;

    j = 0;

    while (i < len && isdigit(str[i]))
      value_string[j++] = str[i++];

    if (str[i] == '.' || str[i] == ',') {
      ++i;

      value_string[j++] = '.';

      while (i < len && isdigit(str[i]))
        value_string[j++] = str[i++];
    }

    value_string[j] = '\0';

    if (value_string[0] != '\0')
      length += int(CStrUtil::toReal(value_string));
  }
  else if (CStrUtil::casencmp(str.substr(i), "mm", 2) == 0) {
    length = int(CStrUtil::toReal(value_string)*(CHARS_PER_INCH/25.4));

    i += 2;
  }
  else if (CStrUtil::casencmp(str.substr(i), "p", 1) == 0) {
    length = int(CStrUtil::toReal(value_string)*(CHARS_PER_INCH/6.0));

    ++i;

    j = 0;

    while (i < len && isdigit(str[i]))
      value_string[j++] = str[i++];

    if (str[i] == '.' || str[i] == ',') {
      ++i;

      value_string[j++] = '.';

      while (i < len && isdigit(str[i]))
        value_string[j++] = str[i++];
    }

    value_string[j] = '\0';

    if (value_string[0] != '\0')
      length += int(CStrUtil::toReal(value_string));
  }
  else if (i >= len || isspace(str[i]))
    length = int(CStrUtil::toReal(value_string));

  CStrUtil::skipSpace(str, &i);

  if (i < len)
    CDocScriptWarning("Extra Characters '%s' after Character Length String '%s' Ignored",
                      str.substr(i).c_str(), str.c_str());

  delete [] value_string;

  if (length == 0)
    length = -1;

  return length;
}

// Convert and IBM Script Height Specification String into a (reasonably) equivalent
// number of lines for the current output device.
//
// Does a very simplistic conversion.
extern int
CDocLengthStringToLines(const std::string &str)
{
  int len = str.size();

  std::string value_string;

  int i = 0;

  CStrUtil::skipSpace(str, &i);

  if (i < len && (str[i] == 'c' || str[i] == 'C' || str[i] == 'p' || str[i] == 'P'))
    ++i;

  while (i < len && isdigit(str[i]))
    value_string += str[i++];

  if (i < len && (str[i] == '.' || str[i] == ',')) {
    ++i;

    value_string += '.';

    while (i < len && isdigit(str[i]))
      value_string += str[i++];
  }

  if (value_string == "") {
    CDocScriptError("Invalid Line Length String '%s'", str.c_str());
    return -1;
  }

  int length;

  if      (CStrUtil::casecmp(str.substr(i, 1), "i") == 0) {
    length = int(CStrUtil::toReal(value_string)*LINES_PER_INCH);

    ++i;
  }
  else if (CStrUtil::casecmp(str.substr(i, 2), "cm") == 0) {
    length = int(CStrUtil::toReal(value_string)*(LINES_PER_INCH/2.54));

    i += 2;
  }
  else if (CStrUtil::casecmp(str.substr(i, 1), "c") == 0) {
    length = int(CStrUtil::toReal(value_string)*(LINES_PER_INCH/6.0));

    ++i;

    std::string value_string;

    while (i < len && isdigit(str[i]))
      value_string += str[i++];

    if (i < len && (str[i] == '.' || str[i] == ',')) {
      ++i;

      value_string += '.';

      while (i < len && isdigit(str[i]))
        value_string += str[i++];
    }

    if (value_string != "")
      length += int(CStrUtil::toReal(value_string));
  }
  else if (CStrUtil::casecmp(str.substr(i, 2), "mm") == 0) {
    length = int(CStrUtil::toReal(value_string)*(LINES_PER_INCH/25.4));

    i += 2;
  }
  else if (CStrUtil::casecmp(str.substr(i, 1), "p") == 0) {
    length = int(CStrUtil::toReal(value_string)*(LINES_PER_INCH/6.0));

    ++i;

    std::string value_string;

    while (i < len && isdigit(str[i]))
      value_string += str[i++];

    if (i < len && (str[i] == '.' || str[i] == ',')) {
      ++i;

      value_string += '.';

      while (i < len && isdigit(str[i]))
        value_string += str[i++];
    }

    if (value_string != "")
      length += int(CStrUtil::toReal(value_string));
  }
  else if (i >= len || isspace(str[i]))
    length = int(CStrUtil::toReal(value_string));

  CStrUtil::skipSpace(str, &i);

  if (i < len)
    CDocScriptWarning("Extra Characters '%s' after Line Length String '%s' Ignored",
                      str.substr(i).c_str(), str.c_str());

  if (length == 0)
    length = -1;

  return length;
}

// Converts the specified number of characters to the equivalent number of em's
// for Troff output.
extern double
CDocCharsToEms(int chars)
{
  return (0.5*chars);
}

// Returns whether the initial characters of the supplied str represent an Escape Code
// used to set the current font style, set the current colour, specify a hypertext link,
// specify a character fill or specify an inline font change.
//
// The actual value returned is the number of characters in the escape code.
extern int
CDocIsEscapeCode(const char *str)
{
  if (str[0] == ESC) {
    if      (CDOC_IS_FONT_ESC(str))
      return 4;
    else if (CDOC_IS_FOREGROUND_ESC(str))
      return 5;
    else if (CDOC_IS_GOTO_ESC(str)) {
      int i = 2;

      while (str[i] != '\0' && str[i] != 'B' && str[i] != 'E')
        ++i;

      if (str[i] != 'B' && str[i] != 'E')
        return 0;

      ++i;

      return i;
    }
    else if (CDOC_IS_HOOK_ESC(str)) {
      int i = 2;

      if (str[i] != 'B' && str[i] != 'E')
        return 0;

      if (str[i] == 'B') {
        ++i;

        while (str[i] != '\0' && str[i] != '\033')
          ++i;

        if (str[i] != '\033')
          return 0;

        ++i;
      }
      else
        ++i;

      return i;
    }
    else if (CDOC_IS_FILL_ESC(str)) {
      int i = 2;

      while (str[i] != '\0' && isdigit(str[i]))
        ++i;

      if (str[i] != ';')
        return 0;

      ++i;

      while (str[i] != '\0' && isdigit(str[i]))
        ++i;

      if (str[i] != ';')
        return 0;

      ++i;

      ++i;

      return i;
    }
    else
      return 0;
  }
  else if (CDOC_IS_INLINE_FONT_ESC(str)) {
    if (str[1] == 'E')
      return 2;

    int i = 2;

    int c = str[++i];

    while (str[i] != '\0' && str[i] != c)
      ++i;

    if (str[i] != c)
      return 0;

    ++i;

    return i;
  }
  else
    return 0;
}

// Returns whether the initial characters of the supplied str represent a CDoc
// Unresolved Page Reference.
extern int
CDocIsPageReference(const char *str)
{
  int  i;

  if (strncmp(str, "##cdoc_ref_pg", 13) == 0 ||
      strncmp(str, "##cdoc_toc_pg", 13) == 0 ||
      strncmp(str, "##cdoc_fig_pg", 13) == 0 ||
      strncmp(str, "##cdoc_tbl_pg", 13) == 0 ||
      strncmp(str, "##cdoc_ih1_pg", 13) == 0 ||
      strncmp(str, "##cdoc_ih2_pg", 13) == 0 ||
      strncmp(str, "##cdoc_ih3_pg", 13) == 0) {
    i = 12;

    while (str[i] != '\0' && str[i] != '#')
      ++i;

    if (str[i] == '#' && str[i + 1] == '#')
      return (i + 2);
    else
      return 0;
  }

  return 0;
}

// Returns whether the characters at the start of the supplied str defines a
// splitter (i.e. a punctuation character/str).
extern int
CDocIsSplitter(const char *str)
{
  /* Space is a Splitter */

  if (isspace(str[0]))
    return true;

  /* Punctuation Characters followed by an End of String or
     a Space are Splitters */

  if (ispunct(str[0]) && (str[1] == '\0' || isspace(str[1])))
    return true;

  /* Comma, Colon, Semi Colon, Question Mark and Exclamation Mark
     followed by an End of String or a non-Punctuation Character
     is a Splitter */

  if (strchr(",:;?!", str[0]) != NULL &&
      (str[1] == '\0' || ! ispunct(str[1])))
     return true;

  /* Any other characters are not Splitters */

  return false;
}

// Check whether the specifier pointer into a str is inside an Escape Code or not.
extern int
CDocInEscapeCode(const char *str, const char *p)
{
  const char *p1 = p;

  while (p1 >= str) {
    int no;

    if ((no = CDocIsEscapeCode(p1)) != 0) {
      if (p1 + no >= p)
        return true;
      else
        return false;
    }
    else
      p1--;
  }

  return false;
}

// Get the previous (non escape) character to that pointed to by the supplied
// pointer into a str.
extern char *
CDocPrevChar(char *str, char *p)
{
  if (p == str)
    return NULL;

  char *p1 = str;
  char *p2 = str;

  for (;;) {
    int no;

    if ((no = CDocIsEscapeCode(p2)) != 0)
      p2 += no;
    else {
      p1 = p2;

      p2++;
    }

    if (p2 >= p)
      break;
  }

  return p1;
}

// Get the next (non escape) character to that pointed to by the supplied pointer.
extern char *
CDocNextChar(char *p)
{
  char *p1 = p;

  while (*p1 != '\0') {
    int no;

    if ((no = CDocIsEscapeCode(p1)) != 0)
      p1 += no;
    else
      break;
  }

  return p1;
}

// Read the next line from the specified file up to the next new line character or
// the end of the file.
extern bool
CDocReadLineFromFile(FILE *fp, std::string &line)
{
  int c = fgetc(fp);

  if (c == EOF)
    return false;

  line = "";

  while (c != EOF && c != '\n') {
    line += c;

    c = fgetc(fp);
  }

  return true;
}
