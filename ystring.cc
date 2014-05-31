/**************************************************************
File			: ystring.cpp
Description		: implements an advanced string class 
Date Created	:
Created By		:
Last Modified	: Manoj
Last Reviewed	:
 **************************************************************/

#include "yinclude.h"
#include "ystring.h"
//#include "ylog.h"

const size_t wyString::vAllBitsOne = (size_t) (-1); // patch to remove the warning generated by 
// the assignment of -1 to size_t variables
#define NOT_ENOUGH_MEMORY "Not enough memory, application terminated!"
#ifndef va_copy // is not C99 std
#define va_copy(dst, src) ((void)((dst) = (src))) 
#endif

void wyString::Set(yCChar * buf, size_t len) {
  SetEmpty();
  Add(buf, len);
}




void wyString::Set(wyString* src) {
  SetEmpty();
  this->Set(src->GetString(), src->GetLen());
}

#ifdef CMANDROID
#include <locale.h>
#else
#include <xlocale.h>
#endif

void wyString::Set(const yWChar * src) {
  yChar* utf8 = NULL;
  yInt32 lengthofutf8 = 0;

  if (wcslen(src) == 0) {
    Set("");
    return;
  }

#ifdef _WIN32
  yInt32 length = WideCharToMultiByte(CP_UTF8, 0, src, -1, NULL, 0, NULL, NULL);
#else
  //set codepage to UTF-8
    setlocale(LC_ALL, "en_US.UTF-8");
    yInt32 length = wcstombs(NULL, src, 0);
#endif

  utf8 = (yChar *) calloc(sizeof (yChar), length + 1);

  if (!utf8) {
#ifndef SQLITE_MAILING_CODE
//    LOG_CRITICAL_DATA(-1, NOT_ENOUGH_MEMORY);
    //Adding abort to generate core dump
    abort();
#endif
  }

#ifdef _WIN32
  lengthofutf8 = WideCharToMultiByte(CP_UTF8, 0, src, -1, utf8, length, NULL, NULL);
#else
  lengthofutf8 = wcstombs(utf8, src, length);
#endif
setlocale(LC_ALL, "");
  //Set(utf8, (yUInt32)lengthofutf8);
  Set(utf8);
  free(utf8);

  return;
}

//http://stackoverflow.com/questions/1031645/how-to-detect-utf-8-in-plain-c
yBool wyString::IsValidUtf8Buffer(yCByte *bytes, size_t len) {
  while (*bytes) {
    if ((// ASCII
      bytes[0] == 0x09 ||
      bytes[0] == 0x0A ||
      bytes[0] == 0x0D ||
      (0x20 <= bytes[0] && bytes[0] <= 0x7E)
      )
      ) {
      bytes += 1;
      continue;
    }

    if ((// non-overlong 2-byte
      (0xC2 <= bytes[0] && bytes[0] <= 0xDF) &&
      (0x80 <= bytes[1] && bytes[1] <= 0xBF)
      )
      ) {
      bytes += 2;
      continue;
    }
    if ((// excluding overlongs
      bytes[0] == 0xE0 &&
      (0xA0 <= bytes[1] && bytes[1] <= 0xBF) &&
      (0x80 <= bytes[2] && bytes[2] <= 0xBF)
      ) ||
      (// straight 3-byte
      ((0xE1 <= bytes[0] && bytes[0] <= 0xEC) ||
      bytes[0] == 0xEE ||
      bytes[0] == 0xEF) &&
      (0x80 <= bytes[1] && bytes[1] <= 0xBF) &&
      (0x80 <= bytes[2] && bytes[2] <= 0xBF)
      ) ||
      (// excluding surrogates
      bytes[0] == 0xED &&
      (0x80 <= bytes[1] && bytes[1] <= 0x9F) &&
      (0x80 <= bytes[2] && bytes[2] <= 0xBF)
      )
      ) {
      bytes += 3;
      continue;
    }
    if ((// planes 1-3
      bytes[0] == 0xF0 &&
      (0x90 <= bytes[1] && bytes[1] <= 0xBF) &&
      (0x80 <= bytes[2] && bytes[2] <= 0xBF) &&
      (0x80 <= bytes[3] && bytes[3] <= 0xBF)
      ) ||
      (// planes 4-15
      (0xF1 <= bytes[0] && bytes[0] <= 0xF3) &&
      (0x80 <= bytes[1] && bytes[1] <= 0xBF) &&
      (0x80 <= bytes[2] && bytes[2] <= 0xBF) &&
      (0x80 <= bytes[3] && bytes[3] <= 0xBF)
      ) ||
      (// plane 16
      bytes[0] == 0xF4 &&
      (0x80 <= bytes[1] && bytes[1] <= 0x8F) &&
      (0x80 <= bytes[2] && bytes[2] <= 0xBF) &&
      (0x80 <= bytes[3] && bytes[3] <= 0xBF)
      )
      ) {
      bytes += 4;
      continue;
    }
    return FALSE;
  }
  return TRUE;
}

