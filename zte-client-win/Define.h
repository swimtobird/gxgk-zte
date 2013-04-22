#define VER_Major    1
#define VER_Minor    0
#define VER_Release    273   
#define VER_Build    769  
#define VER_Version    MAKELONG(MAKEWORD(VER_Major, VER_Minor), VER_Release)

#define _Stringizing(v)    #v
#define _VerJoin(a, b, c, d)  _Stringizing(a.b.c.d)

#define STR_BuildDate    TEXT(__DATE__)
#define STR_BuildTime    TEXT(__TIME__)
#define STR_BuilDateTime  TEXT(__DATE__) TEXT(" ") TEXT(__TIME__)
#define STR_Version    TEXT(_VerJoin(VER_Major, VER_Minor, VER_Release, VER_Build))


#define MAX_STRING 2048
#define CONFIGNAME TEXT("zte.conf")
//#define CONFIGNAME TEXT("ZWZZte.conf")


#define STR_AppName    TEXT("广科校园网认证客户端")
#define STR_Author    TEXT("YZX")
#define STR_Corporation    TEXT("莞香广科")

//#define STR_AppName    TEXT("ZWZ's ZTE认证客户端")
//#define STR_Author    TEXT("ZWZ")
//#define STR_Corporation    TEXT("ZWZ's Studio")

#define STR_WEB      TEXT("bbs.gxgk.cc/")
#define STR_WEB_URL    TEXT("http://") STR_WEB
#define STR_AUTHOR_URL	STR_WEB_URL TEXT("space-uid-207.html")
//#define STR_EmailUrl    TEXT("mailto:") STR_Email TEXT("?Subject=ZWZZte")

#ifdef _CHS
#define STR_Description    STR_AppName
#define STR_Copyright    TEXT("版权没有 (C) 2009\0")
#else // _CHS
#define STR_Description    TEXT("GDST's ZTE")
#define STR_Copyright    TEXT("CopyLeft (C) 2009\0")
#endif // _CHS















