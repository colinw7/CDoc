TEMPLATE = app

QT += widgets

TARGET = CDoc

DEPENDPATH += .

QMAKE_CXXFLAGS += -std=c++11

MOC_DIR = .moc

CONFIG += debug

# Input
SOURCES += \
CDocControl.cpp \
CDocError.cpp \
CDocHelpCommandPanel.cpp \
CDocHelp.cpp \
CDocHelpDatasetData.cpp \
CDocHelpHook.cpp \
CDocHelpListAll.cpp \
CDocHelpListWidget.cpp \
CDocHelpPagePanel.cpp \
CDocHelpPanel.cpp \
CDocHelpPrintPanel.cpp \
CDocHelpSearchPanel.cpp \
CDocHelpSectionPanel.cpp \
CDocMain.cpp \
CDocScriptAmendments.cpp \
CDocScriptDataset.cpp \
CDocScriptDevice.cpp \
CDocScriptError.cpp \
CDocScriptExample.cpp \
CDocScriptFigure.cpp \
CDocScriptFont.cpp \
CDocScriptFootnote.cpp \
CDocScriptFrontSheet.cpp \
CDocScriptIndex.cpp \
CDocScriptLabel.cpp \
CDocScriptLine.cpp \
CDocScriptList.cpp \
CDocScriptMacro.cpp \
CDocScriptMemo.cpp \
CDocScriptOptions.cpp \
CDocScriptPage.cpp \
CDocScriptParagraph.cpp \
CDocScriptPCtrl.cpp \
CDocScriptProcess.cpp \
CDocScriptReference.cpp \
CDocScriptRevCtrl.cpp \
CDocScriptSymbol.cpp \
CDocScriptTable.cpp \
CDocScriptTempFile.cpp \
CDocScriptTitlePage.cpp \
CDocScriptTOC.cpp \
CDocScriptTranslate.cpp \
CDocScriptWrite.cpp \
CDocString.cpp \
\
CFontMetrics.cpp \
CFreeType.cpp \
CImagePixelRenderer.cpp \
CPixelClip.cpp \
CPixelRenderer.cpp \
CPrint.cpp \
CQImageButton.cpp \
CQPixelRenderer.cpp \
CQWidgetPixelRenderer.cpp \
CSpellCtrl.cpp \
CSpellGood.cpp \
CSpellHash.cpp \
CSpellLookup.cpp \
CSpellTree.cpp \
CSymbol2D.cpp \

HEADERS += \
CDocControlI.h \
CDocErrorI.h \
CDocHelpCommandPanelI.h \
CDocHelpDatasetDataI.h \
CDocHelpHookI.h \
CDocHelpI.h \
CDocHelpListAllI.h \
CDocHelpListWidgetI.h \
CDocHelpPagePanelI.h \
CDocHelpPanelI.h \
CDocHelpPrintPanelI.h \
CDocHelpSearchPanelI.h \
CDocHelpSectionPanelI.h \
CDocI.h \
CDocMain.h \
CDocScriptAmendmentsI.h \
CDocScriptDatasetI.h \
CDocScriptDeviceI.h \
CDocScriptErrorI.h \
CDocScriptExampleI.h \
CDocScriptFigureI.h \
CDocScriptFontI.h \
CDocScriptFootnoteI.h \
CDocScriptFrontSheetI.h \
CDocScriptIndexI.h \
CDocScriptLabelI.h \
CDocScriptLineI.h \
CDocScriptListI.h \
CDocScriptMacroI.h \
CDocScriptMemoI.h \
CDocScriptOptionsI.h \
CDocScriptPageI.h \
CDocScriptParagraphI.h \
CDocScriptPCtrlI.h \
CDocScriptProcessI.h \
CDocScriptReferenceI.h \
CDocScriptRevCtrlI.h \
CDocScriptSymbolI.h \
CDocScriptTableI.h \
CDocScriptTempFileI.h \
CDocScriptTitlePageI.h \
CDocScriptTocI.h \
CDocScriptTranslateI.h \
CDocScriptWriteI.h \
CDocStringI.h \
\
CBrush.h \
CBrushStyle.h \
CFontMetrics.h \
CImagePixelRenderer.h \
CPen.h \
CPixelClip.h \
CPixelRenderer.h \
CPrint.h \
CQImageButton.h \
CQPixelRenderer.h \
CQWidgetPixelRenderer.h \
CSpellCtrl.h \
CSpellGenI.h \
CSpellGood.h \
CSpell.h \
CSpellHash.h \
CSpellI.h \
CSpellLookup.h \
CSpellTree.h \
CSymbol2D.h \
std_tmpl++.h \

DESTDIR     = ../bin
OBJECTS_DIR = ../obj
LIB_DIR     = ../lib

INCLUDEPATH += \
. \
Expr \
../include \
../../CQUtil/include \
../../CFont/include \
../../CImageLib/include \
../../CConfig/include \
../../CFile/include \
../../COS/include \
../../CUtil/include \
../../CMath/include \
../../CStrUtil/include \
../../CRGBName/include \
/usr/include/freetype2 \

unix:LIBS += \
-L$$LIB_DIR \
-L../../CQUtil/lib \
-L../../CFont/lib \
-L../../CConfig/lib \
-L../../CUtil/lib \
-L../../CImageLib/lib \
-L../../CFile/lib \
-L../../CFileUtil/lib \
-L../../COS/lib \
-L../../CStrUtil/lib \
-L../../CRegExp/lib \
-L../../CRGBName/lib \
-lCQUtil -lCUtil -lCFont -lCConfig -lCImageLib \
-lCRGBName -lCFile -lCFileUtil -lCUtil -lCOS -lCStrUtil -lCRegExp \
-lpng -ljpeg -ltre -lfreetype
