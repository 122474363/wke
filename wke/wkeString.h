#ifndef WKE_STRING_H
#define WKE_STRING_H


#include <WebCore/config.h>
#include <wtf/text/WTFString.h>

//cexer: ��������ں��棬��Ϊ���е� windows.h �ᶨ�� max��min������ WebCore �ڲ��� max��min ���ִ��ҡ�
#include "wke.h"

//////////////////////////////////////////////////////////////////////////



namespace wke
{





class CString
{
public:
    CString(const WTF::String& str);
    CString(const utf8* str, size_t len = 0);
    CString(const wchar_t* str, size_t len = 0);
   ~CString();

public:
    const utf8* string() const;
    const wchar_t* stringW() const;
    
    void setString(const utf8* str, size_t len = 0);
    void setString(const wchar_t* str, size_t len = 0);

protected:
    void _free();
    void _dirty();

    WTF::String m_string;
    mutable utf8* m_utf8;
    mutable wchar_t* m_wide;
};





};


#endif//WKE_STRING_H