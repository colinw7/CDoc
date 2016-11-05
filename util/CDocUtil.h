#ifndef CDOC_UTIL_H
#define CDOC_UTIL_H

class CDocOutputFormat {
 public:
  // Start/End Normal Text
  virtual const char *startNormal() const = 0;
  virtual const char *endNormal() const = 0;

  // Start/End Bold Text
  virtual const char *startBold() const = 0;
  virtual const char *endBold() const = 0;

  // Start/End Underline Text
  virtual const char *startUnderline() const = 0;
  virtual const char *endUnderline() const = 0;

  // Start/End Bold Underline Text
  virtual const char *startBoldUnderline() const = 0;
  virtual const char *endBoldUnderline() const = 0;
};

class CDocCDocFormat : public CDocOutputFormat {
 public:
  const char *startNormal() const;
  const char *endNormal() const;

  const char *startBold() const;
  const char *endBold() const;

  const char *startUnderline() const;
  const char *endUnderline() const;

  const char *startBoldUnderline() const;
  const char *endBoldUnderline() const;
};

class CDocRawCCFormat : public CDocOutputFormat {
 public:
  const char *startNormal() const;
  const char *endNormal() const;

  const char *startBold() const;
  const char *endBold() const;

  const char *startUnderline() const;
  const char *endUnderline() const;

  const char *startBoldUnderline() const;
  const char *endBoldUnderline() const;
};

class CDocRawFormat : public CDocOutputFormat {
 public:
  const char *startNormal() const;
  const char *endNormal() const;

  const char *startBold() const;
  const char *endBold() const;

  const char *startUnderline() const;
  const char *endUnderline() const;

  const char *startBoldUnderline() const;
  const char *endBoldUnderline() const;
};

class CDocHtmlFormat : public CDocOutputFormat {
 public:
  const char *startNormal() const;
  const char *endNormal() const;

  const char *startBold() const;
  const char *endBold() const;

  const char *startUnderline() const;
  const char *endUnderline() const;

  const char *startBoldUnderline() const;
  const char *endBoldUnderline() const;
};

class CDocILeafFormat : public CDocOutputFormat {
 public:
  const char *startNormal() const;
  const char *endNormal() const;

  const char *startBold() const;
  const char *endBold() const;

  const char *startUnderline() const;
  const char *endUnderline() const;

  const char *startBoldUnderline() const;
  const char *endBoldUnderline() const;
};

class CDocTextFormat : public CDocOutputFormat {
 public:
  const char *startNormal() const;
  const char *endNormal() const;

  const char *startBold() const;
  const char *endBold() const;

  const char *startUnderline() const;
  const char *endUnderline() const;

  const char *startBoldUnderline() const;
  const char *endBoldUnderline() const;
};

#endif