yBool wyString::IsValidUtf8Buffer() {
  return IsValidUtf8Buffer((yCByte*) GetString(), GetLen());
}

void wyString::Add(const yWChar * src) {
  yChar* utf8 = NULL;
  yInt32 lengthofutf8 = 0;

  if (wcslen(src) == 0) {
    Set("");
    return;
  }

#ifdef _WIN32
  yInt32 length = WideCharToMultiByte(CP_UTF8, 0, src, -1, NULL, 0, NULL, NULL);
#else
  //set codepage to UTF-8
  setlocale(LC_ALL, "en_US.utf-8");
  yInt32 length = wcstombs(NULL, src, 0);
#endif

  utf8 = (yChar *) calloc(sizeof (yChar), length + 1);

  if (!utf8) {
#ifndef SQLITE_MAILING_CODE
//    LOG_CRITICAL_DATA(-1, NOT_ENOUGH_MEMORY);
    //Adding abort to generate core dump
    abort();
#endif
  }

#ifdef _WIN32
  lengthofutf8 = WideCharToMultiByte(CP_UTF8, 0, src, -1, utf8, length, NULL, NULL);
#else
  lengthofutf8 = wcstombs(utf8, src, length);
#endif

  Add(utf8);
  free(utf8);

  return;
}

// if Apple and us-ascii, remove the unwanted char 
// wcstombc() is returning invalid in the corrupted ascii char case

void wyString::FixAscii() {
  yInt32 index;
  yInt32 len = GetLen();
  yChar ch;

  for (index = 0; index < len; index++) {
    ch = data_[index];
    if (ch < 0) {
      data_[index] = ' ';
    }
  }
}

// adds a buffer to the existing buffer
// Parameters
//      buf: buffer to add
//      length: buffer len to add
// Returns 
//      void

void wyString::Add(yCChar * buf, size_t length) {
  size_t len;
  size_t required;
  yChar* prev;

  len = (length == vAllBitsOne) ? strlen(buf) : length;

  required = (len_ + len) * sizeof (yChar) + sizeof (yChar);

  if (required > buf_size_) {
    buf_size_ = required * 2;
    prev = data_;
    data_ = (yChar*) calloc(buf_size_, 1);
    if (!data_) {
#ifndef SQLITE_MAILING_CODE
//      LOG_CRITICAL_DATA(-1, NOT_ENOUGH_MEMORY);
#endif
      assert(0);
    }
    // copy existing stuff
    memcpy(data_, prev, len_ * sizeof (yChar));
    // delete prev buf
    if (prev) free(prev);
  }

  // copy new stuff
  memcpy(data_ + len_, buf, len * sizeof (yChar));
  // set the correct len
  len_ += len;
  data_[len_] = 0;
}

// adds a string object to the existing buffer
// Parameters
//      pStr: string object to add
// Returns 
//      void

void wyString::Add(wyString* str) {
  Add(str->GetString(), str->GetLen());
}

// Sets the buffer, here the parameter supports formatstring
// Parameters
//      format_str: format string
//      valist: argument list
//  Returns 
//      void

void wyString::vSprintf(yCChar * format_str, va_list valist) {
  size_t remaining;
  yInt32 count = 0;
  va_list vacopy;
  size_t len;

  if (!buf_size_) {
    VERIFY_VAL(data_, NULL);
    len = strlen(format_str);
    buf_size_ = len * sizeof (yChar) + sizeof (yChar);
    data_ = (yChar*) calloc(buf_size_, 1);
    if (!data_) {
#ifndef SQLITE_MAILING_CODE
//      LOG_CRITICAL_DATA(-1, NOT_ENOUGH_MEMORY);
#endif
      assert(0);
    }
  }

  // how much can be added?
  while (1) {
    va_copy(vacopy, valist);
    // how much buf is available?
    remaining = buf_size_ - len_ * sizeof (yChar);
    remaining /= sizeof (yChar);
    // try writing
    count = wyString::VsnPrintf(data_ + len_, remaining, format_str, vacopy);
    va_end(vacopy);
    if ((count == -1) || (count >= remaining)) {
      yChar* prev;
      buf_size_ *= 2; // double the bet!
      prev = data_;
      data_ = (yChar*) calloc(buf_size_, 1);
      if (!data_) {
#ifndef SQLITE_MAILING_CODE
//        LOG_CRITICAL_DATA(-1, NOT_ENOUGH_MEMORY);
#endif
        assert(0);
      }
      // copy existing stuff // not required!
      memcpy(data_, prev, len_ * sizeof (yChar));
      // delete prev buf
      free(prev);
    } else {
      len_ += count;
      break;
    }
  }
}

