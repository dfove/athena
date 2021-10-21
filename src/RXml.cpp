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

#include <RXml.hpp>

#include <libxml/tree.h>
#include <libxml/parser.h>
#include <libxml/xpath.h>
#include <libxml/encoding.h>
#include <libxml/xmlwriter.h>
#include <libxml/parserInternals.h>

#ifdef HAVE_ICONV_H
#include "iconv.h"
#endif

#include <stdlib.h>
#include <stdio.h>
#include <string.h>
#include <unistd.h>

using namespace athena;

RXml::RXml()
     :m_parserCtx(NULL),
      m_doc(NULL),
      m_xpathCtx(NULL)
{
#ifdef HAVE_ICONV_H
    m_convToUtf8 = iconv_open("UTF-8", "char");
    m_convToAscii = iconv_open("char", "UTF-8");
#endif
}

RXml::~RXml()
{
    Clean();
#ifdef HAVE_ICONV_H
    iconv_close(m_convToUtf8);
    m_convToUtf8 = NULL;
    iconv_close(m_convToAscii);
    m_convToAscii = NULL;
#endif
}

void
RXml::Clean()
{
    m_file.clear();
    if (m_parserCtx != NULL)
    {
        if ( m_parserCtx->myDoc != NULL )
        {
            xmlFreeDoc(m_parserCtx->myDoc);
            m_parserCtx->myDoc = NULL;
        }
        xmlFreeParserCtxt(m_parserCtx);
        m_parserCtx = NULL;
    }
    if ( m_doc != NULL )
    {
        xmlFreeDoc(m_doc);
        m_doc = NULL;
    }
    if (m_xpathCtx != NULL)
    {
        xmlXPathFreeContext(m_xpathCtx); 
        m_xpathCtx = NULL;
    }
}

int
RXml::ParseFile( const char * fileName, const char * namespaces, bool autoCreate)
{
    Clean();

    // load from file firstly.
    m_parserCtx = NULL;
    if ( !access(fileName, F_OK))
        m_parserCtx = xmlCreateFileParserCtxt( fileName);
    if (m_parserCtx != NULL)
    {
        m_file = fileName;
    }
    else
    {
        if (autoCreate && GetInitXmlContent() != NULL)
        {
            const RString & content = *(GetInitXmlContent());
            // load from memory with initiated xml content.
            m_parserCtx = xmlCreateMemoryParserCtxt(content.c_str(), (int)content.length());
        }
        if (m_parserCtx == NULL)
            return -1;
    }
    if (xmlParseDocument(m_parserCtx) != 0)
    {
        Clean();
        return -1;
    }
    //printf("encoding: %d [%s]\n", m_parserCtx->charset, m_parserCtx->encoding);

    /*
    xmlCharEncodingHandlerPtr handler = xmlFindCharEncodingHandler("ASCII");
    if (handler == NULL)
        printf("### handler is null!\n");
    else
    {
        printf("### handler is not null!\n");
        if (handler->input == NULL)
            printf("### handler->input is null!\n");
        else
            printf("### handler->input is not null!\n");
        if (handler->output == NULL)
            printf("### handler->output is null!\n");
        else
            printf("### handler->output is not null!\n");
    }
    */

    /* Create xpath evaluation context */
    m_xpathCtx = xmlXPathNewContext( m_parserCtx->myDoc);
    if (m_xpathCtx == NULL)
    {
        Clean();
        return -1;
    }
    
    /* Register namespaces from list (if any) */
    if (namespaces != NULL && 
        RegisterNamespaces( m_xpathCtx, namespaces) < 0)
    {
        Clean();
        return -1;
    }

    return 0;
}

int 
RXml::ParseMemory(const char* buffer, int bufsize)
{
    Clean();
    if ( bufsize == -1 )
        m_doc = xmlParseDoc((xmlChar*)buffer);
    else if ( bufsize > 0 )
        m_doc = xmlParseMemory(buffer, bufsize);
    else
        return -1;

    if ( m_doc == NULL )
        return -1;

    m_xpathCtx = xmlXPathNewContext(m_doc);
    if (m_xpathCtx == NULL) 
    {
        return -1; 
    }
    return 0;
}

int
RXml::RegisterNamespaces( xmlXPathContextPtr xpathCtx, const char * namespaces)
{
    return 0;
}

xmlDoc *
RXml::GetDoc() const
{
    if ( m_parserCtx != NULL )
        return m_parserCtx->myDoc;
    else if ( m_doc != NULL )
        return m_doc;
    return NULL;
}

const char *
RXml::GetEncoding() const
{
    if (m_parserCtx == NULL)
        return NULL;
    return (const char *)(m_parserCtx->encoding);
}


xmlNodeSet *
RXml::GetNodeSet( const char * xpathExpression, xmlXPathObjectPtr& xpathObj) const
{
    if (xpathExpression == NULL || strlen(xpathExpression) < 1)
        return NULL;
    if (m_xpathCtx == NULL)
        return NULL;

    xpathObj = xmlXPathEvalExpression( BAD_CAST xpathExpression, m_xpathCtx);
    if (xpathObj == NULL)
        return NULL;

    xmlNodeSetPtr nodes = NULL;
    if(xmlXPathNodeSetIsEmpty(xpathObj->nodesetval))
    {
        xmlXPathFreeObject(xpathObj);
        xpathObj = NULL;
        return NULL;
    }
    nodes = xpathObj->nodesetval;

    return nodes;
}

