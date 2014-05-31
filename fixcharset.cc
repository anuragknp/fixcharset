#include "ystring.h"
#include "fixcharset.h"

#define CMBOLDTAG  

//trim chars like SPACE , ; ' " . % = \_
void TrimCharset(wyString *charset) {
  yInt32 start = 0;
  yInt32 end = charset->GetLen() - 1;
#define IS_TRIM_CHAR(c) (isspace(c) || c == '\\' || c == ',' || c == ';' || c == '\'' || c == '"' || c == '%' || c == '.' || c == '_' || c == '=')

  for (; start < charset->GetLen(); start++) {
    if (!IS_TRIM_CHAR(charset->GetCharAt(start))) {
      break;
    }
  }
  for (; end >= 0; end--) {
    if (!IS_TRIM_CHAR(charset->GetCharAt(end))) {
      break;
    }
  }

  if (end-start > 0 && end-start != charset->GetLen()-1) {
    wyString temp;
    temp.Set(charset->GetString() +  start, end-start+1);
    charset->Set(&temp);
  }
}

yBool IsCharsetValidUnderUTF8(wyString *charset) {
  yChar *charset_list[] = {"utf=8", "ansi_x3.110-1983", "printable-ascii", "646", "ascii", "ISO646-US", "iso-utf-8", "us-ascii", "X-", "utf-8", "utf8", "UNKNOWN", "unicode", NULL};

  for (yInt32 i = 0; charset_list[i] != NULL; i++) {
    if (charset->StrNICmp(charset_list[i], strlen(charset_list[i])) == 0) {
      return TRUE;
    }
  }
  if (charset->GetLen() == 3 && charset->StrNICmp("utf", 3) == 0) {
    return TRUE;
  }
  return FALSE;
}

void FixCharset(wyString *charset, yBool *is_utf8) {
  yInt32 len = 0;
  //Trim the charset
  TrimCharset(charset);
  //Some charset are like 'charset=XXXXX'
  if (charset->StrNICmp("charset", strlen("charset")) == 0) {
    charset->StripLChar((yInt32) strlen("charset"));
  }
  TrimCharset(charset);

  //Check for avoid list
  if (IsCharsetValidUnderUTF8(charset) == TRUE) {
    *is_utf8 = TRUE;
    charset->Set("utf-8");
    return;
  } else {
    //Fix wrong charset name
    yChar *wrong_charset_map[][2] = {{"8859_1", "iso-8859-1"},
                                  {"8859-1", "iso-8859-1"},
                                  {"iso-8559-1", "iso-8859-1"},
                                  {"iso-8859", "iso-8859-1"},
                                  {"iso8859_1", "iso-8859-1"},
                                  {"iso-8859-8-i", "iso-8859-8"},
                                  {"iso", "iso-8859-1"},
                                  {"cp-850", "cp850"},
                                  {"latin-1", "iso-8859-1"},
                                  {"iso-8859-x", "iso-8859-1"},
                                  {"iso-2022-jp$esc", "iso-2022-jp"},
                                  {"SJIS", "SHIFT_JIS"},
                                  {"WE8MSWIN1252", "cp1252"},
                                  {"WE8ISO8859P1", "cp1252"},
                                  {"Western European(ISO)", "iso-8859-1"},
                                  {"Western European", "iso-8859-1"},
                                  {"ANSI", "CP1252"},
                                  {"WIN-1252", "CP1252"},
                                  {"so-ir-100", "iso-8859-1"},
                                  {"csISOLatin1", "iso-8859-1"},
                                  {"latin1", "iso-8859-1"},
                                  {"l1", "iso-8859-1"},
                                  {"ibm819", "iso-8859-1"},
                                  {"CP819", "iso-8859-1"},
                                  {"EUC-JP-LINUX", "EUC-JP"},
                                  {"ISO 8859-1 ", "iso-8859-1"},
                                  {"ISO: Western", "iso-8859-1"},
                                  {"euc_kr", "euc-kr"},
                                  {"big5hkscs", "Big5-HKSCS"},
                                  {"Big5_HKSCS", "Big5-HKSCS"},
                                  {"csBig5", "Big5"},
                                  {"big-5", "Big5"},          
                                  {NULL, NULL}};

    for (yInt32 i = 0;wrong_charset_map[i][1] != NULL; i++) {
      if (charset->IsEqual(wrong_charset_map[i][0]) == TRUE) {
        charset->Set(wrong_charset_map[i][1]);
        return;
      }
    }
    //look for ascii and return utf-8
    if (charset->Find("ascii") != -1) {
	  *is_utf8 = TRUE;
	  charset->Set("utf-8");
	  return;
	}
    //Starts with "iso-8859-", if number of chars > 11,
    //if 10th char is int or not, if not, break it there. otherwise break it in 11th.
    if (charset->FindI("iso-8859-") == 0 && charset->GetLen() > 10) {
      yInt32 len1 = strlen("iso-8859-");
      if ((yInt32)charset->GetCharAt(len1 + 1) > 48 && (yInt32)charset->GetCharAt(len1 + 1) < 58) {
        charset->SetCharAt(len1+2, 0);
      } else {
        charset->SetCharAt(len1+1, 0);
      }
      return;
    }
    //Starts with windows-1251 => windows-1251
    len = strlen("windows-1251");
    if (charset->StrNICmp("windows-1251", len) == 0 && len < charset->GetLen()) {
      charset->SetCharAt(len, 0);
      return;
    }
    //Starts with windows-1252 => windows-1252
    len = strlen("windows-1252");
    if (charset->StrNICmp("windows-1252", len) == 0 && len < charset->GetLen()) {
      charset->SetCharAt(len, 0);
      return;
    }
  }
}
