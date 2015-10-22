

#ifndef _MB_YSTRING_H
#define _MB_YSTRING_H
/*
#ifdef _WIN32
extern "C"
// This function is a MSVC internal function. It is being used as there -
//	is no vsscanf in MSVC 2003
//int __cdecl _input ( FILE *pStream, const char *format, va_list pArglist); 
int __cdecl _input_l (FILE *infile, const char *format, _locale_t plocinfo, va_list arglist );
#endif //_WIN32
*/

#include "ydefines.h"
#include <stdlib.h>
#include <string.h>
#include <cstdarg>
#include <ctype.h>

class wyString {
 public:
  wyString(){	
		vInit();
	}
  
	wyString(size_t init_bufsize)	{
		vInit();
		buf_size_ = init_bufsize;

		data_ =(yChar*)calloc(buf_size_, 1);
	}
	
	wyString(yCChar * str) {
		vInit();
		Set(str, strlen(str));
	}
	
	wyString(yCWChar * str) {
		vInit();
		Set(str);
	}

	virtual ~wyString()	{	
		FreeBuffer();
	}

  void FreeBuffer() {
    if(data_) {
      free(data_);
    }
    if(utf8_data_) {
      free(utf8_data_);
    }
    if(wide_data_) {
      free(wide_data_);
    }
    vInit();
  }
	
	// readers
	size_t			GetLen() {
		return len_;
	}
	
	yChar* GetString() {
		return (yChar *) (data_ ? data_ : "");
	}

	yCChar* GetString() const {
		return (yCChar *) (data_ ? data_ : "");
	}

	yInt32 GetAsInt32() {
		return atoi(GetString());
	}

    double GetAsDouble() {
		return atof(GetString());
	}

	yInt64 GetAsInt64() {
		return AToI64(GetString());
	}
	yUInt64 GetAsUInt64() {
			return AToUI64();
		}


	yInt32 StrICmp(wyString * str)	{
		return wyString::StrICmp(this->GetString(), str->GetString());
	}

  yInt32 StrCmp(wyString * str) {
		return wyString::StrCmp(this->GetString(), str->GetString());
	}

   yInt32	StrCmp(yChar * str) {
		 return wyString::StrCmp(this->GetString(), str);
    }

   yInt32	StrCmp(yCChar * str) {
		 return wyString::StrCmp(this->GetString(), str);
    }

	yInt32	StrICmp(yChar * str)	{
		return wyString::StrICmp(this->GetString(), str);
	}

  static yBool isControlCharacter(yChar ch) {
    return ch > 0 && ch <= 0x1F;
  }

	static yInt32	StrICmp(yCChar* str1, yCChar* str2) {
#ifdef _WIN32
		return stricmp (str1, str2);
#else
		return strcasecmp (str1, str2);
#endif //_WIN32
	}

  static yInt32	StrCmp(yCChar* str1, yCChar* str2)  {
    return strcmp (str1, str2);
  }

  static yInt32	StrNCmp(yCChar* str1, yCChar* str2, yInt32 pLen)  {
    return strncmp (str1, str2, pLen);
  }
    
	static yInt32	StrNICmp(yCChar* str1, yCChar* str2, yInt32 pLen) {
#ifdef _WIN32
		return strnicmp (str1, str2, pLen);
#else
		return strncasecmp (str1, str2, pLen);
#endif //_WIN32
	}

	static yChar* StrCpy(yChar* dest, yCChar* src) {
		//ASSERT(dest);
		//ASSERT(src);
		strcpy(dest, src);
		return dest;
	}

	static yChar* StrNCpy(yChar* dest, yCChar* src, size_t size) {
		//ASSERT(dest);
		//ASSERT(src);
		strncpy(dest, src, size);
		return dest;
	}

	yInt32 StrNICmp(wyString* str, size_t count) {
		return StrNICmp(this->GetString(), str->GetString(), count);
	}

	yInt32 StrNICmp(yChar* str, size_t count) {
		return StrNICmp(this->GetString(), str, count);
	}

	static yInt32	StrNICmp(yCChar* str1, yCChar* str2, size_t count) {
#ifdef _WIN32
		return strnicmp(str1, str2, count);
#else
		return strncasecmp(str1, str2, count);
#endif //_WIN32
	}

	yBool IsEqual(yChar* str) {
		return(StrICmp(str)== 0);
	}

	yBool IsEmpty() {
		return(len_ == 0);
	}
  static yInt32 AToI32(const yChar* str) {
		return atoi(str);
	}

