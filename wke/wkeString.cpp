//////////////////////////////////////////////////////////////////////////


#include <WebCore/config.h>
#include <wtf/text/CString.h>

//cexer: ��������ں��棬��Ϊ���е� windows.h �ᶨ�� max��min������ WebCore �ڲ��� max��min ���ִ��ҡ�
#include "wkeString.h"


//////////////////////////////////////////////////////////////////////////



namespace wke
{



CString::CString(const WTF::String& str)
    : m_string(str)
    , m_utf8(NULL)
    , m_wide(NULL)
{

}

CString::CString(const utf8* str, size_t len /*= 0*/)
    : m_utf8(NULL)
    , m_wide(NULL)
{
    m_string = WTF::String::fromUTF8(str, len);
}

CString::CString(const wchar_t* str, size_t len /*= 0*/)
    : m_utf8(NULL)
    , m_wide(NULL)
{
    WTF::String(str, len).swap(m_string);
}

CString::~CString()
{
    _free();
}

CString& CString::operator=(const WTF::String& str)
{
    if (&m_string != &str)
    {
        _dirty();
        m_string = str;
    }
    return *this;
}

CString& CString::operator=(const CString& str)
{
    return operator=(str.m_string);
}

const utf8* CString::string() const
{
    if (!m_utf8)
    {
        WTF::CString wtfUtf8 = m_string.utf8();
        size_t wtfUtf8Len = wtfUtf8.length();

        m_utf8 = new utf8[wtfUtf8Len + 1];
        if (wtfUtf8Len != 0)
            memcpy(m_utf8, wtfUtf8.data(), wtfUtf8Len);

        m_utf8[wtfUtf8Len] = 0;
    }

    return m_utf8;
}

const wchar_t* CString::stringW() const
{
    if (!m_wide)
    {
        const wchar_t* wtfWide = m_string.characters();
        size_t wtfWideLen = m_string.length();

        m_wide = new wchar_t[wtfWideLen + 1];
        if (wtfWideLen != 0)
            memcpy(m_wide, wtfWide, wtfWideLen * sizeof(wchar_t));

        m_wide[wtfWideLen] = 0;
    }

    return m_wide;
}

const WTF::String& CString::original() const
{
    return m_string;
}

void CString::setString(const utf8* str, size_t len /*= 0*/)
{
    _dirty();
    m_string = WTF::String::fromUTF8(str, len);
}

void CString::setString(const wchar_t* str, size_t len /*= 0*/)
{
    _dirty();
    WTF::String(str, len).swap(m_string);
}

void CString::_dirty()
{
    _free();
}

void CString::_free()
{
    if (m_wide)
    {
        delete [] m_wide;
        m_wide = NULL;
    }

    if (m_utf8)
    {
        delete [] m_utf8;
        m_utf8 = NULL;
    }
}





};