// Adds the buffer, here the parameter supports formatstring 
// Parameters
//      format_str: format string
//  Returns 
//      void

void wyString::AddSprintf(yCChar * format_str, ...) {
  va_list va;
  va_start(va, format_str);
  vSprintf(format_str, va);
  va_end(va);

}

// Sets the buffer, here the parameter supports formatstring
// Parameters
//      format_str: format string
//  Returns 
//      void

void wyString::Sprintf(yCChar * format_str, ...) {
  va_list va;

  va_start(va, format_str);
  SetEmpty();
  vSprintf(format_str, va);
  va_end(va);
}

// Copies a string of specidfied length from a string object
// Parameters
//      pStr: the source string object
//      pMax: length
// Returns 
//      void

void wyString::StrNCpy(wyString* str, size_t max) {
  SetEmpty();
  Set(str->GetString(), max);
}

void wyString::StrNCpy(yChar* str, size_t max) {
  if (str == NULL)
    return;

  SetEmpty();
  Set(str, max);
}

// Duplicates a string
// Returns 
//      a new duplicated string object

wyString * wyString::Dup() {
  wyString* newstr = new wyString;
  newstr->Set(this->GetString(), this->GetLen());
  return newstr;
}

// strips up to a token(including token)
// Parameters
//      pToken: token to find and strip
//      new_string: the string upto token[OUT]
// Returns
//      TRUE if not empty, FALSE otherwise

yBool wyString::StripToken(yChar * token, wyString * new_string) {
  yChar* found;
  size_t tokenlen = strlen(token);
  size_t firstpartlen;
  size_t secondpartlen;

  new_string->SetEmpty();
  if (this->IsEmpty()) return FALSE;

  found = strstr(data_, token);
  if (found) {
    // copy everything from the left of the token to new_string
    firstpartlen = found - data_;
    secondpartlen = len_ - (firstpartlen + tokenlen);

    new_string->StrNCpy(this, firstpartlen);

    // remove the extracted string and the token from the current string			
    memmove(data_, data_ + (firstpartlen + tokenlen), secondpartlen * sizeof (yChar) + sizeof (yChar));

    // set the new size
    len_ = secondpartlen;

    return TRUE;

  } else {
    new_string->Set(this);
    // Make this empty
    this->SetEmpty();
    return TRUE;
  }
}

// skips the first white spaces and next token from the buffer
// Returns 
//      TRUE always

yBool wyString::SkipToken() {
  StripLWhiteSpace(); // strip white spaces

  if (!buf_size_) return TRUE;

  while (TRUE) { // strip next token
#ifndef __LINUX__
    //Sayan: On Linux this condition is always true and
    // results in a compiler warning, while on Windows
    // if this check is not performed it may cause an overflow
    if (*data_ > 0 && *data_ <= 255) {
#endif /* __LINUX__ */
      if (!isspace(*data_) && *data_)
        StripLChar(*data_);
      else
        break;
#ifndef __LINUX__
    } else
      break;
#endif /* __LINUX__ */
  }

  return TRUE;
}

// function to convert Hex value to char
//  Parameters
//      pHex: hexadecimal value
//  Returns 
//      Char value

yChar hex2char(yChar * hex) {

  yChar digit;

  digit = (hex[0] >= 'A' ? ((hex[0] & 0xdf) - 'A') + 10 : (hex[0] - '0'));
  digit *= 16;
  digit += (hex[1] >= 'A' ? ((hex[1] & 0xdf) - 'A') + 10 : (hex[1] - '0'));

  return (digit);

}

//  Helps to unescape a url
//  Returns 
//      void

void wyString::UnEscapeURL() {
  size_t i, j;

  if (IsEmpty()) return;

  for (i = 0, j = 0; data_[j] != 0 && j < len_; ++i, ++j) {
    data_[i] = data_[j];

    if (data_[i] == '+') // + is a valid uri char
      data_[i] = ' ';
    else if (data_[i] == '%') {
      data_[i] = hex2char(&data_[j + 1]);
      j += 2;
    }
  }
  data_[i] = 0;
  // reset the length
  len_ = strlen(data_);
  return;

}


