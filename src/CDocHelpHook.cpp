#include "CDocI.h"

/* Internal Typedefs and Structures */

struct CDHookProcData {
  char        *id;
  CDHookProc   proc;
};

/* Internal Variables */

typedef std::map<std::string,CDHookProcData *> CDHookProcList;

static CDHookProcList hook_proc_list;

// Specify a routine to be called when a hook of the the specified Identifier
// is selected in a CDoc document.
extern void
CDocHelpAddHookProc(char *id, CDHookProc proc)
{
  CDHookProcData  *hook_proc;

  hook_proc = new CDHookProcData;

  hook_proc->id   = CStrUtil::strdup(id);
  hook_proc->proc = proc;

  hook_proc_list[hook_proc->id] = hook_proc;
}

// Get the text associated with the specified Hook.
extern std::string
CDocScriptGetHookText(CDHookData *hook_data)
{
  int len = 0;

  if (CDocInst->getOutputFormat() == CDOC_OUTPUT_CDOC) {
    len += 16;

    len += hook_data->getId  ().size();
    len += hook_data->getData().size();
  }

  len += hook_data->getText().size();

  std::string str;

  if (CDocInst->getOutputFormat() == CDOC_OUTPUT_CDOC)
    CStrUtil::sprintf(str, "%sB%s\t%s%c%s%sE",
                      ESCH, hook_data->getId().c_str(), hook_data->getData().c_str(), ESC,
                      hook_data->getText().c_str(), ESCH);
  else
    str = hook_data->getText();

  return str;
}

// Process the Hook selected in the CDoc document and call its associated
// Hook procedure.
//
// Any environment variables specified in the data will be replaced before
// the Hook procedure is called.
extern void
CDocHelpProcessHook(const std::string &id, const std::string &data)
{
  if (id == "system") {
    system(data.c_str());

    return;
  }

  CDHookProcData *hook_proc = hook_proc_list[id];

  if (hook_proc == NULL) {
    fprintf(stderr, "No hook procedure for '%s'\n", id.c_str());
    return;
  }

  char *data1 = CStrUtil::strdup(data.c_str());

  int i = 0;

  for (i = 0; data1[i] != '\0'; i++) {
    if      (data1[i] == '\\')
      i++;
    else if (data1[i] == '$' && data1[i + 1] != '\0' &&
             (isalpha(data1[i + 1]) || data1[i + 1] == '_')) {
      i++;

      char *env_name = CStrUtil::strdup(&data1[i]);

      int j = 0;

      while (env_name[j] != '\0' &&
             (isalnum(env_name[j]) || env_name[j] == '_'))
        j++;

      env_name[j] = '\0';

      i += j;

      char *env_value = getenv(env_name);

      delete env_name;

      if (env_value == NULL)
        goto fail;

      int env_len = strlen(env_value);
      int len1    = strlen(&data1[i]);

      char *data2 = new char [i + env_len + len1 + 1];

      strncpy( data2             , data1    , i      );
      strncpy(&data2[i          ], env_value, env_len);
      strncpy(&data2[i + env_len], &data1[i], len1   );

      data2[i + env_len + len1] = '\0';

      delete data1;

      data1 = data2;

      i += env_len - 1;
    }
  }

  if (hook_proc->proc != NULL)
    (*hook_proc->proc)(data1, id.c_str());

 fail:
  delete data1;
}