	static yInt64	AToI64(const yChar* str) {
#ifdef _WIN32
		return _atoi64(str);
#else
		return atoll(str);
#endif //_WIN32
	}

	yInt64 AToI64() {
#ifdef _WIN32
		return _atoi64(data_);
#else
		return atoll(data_);
#endif //_WIN32
	}

	yUInt64 AToUI64() {
		yUInt64  convval;
		if (!data_) {
			return 0;
		}
#ifdef _WIN32
 convval = _strtoui64(data_, NULL, 10);
#else
 convval = strtoull(data_, NULL, 10);
#endif
 return convval;
	}



	static double	AToDouble(const yChar* str) {
		return atof(str);
	}

	double AToDouble() {
		return atof(data_);
	}

	// returns a char at a specified position
	// Parameters
	//      index: char position
	//      pchar: chart at index[OUT]
	// Returns 
	//      TRUE if index is valid, FALSE otherwise
	yBool GetCharAt(yUInt32 index, yChar *pchar) {
		if(index > len_) {
			return FALSE;
		}	else {
			*pchar = data_[index];
			return TRUE;
		}
	}

	// returns a char at a specified position
	// Parameters
	//      index: char position
	// Returns 
	//      char at index if index is valid, '\0' otherwise
	yChar GetCharAt(yUInt32 index) {
    if(index > len_) {
			return YNULCHAR;
    }
			return data_[index];
	}

  yBool SetCharAt(yUInt32 index, yChar ch) {
    if(index >= len_) {
			return FALSE;
    }
		
		data_[index] = ch;
    return TRUE;
	}

	yInt32 Strip(yInt32 size);

	wyString* Dup();
	//Using iconv to ignore wrong utf8 characters
	void FixUTF8();;
//write formatted output using a pointer to a list of arguments.
	static yInt32	VsnPrintf(yChar* str, size_t size, const yChar* format, va_list ap);
	// writers
	void SetEmpty()	{
		if(buf_size_)	*data_ = 0;

		len_ = 0;
	}

	void Set(yCChar* buf, size_t pLen = vAllBitsOne);
	void Set(wyString* src);	
	void Set(const yWChar* src);
  void Set(yInt64 int_data) {
    SetEmpty();
    Add(int_data);
  }
  void AddUInt64(yUInt64 int_data);

	// adds a buffer to the existing buffer
	// Parameters
	//      buf: buffer to add
	//      pLen: buffer len to add
	// Returns 
	//      void
	void Add(yCChar * buf, size_t pLen = vAllBitsOne);
  void Add(const yWChar * src);

  static yBool IsValidUtf8Buffer(yCByte *bytes, size_t len=-1);
  yBool IsValidUtf8Buffer();

	// adds a string object to the existing buffer
	// Parameters
	//      str: string object to add
	// Returns 
	//      void
	void Add(wyString* str);	

	// Helps to add a integer value to the buffer
	// Parameters
	//      int_data: int data
	// Returns 
	//      void
	void Add(yInt32 int_data);


	// Helps to add a integer value to the buffer
	// Parameters
	//      int_data: int data
	// Returns 
	//      void
	void Add(yInt64 int_data);
  void Add(yULong int_data);

	// Adds the buffer, here the parameter supports formatstring 
	// Parameters
	//      format_str: format string
	//  Returns 
	//      void
	void AddSprintf(yCChar* format_str, ...);

	// Sets the buffer, here the parameter supports formatstring
	// Parameters
	//      format_str: format string
	//  Returns 
	//      void
	void Sprintf(yCChar* format_str, ...);

	// Copies a string of specidfied length from a string object
	// Parameters
	//      str: the source string object
	//      pMax: length
	// Returns 
	//      void
	void StrNCpy(wyString * str, size_t pMax);
	void StrNCpy(yChar* str, size_t pMax);

	// strips up to a token(including token)
	// Parameters
	//      token: token to find and strip
	//      new_string: the string upto token[OUT]
	// Returns
	//      TRUE if not empty, FALSE otherwise
	yBool StripToken(yChar* token, wyString* new_string);

	// skips the first white spaces and next token from the buffer
	// Returns 
	//      TRUE always
	yBool SkipToken();

	// helps to Escape a url
	// Parameters
	//      preserve_reserved: reserved characters can only be used in certain places
	// Returns 
	//      void
	void UnEscapeURL();

	// helps to Escape a url
	// Parameters
	//      preserve_reserved: reserved characters can only be used in certain places
	// Returns 
	//      void
	void EscapeURL(yBool preserve_reserved = FALSE);

