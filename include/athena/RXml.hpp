/*
 * Copyright (c) 2021 dfove.com Authors. All Rights Reserved.
 *
 * Licensed under the Apache License, Version 2.0 (the "License");
 * you may not use this file except in compliance with the License.
 * You may obtain a copy of the License at
 *
 *     http://www.apache.org/licenses/LICENSE-2.0
 *
 * Unless required by applicable law or agreed to in writing, software
 * distributed under the License is distributed on an "AS IS" BASIS,
 * WITHOUT WARRANTIES OR CONDITIONS OF ANY KIND, either express or implied.
 * See the License for the specific language governing permissions and
 * limitations under the License.
 */

#ifndef RXML_HPP_HEADER_INCLUDED
#define RXML_HPP_HEADER_INCLUDED

/*  =======================================
 *  $Id$
 *
 *  $Archive: RXml.hpp $
 *  $Revision: 1 $
 *  $Author: Leijh $
 *  $Date: 2009-05-23 11:17 $
 */

#include <Unidef.hpp>
#include <RString.hpp>
#include <vector>
#include <libxml/parser.h>
#include <libxml/xpath.h>

//typedef struct _xmlParserCtxt xmlParserCtxt;
//typedef struct _xmlDoc xmlDoc;
//typedef struct _xmlNode xmlNode;
//typedef struct _xmlXPathContext xmlXPathContext;
//typedef struct _xmlXPathObject xmlXPathObject;
//typedef struct _xmlNodeSet xmlNodeSet;

namespace athena
{

class ATHENA_EX RXml
{
public:
    RXml();

    virtual ~RXml();

    /*
     * read xml from file
     */
    virtual int ParseFile(const char * fileName,
                          const char * namespaces = NULL,
                          bool autoCreate = false);

    /*
     *  read xml from memory
     *  Params:
     *      buffer  the memory string
     *      bufsize the length of the memory to parse;
     *          default,-1  the buff end with '\0', bufsize = strlen(buffer)
     *          >0          the length
     */
    virtual int ParseMemory(const char* buffer, int bufsize = -1);

    const char * GetEncoding() const;

    int GetValue(const char* xpathExpression, RString & value) const;

    int GetValue(const char* xpathExpression, std::vector< RString >& values) const;

    void DumpBuffer(RString & buffer);

    virtual int AsciiToUtf8( const char* * in, int *inlen,
                             char* * out, int *outlen) const;
    int AsciiToUtf8( const char * in, RString & out) const;

    virtual int Utf8ToAscii( const char* * in, int *inlen,
                             char* * out, int *outlen) const;
    int Utf8ToAscii( const char * in, RString & out) const;

  protected:
    xmlDoc * GetDoc() const;

    xmlNode * GetNode( const char * xpathExpression) const;
    /**
     * Get nodes with the xpath expression.
     * Note: xpathObj should be freed after calling this method.
     */
    xmlNodeSet * GetNodeSet( const char * xpathExpression, xmlXPathObjectPtr& xpathObj) const;

    virtual const RString * GetInitXmlContent() const {return NULL;}

  private:
    void Clean();

    int RegisterNamespaces( xmlXPathContext * xpathCtx, const char * namespaces);

#ifdef HAVE_ICONV_H
    int ConvtEnc( void * cd, const char* * in, int *inlen,
                  char* * out, int *outlen) const;
#endif

  private:
    RString     m_file;

    xmlParserCtxt * m_parserCtx;

    xmlDoc*         m_doc;

    xmlXPathContext * m_xpathCtx;

#ifdef HAVE_ICONV_H
    void * m_convToUtf8;

    void * m_convToAscii;
#endif

};

}   // namespace

#endif /* RXML_HPP_HEADER_INCLUDED */