// fills with pStr bufffer pRepeat times
// Parametrs
//      pStr: bufffer to fill
//      prepeat: repeat count
// Returns 
//      Void

void wyString::Fill(yChar* str, yUInt32 repeat) {
  size_t required;
  size_t len = strlen(str);

  SetEmpty();
  required = ((len * repeat) * sizeof (yChar)) + sizeof (yChar);
  if (required > buf_size_) {
    free(data_);
    buf_size_ = required;

    data_ = (yChar*) calloc(buf_size_, 1);
    if (!data_) {
#ifndef SQLITE_MAILING_CODE
//      LOG_CRITICAL_DATA(-1, NOT_ENOUGH_MEMORY);
#endif
      assert(0);
    }
  }
  for (size_t i = 0; i < repeat; i++)
    memcpy(data_ + (i * len), str, len * sizeof (yChar));

  len_ += (len * repeat);
  data_[len_] = 0;

}

// removes pChar(single/consecutive occurence) from the left of the buffer, 
// Parameters
//      pChar : Char to remove
// Returns
//      void

void wyString::StripLChar(yChar character) {
  if (buf_size_ == 0) return;
  while (*data_ == character && len_ > 0) {
    len_--;

    if (*(data_ + 1) != YZERO) {
      memmove(data_, data_ + 1, len_ * sizeof (yChar));
    } else {
      *data_ = 0;
      return;
    }
  }
  data_[len_] = 0;
}

void wyString::StripLChar(yInt32 size) {
  if (buf_size_ == 0 || size <= 0)
    return;

  if (size > len_) {
    size = len_;
  }

  len_ = len_ - size;
  memmove(data_, data_ + size, len_ * sizeof (yChar));
  data_[len_] = 0;
}

void wyString::ReplaceTabWithSpace() {
  yInt32 index = 0;

  if (buf_size_ == 0) return;
  for (; index < len_; index++) {
    if (*(data_ + index) == '\t')
      *(data_ + index) = ' ';
  }
}

// strips white spaces from both sides of the buffer

void wyString::StripWhiteSpace() {
  StripLWhiteSpace();
  StripRWhiteSpace();
}

// strips left white space

void wyString::StripLWhiteSpace() {
  yChar presentchar;

  if (buf_size_ == 0) return;
  while (TRUE) {
#ifndef __LINUX__
    //Sayan: On Linux this condition is always true and
    // results in a compiler warning, while on Windows
    // if this check is not performed it may cause an overflow
    if (*data_ > 0 && *data_ <= 255) {
#endif /* __LINUX__ */
      if (isspace(*data_)) {
        presentchar = *data_;
        StripLChar(presentchar);
      } else
        break;
#ifndef __LINUX__
    } else
      break;
#endif /* __LINUX__ */
  }
  return;
}

void wyString::TrimMatchingCharFromEnd(yCChar match) {
  if (len_ == 0) return;
  if (data_[len_ - 1] == match) {
    data_[len_ - 1] = '\0';
    len_--;
  }
}

// strips right white space

void wyString::StripRWhiteSpace() {
  if (len_ == 0) return;

  while (TRUE) {
#ifndef __LINUX__
    //Sayan: On Linux this condition is always true and
    // results in a compiler warning, while on Windows
    // if this check is not performed it may cause an overflow
    if (*(data_ + len_ - 1) > 0 && *(data_ + len_ - 1) <= 255) {
#endif /* __LINUX__ */
      if (isspace(*(data_ + len_ - 1))) {
        *(data_ + len_ - 1) = '\0';
        len_--;

        if (len_ == 0) break;
      } else
        break;
#ifndef __LINUX__
    } else
      break;
#endif /* __LINUX__ */
  }

  return;
}

// Helps to add a integer value to the buffer

void wyString::Add(yInt32 int_data) {
  yChar buf[20] = {0};
  //Removed for linux compilance, use sprintf instead	
  //_itoa(pIntData, buf, 10);
  sprintf(buf, "%d", int_data);
  Add(buf);
}

void wyString::Add(yULong int_data) {
  yChar buf[20] = {0};
  //Removed for linux compilance, use sprintf instead	
  //_itoa(pIntData, buf, 10);
  sprintf(buf, "%lu", int_data);
  Add(buf);
}

// Helps to add a integer value to the buffer