	// fills with str bufffer repeat times
	// Parametrs
	//      str: bufffer to fill
	//      prepeat: repeat count
	// Returns 
	//      Void
	void Fill(yChar* str, yUInt32 repeat);

	// finds a string with out considering the case
	// Parameters
	//      pToFind: string to find
	// Returns 
	//      position if found, -1 otherwise
	size_t FindI(yCChar* pToFind, yUInt32 pos = 0);

	// finds a string by considering the case
	// Parameters
	//      pToFind: string to find
	// Returns 
	//      position if found, -1 otherwise
	size_t Find(yCChar* pToFind);


  static size_t Find(yCChar * from_, yCChar* to_find);


	// converts all buffer to lower case
	void ToLower();
        void ToUpper();

	// Sets the buffer, here the parameter supports formatstring
	// Parameters
	//      format_str: format string
	//      va_list: argument list
	//  Returns 
	//      void
	void vSprintf(yCChar* format_str, va_list va_list);

	// sscanf with the variable argument list specified directly
	yInt32 vSscanf(yCChar* format_str, va_list va_list);

	// Read formatted data from a string
	// Parameters
	//      format_str: format string
	//  Returns 
	//      the number of fields successfully converted and assigned
	yInt32 Sscanf(yCChar* format_str, ...);

	// removes pchar(single/consecutive occurence) from the left of the buffer, 
	// Parameters
	//      pchar : Char to remove
	// Returns
	//      void
	void StripLChar(yChar pchar);
  void StripLChar(yInt32 size);
	void ReplaceTabWithSpace();

	// strips left white space
	void StripLWhiteSpace();

	// strips right white space
	void StripRWhiteSpace();

	// strips white spaces from bith sides of the buffer
	void StripWhiteSpace();

	// helps to escape slash in the buffer
	void EscapeSlash();

  // helps to escape HTML entities in the buffer
  void EscapeHTMLEntities();
  void UnEscapeHTMLEntities();

	//http://tools.ietf.org/html/rfc4627

	// helps to escape as javascript escape
	void JsonValueEscape();

    // converts the buffer to utf8 and sends
  yChar* GetAsUTF8(yCodePage in_codepage = WY_CODEPAGE_ASCII, yInt32 *length = NULL);

	// wide char to test
	yWChar* GetAsWideChar(yUInt32 *pLen = NULL);
    yInt32 Replace(yInt32 startpos, yUInt32 originalstrlen, yCChar *replacestr, yUInt32 replacestrlen = 0);
	yInt32 FindIAndReplace(yCChar* findstr, yCChar* replacestr);

	//return the UTF8 equivalent of the string, if the platform is windows
	yChar* IfWindowsGetAsUTF8();	
	
	// Specific to apple. Used to remove unwanted/corrupted chars from 'us-ascii' charset
	void FixAscii();  	

  static yCChar* StrIStr(yCChar* buffer, yCChar *to_find);
	void TrimMatchingCharFromEnd(yCChar match);

  yBool UrlDecode();

 private:
	void vInit() {
		// should only be called from constructors
		// otherwise will leak momory because its losing data_
		buf_size_ = 0;
		data_ = NULL;
		utf8_data_ = NULL;
		len_ = 0;
		wide_data_ = NULL;
	}

    operator yChar*() {
        return GetString();
    }

	yChar* data_;	        // internal data holding pointer
	yChar* utf8_data_;	        // internal data holding pointer
	size_t len_;			// current lint in widechars(not size in bytes!)
	size_t buf_size_;		// in bytes	
	yWChar* wide_data_;

	//checks char is always safe for putting in a URL: 
	// Parameters
	//      pchar: char to check
	// Returns 
	//      1 if safe, 0 otherwise
		yInt32 IsUrlSafe(yChar pchar);

	// checks whether the char is included in the reserved list
	// Parameters
	//      pchar:  char to check
	// Returns 
	//      1 if reserved, 0 otherwise
	yInt32 IsUrlReservedSafe(yChar pchar);

	// Helper function to URL encode the buffer
	// Parameters
	//      pFrom: source
	//      pTo: result buffer[OUT]
	//      preserve_reserved: reserved characters can only be used in certain places
	// Returns 
	//      void
	void	UrlEncode(const yChar* pFrom, yChar* pTo, yBool preserve_reserved);

  

	static const size_t	vAllBitsOne; 
	
    // declare the default constructor which are not supported 
    // in the private section so we won't accidently use this
	wyString(const wyString& p) {
		UNREFERENCED_PARAMETER(p);
	};
	wyString& operator=(const wyString& p) {
		UNREFERENCED_PARAMETER(p);
		return *this;
	}

};

#endif

