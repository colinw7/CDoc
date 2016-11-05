#include "CDocI.h"

// Write specified Error Message (in printf format) to Standard Error in CDoc format.
extern void
CDocWriteError(const char *format, ...)
{
  va_list vargs;

  va_start(vargs, format);

  std::string error_string;

  CStrUtil::vsprintf(error_string, format, &vargs);

  va_end(vargs);

  fprintf(stderr, "CDoc Error - %s\n", error_string.c_str());
}

void
CDocError::
error(const std::string &title, const std::string &msg)
{
  CDocWriteError("Error: %s : %s", title.c_str(), msg.c_str());
}

void
CDocError::
warn(const std::string &title, const std::string &msg)
{
  CDocWriteError("Warn: %s : %s", title.c_str(), msg.c_str());
}

void
CDocError::
info(const std::string &title, const std::string &msg)
{
  CDocWriteError("Info: %s : %s", title.c_str(), msg.c_str());
}
