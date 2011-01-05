/*
 * UriSyntax.cpp -- URI syntax according to RFC3986
 *
 * Copyright (c) 2007-2011, Frank Mertens
 *
 * See ../LICENSE for the license.
 */

#include "UriSyntax.hpp"

namespace ftl
{

UriSyntax::UriSyntax()
{
	// automatically generated from UriSyntax.abnf!
	
	DEFINE_VOID("ALPHA",
	  CHOICE(
	    RANGE('a', 'z'),
	    RANGE('A', 'Z')
	  )
	);
	
	DEFINE_VOID("DIGIT",
	  RANGE('0', '9')
	);
	
	DEFINE_VOID("HEXDIG",
	  CHOICE(
	    INLINE("DIGIT"),
	    CHOICE(
	      RANGE('a', 'f'),
	      RANGE('A', 'F')
	    )
	  )
	);
	
	DEFINE_VOID("BIT",
	  RANGE("01")
	);
	
	DEFINE_VOID("WSP",
	  RANGE(" \t")
	);
	
	DEFINE_VOID("CHAR",
	  RANGE('\1', '\177')
	);
	
	DEFINE_VOID("VCHAR",
	  RANGE('!', '~')
	);
	
	DEFINE_VOID("CR",
	  CHAR('\r')
	);
	
	DEFINE_VOID("LF",
	  CHAR('\n')
	);
	
	DEFINE_VOID("HTAB",
	  CHAR('\t')
	);
	
	DEFINE_VOID("SP",
	  CHAR(' ')
	);
	
	DEFINE_VOID("DQUOTE",
	  CHAR('"')
	);
	
	DEFINE_VOID("CRLF",
	  GLUE(
	    REPEAT(0, 1,
	      INLINE("CR")
	    ),
	    INLINE("LF")
	  )
	);
	
	DEFINE_VOID("CTL",
	  CHOICE(
	    RANGE('\0', '\37'),
	    CHAR('\177')
	  )
	);
	
	DEFINE_VOID("LWSP",
	  REPEAT(
	    CHOICE(
	      INLINE("WSP"),
	      GLUE(
	        INLINE("CRLF"),
	        INLINE("WSP")
	      )
	    )
	  )
	);
	
	DEFINE("URI",
	  GLUE(
	    REF("scheme"),
	    CHAR(':'),
	    REF("hier-part"),
	    REPEAT(0, 1,
	      GLUE(
	        CHAR('?'),
	        REF("query")
	      )
	    ),
	    REPEAT(0, 1,
	      GLUE(
	        CHAR('#'),
	        REF("fragment")
	      )
	    )
	  )
	);
	
	DEFINE("hier-part",
	  CHOICE(
	    GLUE(
	      STRING("//"),
	      REF("authority"),
	      REF("path-abempty")
	    ),
	    CHOICE(
	      REF("path-absolute"),
	      CHOICE(
	        REF("path-rootless"),
	        REF("path-empty")
	      )
	    )
	  )
	);
	
	DEFINE("URI-reference",
	  CHOICE(
	    REF("URI"),
	    REF("relative-ref")
	  )
	);
	
	DEFINE("absolute-URI",
	  GLUE(
	    REF("scheme"),
	    CHAR(':'),
	    REF("hier-part"),
	    REPEAT(0, 1,
	      GLUE(
	        CHAR('?'),
	        REF("query")
	      )
	    )
	  )
	);
	
	DEFINE("relative-ref",
	  GLUE(
	    REF("relative-part"),
	    REPEAT(0, 1,
	      GLUE(
	        CHAR('?'),
	        REF("query")
	      )
	    ),
	    REPEAT(0, 1,
	      GLUE(
	        CHAR('#'),
	        REF("fragment")
	      )
	    )
	  )
	);
	
	DEFINE("relative-part",
	  CHOICE(
	    GLUE(
	      STRING("//"),
	      REF("authority"),
	      REF("path-abempty")
	    ),
	    CHOICE(
	      REF("path-absolute"),
	      CHOICE(
	        REF("path-noscheme"),
	        REF("path-empty")
	      )
	    )
	  )
	);
	
	DEFINE("scheme",
	  GLUE(
	    REF("ALPHA"),
	    REPEAT(
	      CHOICE(
	        REF("ALPHA"),
	        CHOICE(
	          REF("DIGIT"),
	          RANGE("+-.")
	        )
	      )
	    )
	  )
	);
	
	DEFINE("authority",
	  GLUE(
	    REPEAT(0, 1,
	      GLUE(
	        REF("userinfo"),
	        CHAR('@')
	      )
	    ),
	    REF("host"),
	    REPEAT(0, 1,
	      GLUE(
	        CHAR(':'),
	        REF("port")
	      )
	    )
	  )
	);
	
	DEFINE("userinfo",
	  REPEAT(
	    CHOICE(
	      REF("unreserved"),
	      CHOICE(
	        REF("pct-encoded"),
	        CHOICE(
	          REF("sub-delims"),
	          CHAR(':')
	        )
	      )
	    )
	  )
	);
	
	DEFINE("host",
	  CHOICE(
	    REF("IP-literal"),
	    CHOICE(
	      REF("IPv4address"),
	      REF("reg-name")
	    )
	  )
	);
	
	DEFINE("port",
	  REPEAT(
	    REF("DIGIT")
	  )
	);
	
	DEFINE("IP-literal",
	  GLUE(
	    CHAR('['),
	    CHOICE(
	      REF("IPv6address"),
	      REF("IPvFuture")
	    ),
	    CHAR(']')
	  )
	);
	
	DEFINE("IPvFuture",
	  GLUE(
	    CHAR('v'),
	    REPEAT(1,
	      REF("HEXDIG")
	    ),
	    CHAR('.'),
	    REPEAT(1,
	      CHOICE(
	        REF("unreserved"),
	        CHOICE(
	          REF("sub-delims"),
	          CHAR(':')
	        )
	      )
	    )
	  )
	);
	
	DEFINE("IPv6address",
	  CHOICE(
	    GLUE(
	      REPEAT(6, 6,
	        GLUE(
	          REF("h16"),
	          CHAR(':')
	        )
	      ),
	      REF("ls32")
	    ),
	    CHOICE(
	      GLUE(
	        STRING("::"),
	        REPEAT(5, 5,
	          GLUE(
	            REF("h16"),
	            CHAR(':')
	          )
	        ),
	        REF("ls32")
	      ),
	      CHOICE(
	        GLUE(
	          REPEAT(0, 1,
	            REF("h16")
	          ),
	          STRING("::"),
	          REPEAT(4, 4,
	            GLUE(
	              REF("h16"),
	              CHAR(':')
	            )
	          ),
	          REF("ls32")
	        ),
	        CHOICE(
	          GLUE(
	            REPEAT(0, 1,
	              GLUE(
	                REPEAT(0, 1,
	                  GLUE(
	                    REF("h16"),
	                    CHAR(':')
	                  )
	                ),
	                REF("h16")
	              )
	            ),
	            STRING("::"),
	            REPEAT(3, 3,
	              GLUE(
	                REF("h16"),
	                CHAR(':')
	              )
	            ),
	            REF("ls32")
	          ),
	          CHOICE(
	            GLUE(
	              REPEAT(0, 1,
	                GLUE(
	                  REPEAT(0, 2,
	                    GLUE(
	                      REF("h16"),
	                      CHAR(':')
	                    )
	                  ),
	                  REF("h16")
	                )
	              ),
	              STRING("::"),
	              REPEAT(2, 2,
	                GLUE(
	                  REF("h16"),
	                  CHAR(':')
	                )
	              ),
	              REF("ls32")
	            ),
	            CHOICE(
	              GLUE(
	                REPEAT(0, 1,
	                  GLUE(
	                    REPEAT(0, 3,
	                      GLUE(
	                        REF("h16"),
	                        CHAR(':')
	                      )
	                    ),
	                    REF("h16")
	                  )
	                ),
	                STRING("::"),
	                REF("h16"),
	                CHAR(':'),
	                REF("ls32")
	              ),
	              CHOICE(
	                GLUE(
	                  REPEAT(0, 1,
	                    GLUE(
	                      REPEAT(0, 4,
	                        GLUE(
	                          REF("h16"),
	                          CHAR(':')
	                        )
	                      ),
	                      REF("h16")
	                    )
	                  ),
	                  STRING("::"),
	                  REF("ls32")
	                ),
	                CHOICE(
	                  GLUE(
	                    REPEAT(0, 1,
	                      GLUE(
	                        REPEAT(0, 5,
	                          GLUE(
	                            REF("h16"),
	                            CHAR(':')
	                          )
	                        ),
	                        REF("h16")
	                      )
	                    ),
	                    STRING("::"),
	                    REF("h16")
	                  ),
	                  GLUE(
	                    REPEAT(0, 1,
	                      GLUE(
	                        REPEAT(0, 6,
	                          GLUE(
	                            REF("h16"),
	                            CHAR(':')
	                          )
	                        ),
	                        REF("h16")
	                      )
	                    ),
	                    STRING("::")
	                  )
	                )
	              )
	            )
	          )
	        )
	      )
	    )
	  )
	);
	
	DEFINE("h16",
	  REPEAT(1, 4,
	    REF("HEXDIG")
	  )
	);
	
	DEFINE("ls32",
	  CHOICE(
	    GLUE(
	      REF("h16"),
	      CHAR(':'),
	      REF("h16")
	    ),
	    REF("IPv4address")
	  )
	);
	
	DEFINE("IPv4address",
	  GLUE(
	    REF("dec-octet"),
	    CHAR('.'),
	    REF("dec-octet"),
	    CHAR('.'),
	    REF("dec-octet"),
	    CHAR('.'),
	    REF("dec-octet")
	  )
	);
	
	DEFINE("dec-octet",
	  CHOICE(
	    REF("DIGIT"),
	    CHOICE(
	      GLUE(
	        RANGE('1', '9'),
	        REF("DIGIT")
	      ),
	      CHOICE(
	        GLUE(
	          CHAR('1'),
	          REPEAT(2, 2,
	            REF("DIGIT")
	          )
	        ),
	        CHOICE(
	          GLUE(
	            CHAR('2'),
	            RANGE('0', '4'),
	            REF("DIGIT")
	          ),
	          GLUE(
	            STRING("25"),
	            RANGE('0', '5')
	          )
	        )
	      )
	    )
	  )
	);
	
	DEFINE("reg-name",
	  REPEAT(
	    CHOICE(
	      REF("unreserved"),
	      CHOICE(
	        REF("pct-encoded"),
	        REF("sub-delims")
	      )
	    )
	  )
	);
	
	DEFINE("path",
	  CHOICE(
	    REF("path-abempty"),
	    CHOICE(
	      REF("path-absolute"),
	      CHOICE(
	        REF("path-noscheme"),
	        CHOICE(
	          REF("path-rootless"),
	          REF("path-empty")
	        )
	      )
	    )
	  )
	);
	
	DEFINE("path-abempty",
	  REPEAT(
	    GLUE(
	      CHAR('/'),
	      REF("segment")
	    )
	  )
	);
	
	DEFINE("path-absolute",
	  GLUE(
	    CHAR('/'),
	    REPEAT(0, 1,
	      GLUE(
	        REF("segment-nz"),
	        REPEAT(
	          GLUE(
	            CHAR('/'),
	            REF("segment")
	          )
	        )
	      )
	    )
	  )
	);
	
	DEFINE("path-noscheme",
	  GLUE(
	    REF("segment-nz-nc"),
	    REPEAT(
	      GLUE(
	        CHAR('/'),
	        REF("segment")
	      )
	    )
	  )
	);
	
	DEFINE("path-rootless",
	  GLUE(
	    REF("segment-nz"),
	    REPEAT(
	      GLUE(
	        CHAR('/'),
	        REF("segment")
	      )
	    )
	  )
	);
	
	DEFINE("path-empty",
	  PASS()
	);
	
	DEFINE("segment",
	  REPEAT(
	    REF("pchar")
	  )
	);
	
	DEFINE("segment-nz",
	  REPEAT(1,
	    REF("pchar")
	  )
	);
	
	DEFINE("segment-nz-nc",
	  REPEAT(1,
	    CHOICE(
	      REF("unreserved"),
	      CHOICE(
	        REF("pct-encoded"),
	        CHOICE(
	          REF("sub-delims"),
	          CHAR('@')
	        )
	      )
	    )
	  )
	);
	
	DEFINE_VOID("pchar",
	  CHOICE(
	    REF("unreserved"),
	    CHOICE(
	      REF("pct-encoded"),
	      CHOICE(
	        REF("sub-delims"),
	        RANGE(":@")
	      )
	    )
	  )
	);
	
	DEFINE("query",
	  REPEAT(
	    CHOICE(
	      REF("pchar"),
	      RANGE("/?")
	    )
	  )
	);
	
	DEFINE("fragment",
	  REPEAT(
	    CHOICE(
	      REF("pchar"),
	      RANGE("/?")
	    )
	  )
	);
	
	DEFINE("pct-encoded",
	  GLUE(
	    CHAR('%'),
	    REF("HEXDIG"),
	    REF("HEXDIG")
	  )
	);
	
	DEFINE_VOID("unreserved",
	  CHOICE(
	    REF("ALPHA"),
	    CHOICE(
	      REF("DIGIT"),
	      RANGE("-._~")
	    )
	  )
	);
	
	DEFINE_VOID("reserved",
	  CHOICE(
	    REF("gen-delims"),
	    REF("sub-delims")
	  )
	);
	
	DEFINE_VOID("gen-delims",
	  RANGE(":/?#[]@")
	);
	
	DEFINE_VOID("sub-delims",
	  RANGE("!$&'()*+,;=")
	);
	
	ENTRY("URI");
}

} // namespace ftl
