#ifndef CDOC_CONTROL_H
#define CDOC_CONTROL_H

#include <map>

/*****************************************************************************/

/* Defines */

#define CDocInputScript     "input_script"
#define CDocInputCDoc       "input_cdoc"
#define CDocInputRaw        "input_raw"
#define CDocInputShowcase   "input_showcase"
#define CDocInputPostscript "input_postscript"
#define CDocInputXWD        "input_xwd"
#define CDocInputHTML       "input_html"
#define CDocInputILeaf      "input_ileaf"

#define CDocOutputCDoc       "output_cdoc"
#define CDocOutputTroff      "output_troff"
#define CDocOutputRawCC      "output_raw_cc"
#define CDocOutputRaw        "output_raw"
#define CDocOutputShowcase   "output_showcase"
#define CDocOutputPostscript "output_postscript"
#define CDocOutputXWD        "output_xwd"
#define CDocOutputHTML       "output_html"
#define CDocOutputILeaf      "output_ileaf"

enum CDocOutputFormat {
  CDOC_OUTPUT_CDOC       = 1,
  CDOC_OUTPUT_TROFF      = 2,
  CDOC_OUTPUT_RAW_CC     = 3,
  CDOC_OUTPUT_RAW        = 4,
  CDOC_OUTPUT_SHOWCASE   = 5,
  CDOC_OUTPUT_POSTSCRIPT = 6,
  CDOC_OUTPUT_XWD        = 7,
  CDOC_OUTPUT_HTML       = 8,
  CDOC_OUTPUT_ILEAF      = 9
};

class  CDProgramHelps;
struct CDTempFile;

#define CDocInst CDoc::getInstance()

class CDoc {
 private:
  typedef std::map<std::string,CDProgramHelps *> ProgramHelpsMap;

  uint             debug_level_;
  ProgramHelpsMap  helps_list_;
  CDocOutputFormat output_format_;
  std::string      output_filename_;
  std::string      print_command_;
  bool             icon_bar_;
  bool             show_header_;
  bool             show_footer_;
  bool             ragged_;
  bool             first_init_;

 public:
  static CDoc *getInstance();

  void setDebug(uint debugLevel);
  uint getDebug() const;

  void initProgramHelps(const std::string &program, const std::string &directory);
  void termProgramHelps(const std::string &program);

  CDProgramHelps *getProgramHelps(const std::string &program) const;

  bool anyProgramHelps() const;

  void setOutputFormat(const std::string &);

  CDocOutputFormat getOutputFormat() const;

  void setOutputFilename(const std::string &);

  const std::string &getOutputFilename() const;

  void setPrintCommand(const std::string &);

  const std::string &getPrintCommand() const;

  void setIconBar(bool);
  bool getIconBar() const;

  void setShowHeader(bool);
  bool getShowHeader() const;

  void setShowFooter(bool);
  bool getShowFooter() const;

  void setRagged(bool);
  bool getRagged() const;

  std::string getTempFileName() const;

  CDTempFile *addTemporaryFile(const std::string &);

  void deleteTemporaryFiles(const std::string &);

  void deleteAllTemporaryFiles();

 private:
  CDoc();
 ~CDoc();
};

#endif
