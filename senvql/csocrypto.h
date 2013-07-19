//
//  csocrypto.h
//  csobox
//
//  Created by Daniel Grigg on 18/07/13.
//  Copyright (c) 2013 Daniel Grigg. All rights reserved.
//

#ifndef __csobox__csocrypto__
#define __csobox__csocrypto__

//#include <iostream>

#ifdef __cplusplus
extern "C" {
#endif
  
int render_envelope(const char* filename, const char* out_dir);
  
#ifdef __cplusplus
}
#endif

#endif /* defined(__csobox__csocrypto__) */
