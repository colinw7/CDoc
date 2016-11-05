#ifndef CDOC_HELP_H
#define CDOC_HELP_H

/*****************************************************************************/

class QWidget;

/* External Subroutines */

extern void     CDocDatasetDisplay
                 (QWidget *, const std::string &, const std::string &);
extern int      CDocAddHelp
                 (const std::string &, const std::string &, const std::string &);
extern int      CDocAddTextHelp
                 (const std::string &, const std::string &, const std::string &);
extern int      CDocAddFormattedHelp
                 (const std::string &, const std::string &, const std::string &,
                  const std::string &, const std::string &);
extern int      CDocAddFormattedTextHelp
                 (const std::string &, const std::string &, const std::string &,
                  const std::string &, const std::string &);
extern int      CDocAddHelpSection
                 (const std::string &, int, const std::string &);
extern void     CDocAddWidgetKeyboardHelps
                 (const std::string &, QWidget *, int *, int);
extern QWidget *CDocCreateHelpButton
                 (const std::string &, QWidget *, void *, int, int *, int);
extern void     CDocAddHelpsToButton
                 (const std::string &, QWidget *, int *, int);
extern void     CDocDisplayHelpsForWidget
                 (const std::string &, QWidget *, int *, int);
extern void     CDocSetPopupOnDisplay
                 (int);
extern void     CDocSetPrintOnDisplay
                 (int);
extern int      CDocGetNoHelpPanels
                 ();

/*****************************************************************************/

#endif
