#ifndef IDisplayText__H
#define IDisplayText__H

#include <gcom/gcom.h>

/* {CE41F1D8-349C-11D5-8000-000001135627} */
extern REFIID IID_IDisplayText;

BEGIN_INTERFACE( IDisplayText )
   HRESULT (*SetText)( IDisplayText *, char *, uint32 );
   HRESULT (*GetText)( IDisplayText *, char *, uint32 );
   HRESULT (*Display)( IDisplayText * );
END_INTERFACE( IDisplayText )

#endif
