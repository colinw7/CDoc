#ifndef CDOC_SCRIPT_LINE_I_H
#define CDOC_SCRIPT_LINE_I_H

/*---------------------------------------------------------------------*/

class CDDotCommand;
class CDColonCommand;

enum CDocLineType {
  CDOC_NO_COMMAND    = 0,
  CDOC_DOT_COMMAND   = 1,
  CDOC_COLON_COMMAND = 2,
  CDOC_NORMAL_TEXT   = 3,
  CDOC_CENTRED_TEXT  = 4,
  CDOC_RAW_TEXT      = 5
};

class CDScriptLine {
 private:
  CDocLineType  type_;
  void         *data_;

 public:
  CDScriptLine(CDocLineType type, const std::string &data);
  CDScriptLine(CDDotCommand *dot_command);
  CDScriptLine(CDColonCommand *colon_command);
  CDScriptLine(const CDScriptLine &script_line);
 ~CDScriptLine();

  void reset();

  CDScriptLine *dup();

  void setType(CDocLineType type) { type_ = type; }

  CDocLineType getType() const { return type_; }

  void setData(CDocLineType type, const std::string &data);
  void setData(CDDotCommand *dot_command);
  void setData(CDColonCommand *colon_command);

  std::string     getData        ();
  CDDotCommand   *getDotCommand  ();
  CDColonCommand *getColonCommand();

  void print() const;
};

/*--------------------------------------------------------------------*/

#define DOT_TYPE_NONE 0
#define DOT_TYPE_ONE  1

struct CDDotCommandData {
  const char *command;
  int         type;
  int         break_line;
  int         flush;
  int         supported;
};

class CDDotCommand {
 private:
  bool              literal_;
  std::string       command_;
  std::string       text_;
  CDDotCommandData *command_data_;

 public:
  CDDotCommand(const char *command="", const char *text="");
  CDDotCommand(const CDDotCommand &dot_command);

 ~CDDotCommand() { }

  CDDotCommand *dup();

  bool getLiteral() const { return literal_; }
  void setLiteral(bool literal) { literal_ = literal; }

  const std::string &getCommand() const { return command_; }
  const std::string &getText   () const { return text_; }

  CDDotCommandData *getCommandData() const { return command_data_; }

  void setCommand(const std::string &command) { command_ = command; }

  void setText(const std::string &text) { text_ = text; }

  void setCommandData(CDDotCommandData *command_data) {
    command_data_ = command_data;
  }

  void print() const;
};

/*--------------------------------------------------------------------*/

#define TEXT_NONE       0
#define TEXT_PARAGRAPH  1
#define TEXT_LINE       2

#define PARAMETER_NONE   0
#define PARAMETER_VALUES 1
#define PARAMETER_MIXED  2

struct CDColonCommandData {
  const char *command;
  int         in_line;
  int         no_parameters;
  int         parameter_types;
  int         text_type;
  int         flush;
  const char *end_command;
};

class CDColonCommand {
 private:
  std::string              command_;
  std::vector<std::string> parameters_;
  std::vector<std::string> values_;
  std::string              text_;
  std::vector<int>         breaks_;
  CDColonCommandData*      command_data_;

 public:
  CDColonCommand();

  CDColonCommand(const CDColonCommand &colon_command);

  CDColonCommand *dup();

  const std::string &getCommand() const { return command_; }

  void setCommand(const std::string &command) { command_ = command; }

  const std::vector<std::string> &getParameters() const { return parameters_; }

  const std::vector<std::string> &getValues() const { return values_; }

  void addParameter(const std::string &name, const std::string &value) {
    parameters_.push_back(name);
    values_    .push_back(value);
  }

  uint getNumParameters() const { return parameters_.size(); }

  void getParameter(uint i, std::string &name, std::string &value) const {
    name  = parameters_[i];
    value = values_    [i];
  }

  void setParameter(uint i, const std::string &name, const std::string &value) {
    parameters_[i] = name;
    values_    [i] = value;
  }

  const std::string getParameter(uint i) const { return parameters_[i]; }
  const std::string getValue    (uint i) const { return values_    [i]; }

  const std::string &getText() const { return text_; }

  void setText(const std::string &text) { text_ = text; }

  void addBreak(int pos) { breaks_.push_back(pos); }

  uint getNumBreaks() const { return breaks_.size(); }

  void getBreak(uint i, int &pos) const { pos = breaks_[i]; }

  int getBreak(uint i) const { return breaks_[i]; }

  CDColonCommandData *getCommandData() const { return command_data_; }

  void setCommandData(CDColonCommandData *command_data) { command_data_ = command_data; }

  void print() const;
};

/*---------------------------------------------------------------------*/

extern CDScriptLine *CDocScriptGetNextLine
                      ();

#endif
