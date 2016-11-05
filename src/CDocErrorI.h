#ifndef CDOC_ERROR_I_H
#define CDOC_ERROR_I_H

/*****************************************************************************/

/* Includes */

#include <CDocError.h>

/*****************************************************************************/

namespace CDocError {
  void error(const std::string &title, const std::string &msg);
  void warn (const std::string &title, const std::string &msg);
  void info (const std::string &title, const std::string &msg);
};

#endif
