/*
Used to enable error manager macros
*/

//for errors
#define _ERROR (ErrMgr->SetOwner(__FILE__,__LINE__,__FUNCTION__,ErrMgr->FlgError,::GetLastError()),false) ? ErrMgr->SetOwner("",0,"",0,0) : _ERROR

//for warnings
#define _WARN (ErrMgr->SetOwner(__FILE__,__LINE__,__FUNCTION__,ErrMgr->FlgWarning,::GetLastError()),false) ? ErrMgr->SetOwner("",0,"",0,0) : _WARN

//for fatal errors
#define _FATAL (ErrMgr->SetOwner(__FILE__,__LINE__,__FUNCTION__,ErrMgr->FlgFatal,::GetLastError()),false) ? ErrMgr->SetOwner("",0,"",0,0) : _FATAL

//Log globals
#define _LOG (ErrMgr->SetOwner(__FILE__,__LINE__,__FUNCTION__,ErrMgr->FlgLog,::GetLastError()),false) ? ErrMgr->SetOwner("",0,"",0,0) : _LOG
#define _LOGB (ErrMgr->SetOwner(__FILE__,__LINE__,__FUNCTION__,ErrMgr->FlgLog,::GetLastError()),false) ? 0 : _LOGB
#define _LOGE (ErrMgr->SetOwner(__FILE__,__LINE__,__FUNCTION__,ErrMgr->FlgLog,::GetLastError()),false) ? ErrMgr->SetOwner("",0,"",0,0) : _LOGE