xmlNode *
RXml::GetNode( const char * xpathExpression) const
{
    xmlNodePtr fndNode = NULL;
    xmlXPathObjectPtr xpathObj = NULL;
    xmlNodeSetPtr nodes = GetNodeSet( xpathExpression, xpathObj);
    if (nodes && nodes->nodeTab && nodes->nodeTab[0])
        fndNode = nodes->nodeTab[0];
    if (xpathObj != NULL)
    {
        xmlXPathFreeObject(xpathObj);
    }
    return fndNode;
}

int
RXml::GetValue(const char * xpathExpression, RString & value) const
{
    value.clear();
    xmlNodePtr fndNode = GetNode( xpathExpression);
    int ret = -1;
    if ( fndNode == NULL )
        return ret;
    if (fndNode->type == XML_ELEMENT_NODE)
    {
        xmlNodePtr cur = fndNode;
        if (cur->children && cur->children->type == XML_TEXT_NODE)
        {
            value = (const char *)(cur->children->content);
            ret = 0;
        }
    }
    else if (fndNode->type == XML_ATTRIBUTE_NODE)
    {
        xmlAttrPtr cur = (xmlAttrPtr)fndNode;
        if (cur->children && cur->children->type == XML_TEXT_NODE)
        {
            value = (const char *)(cur->children->content);
            ret = 0;
        }
    }
    //xmlFree(fndNode);
    return ret;
}

int 
RXml::GetValue(const char* xpathExpression, std::vector< RString >& values) const
{
    values.clear();
    xmlDoc* doc = GetDoc();
    if ( doc == NULL )
        return -1;
    xmlChar *keyword;
    xmlXPathObjectPtr xpathObj = NULL;
    xmlNodeSetPtr nodes = GetNodeSet( xpathExpression, xpathObj);
    if ( xpathObj == NULL || nodes == NULL )
        return -1;
    for (int i=0; i < nodes->nodeNr; i++) 
    {
        keyword = xmlNodeListGetString(doc, nodes->nodeTab[i]->xmlChildrenNode, 1);
        values.push_back((char*)keyword);
        xmlFree(keyword);
    }
    xmlXPathFreeObject(xpathObj);
    return 0;
}

void
RXml::DumpBuffer( RString & buffer)
{
    xmlChar* buf = NULL;
    int size = 0;
    xmlDocDumpMemory( GetDoc(), &buf, &size);
    buffer.assign((char*)buf, size);
    xmlFree(buf);
}

#ifdef HAVE_ICONV_H
int
RXml::ConvtEnc( void * cd, const char* * in, int *inlen,
                         char* * out, int *outlen) const
{
    size_t icv_inlen, icv_outlen;
    icv_inlen = *inlen;
    icv_outlen = *outlen;

    iconv((iconv_t)cd, NULL, NULL, NULL, NULL);

    size_t i = iconv((iconv_t)cd, in, &icv_inlen, out, &icv_outlen);
    if (in != NULL)
    {
        *inlen -= (int)icv_inlen;
        *outlen -= (int)icv_outlen;
    }
    else
    {
        *inlen = 0;
        *outlen = 0;
    }
    return (int)i;
}
#endif

int
RXml::AsciiToUtf8( const char* * in, int *inlen,
                            char* * out, int *outlen) const
{
#ifdef HAVE_ICONV_H
    return convtEnc(m_convToUtf8, in, inlen, out, outlen);
#else
    //Be specified in the inherited class.
    int len = (*inlen < *outlen) ? *inlen : *outlen;
    memcpy( *out, *in, len);
    return 0;
#endif
}

int
RXml::AsciiToUtf8( const char * in, RString & out) const
{
    int inlen = (int)strlen(in);
    int outlen = inlen * 2;
    char * buf = new char[outlen];
    memset(buf, 0, outlen);
    char * output = buf;
    int i = AsciiToUtf8( &in, &inlen, &output, &outlen);
    if (i == 0)
    {
        out.assign(buf, outlen);
    }
    delete[] buf;
    return i;
}

int
RXml::Utf8ToAscii( const char* * in, int *inlen,
                            char* * out, int *outlen) const
{
#ifdef HAVE_ICONV_H
    return convtEnc(m_convToAscii, in, inlen, out, outlen);
#else
    //Be specified in the inherited class.
    int len = (*inlen < *outlen) ? *inlen : *outlen;
    memcpy( *out, *in, len);
    return 0;
#endif
}

int
RXml::Utf8ToAscii( const char * in, RString & out) const
{
    int inlen = (int)strlen(in);
    int outlen = inlen * 2;
    char * buf = new char[outlen];
    memset(buf, 0, outlen);
    char * output = buf;
    int i = Utf8ToAscii( &in, &inlen, &output, &outlen);
    if (i == 0)
    {
        out.assign(buf, outlen);
    }
    delete[] buf;
    return i;
}