void wyString::Add(yInt64 int_data) {
  yChar buf[256] = {0};
  //Removed for linux compilance, use sprintf instead	
  //_itoa(pIntData, buf, 10);
#ifdef _WIN32
  sprintf(buf, "%I64d", int_data);
#elif __APPLE__
  // http://stackoverflow.com/questions/799884/how-do-i-force-64-bit-integer-arithmetic-on-os-x/799894
  sprintf(buf, "%lld", int_data);
#else
  sprintf(buf, "%Ld", int_data);
#endif // _win32
  Add(buf);
}
void wyString::AddUInt64(yUInt64 int_data) {
	yChar buf[256] = {0};

#ifdef _WIN32
  sprintf(buf, "%I64u", int_data);
#else
  sprintf(buf, "%llu", int_data);
#endif
  Add(buf);
}

// converts all buffer to lower case

void wyString::ToLower() {
  yChar * p = data_;
  if (data_ == NULL) {
    return;
  }
  do {
    *p = tolower(*p);

  } while (*p++);
}

void wyString::ToUpper() {
  yChar * p = data_;
  if (data_ == NULL) {
    return;
  }
  do {
    *p = toupper(*p);

  } while (*p++);
}

// finds a string with out considering the case
// Parameters
//      pToFind: string to find
// Returns 
//      position if found, -1 otherwise

size_t wyString::FindI(yCChar* to_find, yUInt32 pos) {
  yCChar* found = NULL;

  if (data_ == NULL) {
    return vAllBitsOne;
  }

  if (pos < len_) {
    found = wyString::StrIStr(data_ + pos, to_find);
  }

  if (!found) {
    return vAllBitsOne; // not found!		
  }

  return (found - data_); // return the string position

}

// finds a string with out considering the case
// Parameters
//      pToFind: string to find
// Returns 
//      position if found, -1 otherwise

size_t wyString::Find(yCChar * to_find) {
  if (data_ == NULL) {
    return vAllBitsOne;
  }

  yChar* found;
  found = strstr(data_, to_find);
  if (!found) {
    return vAllBitsOne; // not found!		
  }

  return (found - data_); // return the string position
}


size_t wyString::Find(yCChar * from_, yCChar* to_find) {
  if (from_ == NULL || to_find == NULL) {
    return vAllBitsOne;
  }

  yCChar* found;
  found = strstr(from_, to_find);
  if (!found) {
    return vAllBitsOne; // not found!		
  }
  return (found - from_); // return the string position
}

//write formatted output using a pointer to a list of arguments.

yInt32 wyString::VsnPrintf(yChar *str, size_t size,
        const yChar *format, va_list ap) {
#ifdef _WIN32
  return _vsnprintf(str, size, format, ap);
#else
  return vsnprintf(str, size, format, ap);
#endif //_WIN32
}

// Strips n characters from the end of the buffer
// Parameters
//      pSize: number of characters to strip
// Returns
//      the string length after stripping

yInt32 wyString::Strip(yInt32 size) {
  if (buf_size_ == 0)
    return 0;
  else if (size > len_) {
    len_ = 0;
    data_[0] = YNULCHAR;
  } else {
    data_[len_ - size] = YNULCHAR;
    len_ = len_ - size;
  }

  return len_;
}

//checks char is always safe for putting in a URL: 
// Parameters
//      pChar: char to check
// Returns 
//      1 if safe, 0 otherwise

yInt32 wyString::IsUrlSafe(yChar character) {
  // see url(7) for details
  // XXX don't touch the 0 (zero) at the end of either of these arrays! 
  // these are always safe for putting in a URL: 
  yChar urlsafe [] = {'-', '_', '.', '!', '~', '*', '\'', '(', ')', '\0'};
  yInt32 i;

  for (i = 0; urlsafe[i]; ++i) {
    if (character == urlsafe[i]) return 1;
  }

  return 0;
}

// checks whether the char is included in the reserved list
// Parameters
//      pChar:  char to check
// Returns 
//      1 if reserved, 0 otherwise

yInt32 wyString::IsUrlReservedSafe(yChar character) {
  //reserved characters can only be used in certain places:
  yChar urlreserved [] = {';', '/', '?', ':', '@', '&', '=', '+', '$', ',', '\0'};
  yInt32 i;

  for (i = 0; urlreserved[i]; ++i) {
    if (character == urlreserved[i]) return 1;
  }

  return 0;
}

// Helper function to URL encode the buffer
// Parameters
//      pFrom: source
//      pTo: result buffer[OUT]
//      pPreserveReserved: reserved characters can only be used in certain places
// Returns 
//      void

void wyString::UrlEncode(const yChar * from, yChar * to, yBool preserve_reserved) {
  while (*from) {
    if ((*from > 0 && *from < 0xFF &&  isalnum(*from)) || IsUrlSafe(*from) ||
            (preserve_reserved == TRUE && IsUrlReservedSafe(*from))) {
      *(to++) = *from;
    } else {
      sprintf(to, "%%%02x", (yByte) * from);
      to += 3;
    }
    from++;
  }
}

