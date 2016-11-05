#ifndef CDOC_SCRIPT_PARAGRAPH_I_H
#define CDOC_SCRIPT_PARAGRAPH_I_H

/*-----------------------------------------------------------------------*/

#define NORMAL_PARAGRAPH     0
#define DEFINITION_PARAGRAPH 1
#define LIST_PARAGRAPH       2
#define SUB_LIST_PARAGRAPH1  3
#define SUB_LIST_PARAGRAPH2  4
#define GLOSSARY_PARAGRAPH   5

#define LEFT_JUSTIFICATION   0
#define CENTRE_JUSTIFICATION 1
#define RIGHT_JUSTIFICATION  2

class CDParagraph {
 public:
  typedef std::vector<int> HighlightList;

 private:
  int           type_;
  bool          formatted_;
  int           justification_;
  std::string   pre_line_;
  std::string   header_;
  std::string   prefix_;
  int           prefix_font_;
  bool          indent_prefix_;
  std::string   text_;
  int           font_;
  HighlightList highlight_list_;

 public:
  CDParagraph();
 ~CDParagraph();

  int getType() const { return type_; }

  void setType(int type) { type_ = type; }

  bool getFormatted() const { return formatted_; }

  void setFormatted(bool formatted) { formatted_ = formatted; }

  bool getJustification() const { return justification_; }

  void setJustification(int justification) { justification_ = justification; }

  const std::string &getPreLine() const { return pre_line_; }

  void setPreLine(const std::string &pre_line) { pre_line_ = pre_line; }

  const std::string &getHeader() const { return header_; }

  void setHeader(const std::string &header) { header_ = header; }

  const std::string &getPrefix() const { return prefix_; }

  int getPrefixFont() const { return prefix_font_; }

  bool getIndentPrefix() const { return indent_prefix_; }

  void setPrefix(const std::string &str, int font=CDOC_NORMAL_FONT, bool indent=true) {
    prefix_        = str;
    prefix_font_   = font;
    indent_prefix_ = indent;
  }

  const std::string &getText() const { return text_; }

  uint getTextLen() const { return text_.size(); }

  char getTextChar(uint i) const { return text_[i]; }

  void setText(const std::string &text) { text_ = text; }
  void addText(const std::string &text) { text_ += text; }

  uint getNumHighlights() const { return highlight_list_.size(); }

  void addHighlight(int highlight) { highlight_list_.push_back(highlight); }

  int getHighlight(uint i) const { return highlight_list_[i]; }

  void removeHighlight() { highlight_list_.pop_back(); }

  void clearHighlights() { highlight_list_.clear(); }
};

/*-----------------------------------------------------------------------*/

extern CDParagraph *cdoc_paragraph;
extern bool         cdoc_in_paragraph;
extern bool         cdoc_paragraph_done;
extern bool         cdoc_outputting_paragraph;
extern bool         cdoc_paragraph_last_line;

/*-----------------------------------------------------------------------*/

extern void        CDocScriptInitParagraphs
                    ();
extern void        CDocScriptTermParagraphs
                    ();
extern void        CDocScriptNewParagraph
                    (const std::string &, int, int, int);
extern void        CDocScriptAddStringToParagraph
                    (const std::string &);
extern const char *CDocScriptStartParagraphHighlight
                    (int);
extern const char *CDocScriptEndParagraphHighlight
                    (int);
extern void        CDocScriptTermParagraphHighlights
                    ();
extern void        CDocScriptSetParagraphPrefix
                    (const std::string &, int, int);
extern void        CDocScriptOutputParagraph
                    ();

#endif
