typedef void (*CDHookProc)
                (const char *, const char *);

extern void CDocHelpAddHookProc
             (char *, CDHookProc);