// helps to Escape a url
// Parameters
//      pPreserveReserved: reserved characters can only be used in certain places
// Returns 
//      void

void wyString::EscapeURL(yBool preserve_reserved) {
  yChar *ret = NULL, *to = NULL;

  if (!len_) // nothing to escape
    return;

  /* our new string is at _most_ 3 times longer */
  ret = (yChar*) calloc(1, (len_ * 3) + 1);
  if (!ret) {
#ifndef SQLITE_MAILING_CODE
//    LOG_CRITICAL_DATA(-1, NOT_ENOUGH_MEMORY);
#endif
    assert(0);
  }
  to = ret;
  UrlEncode(data_, to, preserve_reserved);

  if (data_) free(data_);

  len_ = strlen(ret);
  buf_size_ = len_ + 1;
  ret = (yChar*) realloc(ret, buf_size_);
  data_ = ret;
  return;
}

// helps to escape slash in the buffer
// Returns 
//      void

void wyString::EscapeSlash() {
  yInt32 index;
  yChar ch;

  for (index = 0; index < GetLen(); index++) {
    ch = GetCharAt(index);
    switch (ch) {
      case '\\':
        Replace(index, 1, "\\\\");
        index++;
        break;
    }
  }
  return;
}

void wyString::JsonValueEscape() {
  /*
  yInt32 index;
  yChar ch;
  wyString temp;

  for (index = 0; index < GetLen(); index++) {
    ch = GetCharAt(index);

    switch (ch) {
      case '\"':
        Replace(index, 1, "\\\"");
        index++;
        break;
      case '\\':
        Replace(index, 1, "\\\\");
        index++;
        break;
      case '\b':
        Replace(index, 1, "\\b");
        index++;
        break;
      case '\f':
        Replace(index, 1, "\\f");
        index++;
        break;
      case '\n':
        Replace(index, 1, "\\n");
        index++;
        break;
      case '\r':
        Replace(index, 1, "\\r");
        index++;
        break;
      case '\t':
        Replace(index, 1, "\\t");
        index++;
        break;
      default:
      {
        if (isControlCharacter(ch)) {
          temp.Sprintf("\\u%04x", ch);
          Replace(index, 1, temp.GetString(), temp.GetLen());
          index += (temp.GetLen() - 1);
        }
      }
    }
  }

  */
  yInt32 index;
  yChar ch;
  wyString temp;
  wyString t;

  for (index = 0; index < GetLen(); index++) {
    ch = GetCharAt(index);

    switch (ch) {
      case '/':
        if (index > 0 && GetCharAt(index - 1) == '<') {
          t.Add("\\/");
        } else {
          t.Add("/");
        }
        break;
      case '\"':
        t.Add("\\\"");
        break;
      case '\\':
        t.Add("\\\\");
        break;
      case '\b':
        t.Add("\\b");
        break;
      case '\f':
        t.Add("\\f");
        break;
      case '\n':
        t.Add("\\n");
        break;
      case '\r':
        t.Add("\\r");
        break;
      case '\t':
        t.Add("\\t");
        break;
      default:
      {
        if ((unsigned char)ch == 0xE2 && index < GetLen() - 2 && 
          (unsigned char)GetCharAt(index+1) == 0x80 && (unsigned char)GetCharAt(index+2) == 0xA8) {
          index = index + 2;
        } else if ((unsigned char)ch == 0xE2 && index < GetLen() - 2 && 
          (unsigned char)GetCharAt(index+1) == 0x80 && (unsigned char)GetCharAt(index+2) == 0xA9) {
          index = index + 2;
        } else if (isControlCharacter(ch)) {
          temp.Sprintf("\\u%04x", ch);
          t.Add(&temp);
        } else {
          t.AddSprintf("%c", ch);
        }
      }
    }
  }

  Set(&t);
  
}

// Return the UTF8 equivalent of the string in case of the platform is WINDOWS

yChar * wyString::IfWindowsGetAsUTF8() {
#ifdef _WIN32
  //return GetAsUTF8(); //If windows convert to UTF8 string
  yWChar* buffer = GetAsWideChar();
  wyString temp;
  temp.Set(buffer);
  Set(&temp);
#endif

  return GetString(); //Else return as it is. It is done becuase in Linux the .ini file is already in UTF8

}

// Return the UTF8 equivalent of the string

