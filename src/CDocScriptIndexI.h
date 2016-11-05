#ifndef CDOC_SCRIPT_INDEX_I_H
#define CDOC_SCRIPT_INDEX_I_H

#define PG_START ((char *) 1)
#define PG_END   ((char *) 2)
#define PG_MAJOR ((char *) 3)

#define PG_START_STR "__pg_start__"
#define PG_END_STR   "__pg_end__"
#define PG_MAJOR_STR "__pg_major__"

extern void        CDocScriptOutputIndex
                    ();
extern void        CDocScriptAddIndex1
                    (const std::string &, const std::string &, const std::string &);
extern void        CDocScriptAddIndexHeader1
                    (const std::string &, const std::string &, const std::string &,
                     const std::string &, const std::string &);
extern void        CDocScriptAddIndex2
                    (const std::string &, const std::string &, const std::string &);
extern void        CDocScriptAddIndexHeader2
                    (const std::string &, const std::string &, const std::string &,
                     const std::string &, const std::string &);
extern void        CDocScriptAddIndex3
                    (const std::string &, const std::string &, const std::string &);
extern void        CDocScriptAddIndexHeader3
                    (const std::string &, const std::string &, const std::string &,
                     const std::string &, const std::string &);
extern void        CDocScriptAddIndexRef
                    (const std::string &, const std::string &, const std::string &,
                     const std::string &);
extern std::string CDocScriptGetIndex1SeeText
                    (const std::string &, const std::string &);
extern std::string CDocScriptGetIndex2SeeText
                    (const std::string &, const std::string &);
extern std::string CDocScriptGetIndex3SeeText
                    (const std::string &, const std::string &);
extern void        CDocScriptSetIndex1Page
                    (const std::string &, const std::string &, const std::string &);
extern void        CDocScriptSetIndex2Page
                    (const std::string &, const std::string &, const std::string &);
extern void        CDocScriptSetIndex3Page
                    (const std::string &, const std::string &, const std::string &);
extern std::string CDocScriptGetIndex1Page
                    (int);
extern std::string CDocScriptGetIndex2Page
                    (int, int);
extern std::string CDocScriptGetIndex3Page
                    (int, int, int);
extern void        CDocScriptDeleteIndex
                    ();

#endif
