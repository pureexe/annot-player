#pragma once

// texthook_config.h
// 10/20/2011

#ifdef TEXTHOOK_LIB
# define TEXTHOOKAPI Q_DECL_EXPORT
#elif defined(QTH_STATICLIB)
# define TEXTHOOKAPI
#else
# define TEXTHOOKAPI Q_DECL_IMPORT
#endif // TEXTHOOK_LIB

// EOF