yChar * wyString::GetAsUTF8(yCodePage in_codepage, yInt32 *length) {
  yInt32 utf8len;
  yInt32 lengthtoallocate;
  wchar_t* widecharbuff;
  yInt32 lengthwidechar;

#ifdef _WIN32
  // Get the Length for allocation of Widecharacter
  lengthtoallocate = MultiByteToWideChar(in_codepage, 0, data_, len_, NULL, NULL);
#else
  //set default codepage to ANSI
  setlocale(LC_ALL, in_codepage);
  lengthtoallocate = mbstowcs(NULL, data_, len_);
#endif

  widecharbuff = (wchar_t *)calloc(sizeof (wchar_t), (lengthtoallocate + 1) * (sizeof (wchar_t)));

  if (!widecharbuff) {
#ifndef SQLITE_MAILING_CODE
//    LOG_CRITICAL_DATA(-1, NOT_ENOUGH_MEMORY);
#endif
    assert(0);
  }
#ifdef _WIN32
  //Conversion to WideCharacter
  lengthwidechar = MultiByteToWideChar(in_codepage, 0, data_, len_, widecharbuff, lengthtoallocate);
  // Get the Length for Allocation of multibyte
  lengthtoallocate = WideCharToMultiByte(CP_UTF8, 0, widecharbuff, lengthwidechar, NULL, 0, NULL, NULL);
#else
  //Conversion to WideCharacter
  lengthwidechar = mbstowcs(widecharbuff, data_, lengthtoallocate);
  setlocale(LC_ALL, "en_US.utf-8");
  lengthtoallocate = wcstombs(NULL, widecharbuff, lengthwidechar);
#endif

  if (utf8_data_) free(utf8_data_);

  utf8_data_ = (yChar*) calloc(sizeof (yChar), lengthtoallocate + 1);
  if (!utf8_data_) {
#ifndef SQLITE_MAILING_CODE
//    LOG_CRITICAL_DATA(-1, NOT_ENOUGH_MEMORY);
#endif
    assert(0);
  }

#ifdef _WIN32
  //Conversion to Multibyte
  utf8len = WideCharToMultiByte(CP_UTF8, 0, widecharbuff, lengthwidechar, utf8_data_, lengthtoallocate, NULL, NULL);
#else
  utf8len = wcstombs(utf8_data_, widecharbuff, lengthtoallocate);
#endif

  utf8_data_[utf8len] = '\0';
  if (length != NULL) {
    *length = utf8len;
  }
  free(widecharbuff);
  return utf8_data_;
}

yWChar * wyString::GetAsWideChar(yUInt32 *length) {
  yInt32 lengthwidechar;

  if (wide_data_) free(wide_data_); // if somethin is there already? then free memory

  // allocate
  wide_data_ = (yWChar *) calloc(sizeof (yWChar), (len_ + 1) * (sizeof (yWChar)));
  if (!wide_data_) {
#ifndef SQLITE_MAILING_CODE
//    LOG_CRITICAL_DATA(-1, NOT_ENOUGH_MEMORY);
#endif
    assert(0);
  }

#ifdef _WIN32

  /*Function to get Wide character.
   * Fills the Wide character buffer with converted 
   * wide character data.*/

  lengthwidechar = MultiByteToWideChar(CP_UTF8, 0, data_, len_, wide_data_,
          (len_ * sizeof (yWChar)));

#else
  setlocale(LC_ALL, "en_US.utf-8");
  lengthwidechar = mbstowcs(wide_data_, data_, (len_ * sizeof (yWChar)));
  setlocale(LC_ALL, "");

#endif

  if (length) *length = lengthwidechar;

  return wide_data_;
}

yCChar* wyString::StrIStr(yCChar* buffer, yCChar *to_find) {
  // Check for NULL
  if (!*to_find) {
    return buffer;
  }
  // loop through all till a match found, otherwise just return null
  for (; *buffer; ++buffer) {
    if (toupper(*buffer) == toupper(*to_find)) {
      /* Matched starting char -- loop through remaining chars.*/
      yCChar *h, *n;
      for (h = buffer, n = to_find; *h && *n; ++h, ++n) {
        if (toupper(*h) != toupper(*n)) {
          break;
        }
      }
      if (!*n) {/* matched all of 'to_find' to null termination */
        return buffer; /* return the start of the match */
      }
    }
  }
  return NULL;
}

yInt32 wyString::FindIAndReplace(yCChar* findstr, yCChar* replacestr) {
  yInt32 flen = strlen(findstr);
  yInt32 rlen = strlen(replacestr);
  if (flen == 0) {
    return -1;
  }
  yInt32 pos = FindI(findstr);
  while (pos != -1) {
    Replace(pos, flen, replacestr, rlen);
    pos = FindI(findstr, pos + rlen);
  }
  return GetLen();
}

