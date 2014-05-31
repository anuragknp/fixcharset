//
//  beetle.h
//  Test
//
//  Created by Manoj on 20/03/14.
//  Copyright (c) 2014 Manoj. All rights reserved.
//

#ifndef cm_beetle_h
#define cm_beetle_h


#include "ystring.h"

void TrimCharset(wyString *charset);
yBool IsCharsetValidUnderUTF8(wyString *charset);
void FixCharset(wyString *charset, yBool *is_utf8);

#endif