yInt32 wyString::Replace(yInt32 startpos, yUInt32 originalstrlen,
        yCChar *replacestr, yUInt32 replacestrlen) {
  if (startpos == -1 || originalstrlen == -1 || replacestr == NULL) {
    ASSERT(0);
  }
  if (replacestrlen == 0) {
    replacestrlen = strlen(replacestr);
  }
  if (replacestrlen <= originalstrlen) {
    // if old string length is less than or equal to new string just replace and use memmove
    // copy new string
    memmove(data_ + startpos, replacestr, replacestrlen * sizeof (yChar));
    // move buffer
    memmove(data_ + startpos + replacestrlen, data_ + startpos + originalstrlen,
            (len_ - (startpos + originalstrlen)) * sizeof (yChar));
    len_ -= (originalstrlen - replacestrlen);
    data_[len_] = '\0';
    return len_;
  }

  if (buf_size_ <= len_ + (replacestrlen - originalstrlen)) {
    // allocate enough first if not
    buf_size_ = (len_ + (replacestrlen - originalstrlen) + 1) * 2;
    yChar* prev = data_;
    data_ = (yChar*) calloc(buf_size_, 1);
    if (!data_) {
#ifndef SQLITE_MAILING_CODE
//      LOG_CRITICAL_DATA(-1, NOT_ENOUGH_MEMORY);
#endif
      assert(0);
    }
    // copy existing stuff
    memcpy(data_, prev, len_ * sizeof (yChar));
    // delete prev buf
    if (prev) {
      free(prev);
    }
  }

  // first move the buffer
  memmove(data_ + startpos + replacestrlen, data_ + startpos + originalstrlen,
          (len_ - (startpos + originalstrlen)) * sizeof (yChar));
  // copy new string
  memmove(data_ + startpos, replacestr, replacestrlen * sizeof (yChar));
  len_ += (replacestrlen - originalstrlen);
  data_[len_] = '\0';

  return len_;
}

// helps to escape HTML entities in the buffer
// Returns 
//      void

void wyString::EscapeHTMLEntities() {
  yInt32 index;
  yChar ch;

  for (index = 0; index < GetLen(); index++) {
    ch = GetCharAt(index);
    switch (ch) {
      case '&':
        Replace(index, 1, "&amp;");
        index = index + 4;
        break;
      case '<':
        Replace(index, 1, "&lt;");
        index = index + 3;
        break;
      case '>':
        Replace(index, 1, "&gt;");
        index = index + 3;
        break;
    }
  }
  return;
}

// helps to unescape HTML entities in the buffer
// Returns 
//      void

void wyString::UnEscapeHTMLEntities() {
  FindIAndReplace("&lt;", "<");
  FindIAndReplace("&gt;", ">");
  FindIAndReplace("&amp;", "&");
}

yBool wyString::UrlDecode() {
  yInt32 i = 0, j = 0;
  for (; i < GetLen(); i++, j++) {
    yChar ch = GetCharAt(i);
    if (ch == '%') {
      if (i + 3 <= GetLen()) {
        yChar hex[3];
        hex[0] = GetCharAt(++i);
        hex[1] = GetCharAt(++i);
        hex[2] = '\0';

        yInt32 decoded;
        sscanf(hex, "%X", &decoded);

        SetCharAt(j, (yChar) decoded);
      } else {
        return FALSE;
      }
    } else if (ch == '+') {
      SetCharAt(j, ' ');
    } else {
      SetCharAt(j, ch);
    }
  }
  Strip(i - j);
  return TRUE;
}

#ifndef _WIN32
void wyString::FixUTF8() {
//  yInt32 flag = 1;
//  yChar *inbuf = GetString();
//  yUInt64 len = GetLen();
//  yUInt64 outlen = GetLen();
//  yChar *outbuf = (yChar *)calloc(len + 1, sizeof(yChar));
//  yChar *buf = outbuf;
//  iconv_t conv = iconv_open("UTF-8//IGNORE","UTF-8");
//  iconvctl(conv, ICONV_SET_DISCARD_ILSEQ, &flag);
//  iconv(conv, &inbuf, &len, &outbuf, &outlen);
//  iconv_close(conv);
//  Set(buf);
//  free(buf);
}
#endif



void TestwyString() {
    wyString temp;
    temp.Set("A € B ® C € D");
    printf("Data: %s\nLen: %d\n\n", temp.GetString(), temp.GetLen());
}