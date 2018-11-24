#include "glext.h"
// GL_ARB_vertex_program
PFNGLVERTEXATTRIB1SARBPROC glVertexAttrib1sARB = NULL;
PFNGLVERTEXATTRIB1FARBPROC glVertexAttrib1fARB = NULL;
PFNGLVERTEXATTRIB1DARBPROC glVertexAttrib1dARB = NULL;
PFNGLVERTEXATTRIB2SARBPROC glVertexAttrib2sARB = NULL;
PFNGLVERTEXATTRIB2FARBPROC glVertexAttrib2fARB = NULL;
PFNGLVERTEXATTRIB2DARBPROC glVertexAttrib2dARB = NULL;
PFNGLVERTEXATTRIB3SARBPROC glVertexAttrib3sARB = NULL;
PFNGLVERTEXATTRIB3FARBPROC glVertexAttrib3fARB = NULL;
PFNGLVERTEXATTRIB3DARBPROC glVertexAttrib3dARB = NULL;
PFNGLVERTEXATTRIB4SARBPROC glVertexAttrib4sARB = NULL;
PFNGLVERTEXATTRIB4FARBPROC glVertexAttrib4fARB = NULL;
PFNGLVERTEXATTRIB4DARBPROC glVertexAttrib4dARB = NULL;
PFNGLVERTEXATTRIB4NUBARBPROC glVertexAttrib4NubARB = NULL;
PFNGLVERTEXATTRIB1SVARBPROC glVertexAttrib1svARB = NULL;
PFNGLVERTEXATTRIB1FVARBPROC glVertexAttrib1fvARB = NULL;
PFNGLVERTEXATTRIB1DVARBPROC glVertexAttrib1dvARB = NULL;
PFNGLVERTEXATTRIB2SVARBPROC glVertexAttrib2svARB = NULL;
PFNGLVERTEXATTRIB2FVARBPROC glVertexAttrib2fvARB = NULL;
PFNGLVERTEXATTRIB2DVARBPROC glVertexAttrib2dvARB = NULL;
PFNGLVERTEXATTRIB3SVARBPROC glVertexAttrib3svARB = NULL;
PFNGLVERTEXATTRIB3FVARBPROC glVertexAttrib3fvARB = NULL;
PFNGLVERTEXATTRIB3DVARBPROC glVertexAttrib3dvARB = NULL;
PFNGLVERTEXATTRIB4BVARBPROC glVertexAttrib4bvARB = NULL;
PFNGLVERTEXATTRIB4SVARBPROC glVertexAttrib4svARB = NULL;
PFNGLVERTEXATTRIB4IVARBPROC glVertexAttrib4ivARB = NULL;
PFNGLVERTEXATTRIB4UBVARBPROC glVertexAttrib4ubvARB = NULL;
PFNGLVERTEXATTRIB4USVARBPROC glVertexAttrib4usvARB = NULL;
PFNGLVERTEXATTRIB4UIVARBPROC glVertexAttrib4uivARB = NULL;
PFNGLVERTEXATTRIB4FVARBPROC glVertexAttrib4fvARB = NULL;
PFNGLVERTEXATTRIB4DVARBPROC glVertexAttrib4dvARB = NULL;
PFNGLVERTEXATTRIB4NBVARBPROC glVertexAttrib4NbvARB = NULL;
PFNGLVERTEXATTRIB4NSVARBPROC glVertexAttrib4NsvARB = NULL;
PFNGLVERTEXATTRIB4NIVARBPROC glVertexAttrib4NivARB = NULL;
PFNGLVERTEXATTRIB4NUBVARBPROC glVertexAttrib4NubvARB = NULL;
PFNGLVERTEXATTRIB4NUSVARBPROC glVertexAttrib4NusvARB = NULL;
PFNGLVERTEXATTRIB4NUIVARBPROC glVertexAttrib4NuivARB = NULL;
PFNGLVERTEXATTRIBPOINTERARBPROC glVertexAttribPointerARB = NULL;
PFNGLENABLEVERTEXATTRIBARRAYARBPROC glEnableVertexAttribArrayARB = NULL;
PFNGLDISABLEVERTEXATTRIBARRAYARBPROC glDisableVertexAttribArrayARB = NULL;
PFNGLPROGRAMSTRINGARBPROC glProgramStringARB = NULL;
PFNGLBINDPROGRAMARBPROC glBindProgramARB = NULL;
PFNGLDELETEPROGRAMSARBPROC glDeleteProgramsARB = NULL;
PFNGLGENPROGRAMSARBPROC glGenProgramsARB = NULL;
PFNGLPROGRAMENVPARAMETER4DARBPROC glProgramEnvParameter4dARB = NULL;
PFNGLPROGRAMENVPARAMETER4DVARBPROC glProgramEnvParameter4dvARB = NULL;
PFNGLPROGRAMENVPARAMETER4FARBPROC glProgramEnvParameter4fARB = NULL;
PFNGLPROGRAMENVPARAMETER4FVARBPROC glProgramEnvParameter4fvARB = NULL;
PFNGLPROGRAMLOCALPARAMETER4DARBPROC glProgramLocalParameter4dARB = NULL;
PFNGLPROGRAMLOCALPARAMETER4DVARBPROC glProgramLocalParameter4dvARB = NULL;
PFNGLPROGRAMLOCALPARAMETER4FARBPROC glProgramLocalParameter4fARB = NULL;
PFNGLPROGRAMLOCALPARAMETER4FVARBPROC glProgramLocalParameter4fvARB = NULL;
PFNGLGETPROGRAMENVPARAMETERDVARBPROC glGetProgramEnvParameterdvARB = NULL;
PFNGLGETPROGRAMENVPARAMETERFVARBPROC glGetProgramEnvParameterfvARB = NULL;
PFNGLGETPROGRAMLOCALPARAMETERDVARBPROC glGetProgramLocalParameterdvARB = NULL;
PFNGLGETPROGRAMLOCALPARAMETERFVARBPROC glGetProgramLocalParameterfvARB = NULL;
PFNGLGETPROGRAMIVARBPROC glGetProgramivARB = NULL;
PFNGLGETPROGRAMSTRINGARBPROC glGetProgramStringARB = NULL;
PFNGLGETVERTEXATTRIBDVARBPROC glGetVertexAttribdvARB = NULL;
PFNGLGETVERTEXATTRIBFVARBPROC glGetVertexAttribfvARB = NULL;
PFNGLGETVERTEXATTRIBIVARBPROC glGetVertexAttribivARB = NULL;
PFNGLGETVERTEXATTRIBPOINTERVARBPROC glGetVertexAttribPointervARB = NULL;
PFNGLISPROGRAMARBPROC glIsProgramARB = NULL;

PFNGLCREATEPROGRAMOBJECTARBPROC glCreateProgramObjectARB = NULL;
PFNGLCREATESHADEROBJECTARBPROC glCreateShaderObjectARB = NULL;
PFNGLDELETEOBJECTARBPROC glDeleteObjectARB = NULL;
PFNGLDETACHOBJECTARBPROC glDetachObjectARB = NULL;
PFNGLATTACHOBJECTARBPROC glAttachObjectARB = NULL;
PFNGLSHADERSOURCEARBPROC glShaderSourceARB = NULL;
PFNGLCOMPILESHADERARBPROC glCompileShaderARB = NULL;
PFNGLLINKPROGRAMARBPROC glLinkProgramARB = NULL;
PFNGLGETINFOLOGARBPROC glGetInfoLogARB = NULL;
PFNGLUSEPROGRAMOBJECTARBPROC glUseProgramObjectARB = NULL;
PFNGLGETOBJECTPARAMETERIVARBPROC glGetObjectParameterivARB = NULL;
PFNGLGETOBJECTPARAMETERFVARBPROC glGetObjectParameterfvARB = NULL;
PFNGLGETUNIFORMLOCATIONARBPROC glGetUniformLocationARB = NULL;
PFNGLUNIFORM1FARBPROC glUniform1fARB = NULL;
PFNGLUNIFORM2FARBPROC glUniform2fARB = NULL;
PFNGLUNIFORM3FARBPROC glUniform3fARB = NULL;
PFNGLUNIFORM4FARBPROC glUniform4fARB = NULL;
PFNGLUNIFORM1IARBPROC glUniform1iARB = NULL;
PFNGLUNIFORM2IARBPROC glUniform2iARB = NULL;
PFNGLUNIFORM3IARBPROC glUniform3iARB = NULL;
PFNGLUNIFORM4IARBPROC glUniform4iARB = NULL;
PFNGLUNIFORM1FVARBPROC glUniform1fvARB = NULL;
PFNGLUNIFORM2FVARBPROC glUniform2fvARB = NULL;
PFNGLUNIFORM3FVARBPROC glUniform3fvARB = NULL;
PFNGLUNIFORM4FVARBPROC glUniform4fvARB = NULL;
PFNGLUNIFORM1IVARBPROC glUniform1ivARB = NULL;
PFNGLUNIFORM2IVARBPROC glUniform2ivARB = NULL;
PFNGLUNIFORM3IVARBPROC glUniform3ivARB = NULL;
PFNGLUNIFORM4IVARBPROC glUniform4ivARB = NULL;
PFNGLUNIFORM1BARBPROC glUniform1bARB = NULL;

BOOL bNoFragmentProgram = TRUE;
BOOL bSupportGLSL = FALSE;

/*****************************************************************************
 * Function Name  : InitExtensions
 * Inputs		  : 
 * Returns		  : BOOL
 * Description    : Inits the OpenGL extensions needed by the program. Returns
					FALSE if required extensions are not found
 *****************************************************************************/
BOOL InitExtensions()
{
	char *extensions;
	extensions = (char*)glGetString(GL_EXTENSIONS);

	/* if the OpenGL implementation does not support ARB_vertex_program extension
	quit, this is minimum that we need */
	if(!strstr(extensions, "GL_ARB_vertex_program"))
		return FALSE;

	/* we need ARB_fragment_program extension for more advanced features,
	but we can do without */
/*	if(strstr(extensions, "GL_ARB_fragment_program"))
		bNoFragmentProgram = FALSE;*/

	glProgramStringARB  = (PFNGLPROGRAMSTRINGARBPROC)  wglGetProcAddress("glProgramStringARB");
	glBindProgramARB    = (PFNGLBINDPROGRAMARBPROC)    wglGetProcAddress("glBindProgramARB");
	glDeleteProgramsARB = (PFNGLDELETEPROGRAMSARBPROC) wglGetProcAddress("glDeleteProgramsARB");
	glGenProgramsARB    = (PFNGLGENPROGRAMSARBPROC)    wglGetProcAddress("glGenProgramsARB");
	glProgramEnvParameter4dARB    = (PFNGLPROGRAMENVPARAMETER4DARBPROC)    wglGetProcAddress("glProgramEnvParameter4dARB");
	glProgramEnvParameter4dvARB   = (PFNGLPROGRAMENVPARAMETER4DVARBPROC)   wglGetProcAddress("glProgramEnvParameter4dvARB");
	glProgramEnvParameter4fARB    = (PFNGLPROGRAMENVPARAMETER4FARBPROC)    wglGetProcAddress("glProgramEnvParameter4fARB");
	glProgramEnvParameter4fvARB   = (PFNGLPROGRAMENVPARAMETER4FVARBPROC)   wglGetProcAddress("glProgramEnvParameter4fvARB");
	glProgramLocalParameter4dARB  = (PFNGLPROGRAMLOCALPARAMETER4DARBPROC)  wglGetProcAddress("glProgramLocalParameter4dARB");
	glProgramLocalParameter4dvARB = (PFNGLPROGRAMLOCALPARAMETER4DVARBPROC) wglGetProcAddress("glProgramLocalParameter4dvARB");
	glProgramLocalParameter4fARB  = (PFNGLPROGRAMLOCALPARAMETER4FARBPROC)  wglGetProcAddress("glProgramLocalParameter4fARB");
	glProgramLocalParameter4fvARB = (PFNGLPROGRAMLOCALPARAMETER4FVARBPROC) wglGetProcAddress("glProgramLocalParameter4fvARB");
	glGetProgramEnvParameterdvARB = (PFNGLGETPROGRAMENVPARAMETERDVARBPROC) wglGetProcAddress("glGetProgramEnvParameterdvARB");
	glGetProgramEnvParameterfvARB = (PFNGLGETPROGRAMENVPARAMETERFVARBPROC) wglGetProcAddress("glGetProgramEnvParameterfvARB");
	glGetProgramLocalParameterdvARB = (PFNGLGETPROGRAMLOCALPARAMETERDVARBPROC) wglGetProcAddress("glGetProgramLocalParameterdvARB");
	glGetProgramLocalParameterfvARB = (PFNGLGETPROGRAMLOCALPARAMETERFVARBPROC) wglGetProcAddress("glGetProgramLocalParameterfvARB");
	glGetProgramivARB     = (PFNGLGETPROGRAMIVARBPROC)     wglGetProcAddress("glGetProgramivARB");
	glGetProgramStringARB = (PFNGLGETPROGRAMSTRINGARBPROC) wglGetProcAddress("glGetProgramStringARB");
	glIsProgramARB        = (PFNGLISPROGRAMARBPROC)        wglGetProcAddress("glIsProgramARB");
	glVertexAttrib1sARB = (PFNGLVERTEXATTRIB1SARBPROC) wglGetProcAddress("glVertexAttrib1sARB");
	glVertexAttrib1fARB = (PFNGLVERTEXATTRIB1FARBPROC) wglGetProcAddress("glVertexAttrib1fARB");
	glVertexAttrib1dARB = (PFNGLVERTEXATTRIB1DARBPROC) wglGetProcAddress("glVertexAttrib1dARB");
	glVertexAttrib2sARB = (PFNGLVERTEXATTRIB2SARBPROC) wglGetProcAddress("glVertexAttrib2sARB");
	glVertexAttrib2fARB = (PFNGLVERTEXATTRIB2FARBPROC) wglGetProcAddress("glVertexAttrib2fARB");
	glVertexAttrib2dARB = (PFNGLVERTEXATTRIB2DARBPROC) wglGetProcAddress("glVertexAttrib2dARB");
	glVertexAttrib3sARB = (PFNGLVERTEXATTRIB3SARBPROC) wglGetProcAddress("glVertexAttrib3sARB");
	glVertexAttrib3fARB = (PFNGLVERTEXATTRIB3FARBPROC) wglGetProcAddress("glVertexAttrib3fARB");
	glVertexAttrib3dARB = (PFNGLVERTEXATTRIB3DARBPROC) wglGetProcAddress("glVertexAttrib3dARB");
	glVertexAttrib4sARB = (PFNGLVERTEXATTRIB4SARBPROC) wglGetProcAddress("glVertexAttrib4sARB");
	glVertexAttrib4fARB = (PFNGLVERTEXATTRIB4FARBPROC) wglGetProcAddress("glVertexAttrib4fARB");
	glVertexAttrib4dARB = (PFNGLVERTEXATTRIB4DARBPROC) wglGetProcAddress("glVertexAttrib4dARB");
	glVertexAttrib4NubARB = (PFNGLVERTEXATTRIB4NUBARBPROC) wglGetProcAddress("glVertexAttrib4NubARB");
	glVertexAttrib1svARB  = (PFNGLVERTEXATTRIB1SVARBPROC)  wglGetProcAddress("glVertexAttrib1svARB");
	glVertexAttrib1fvARB  = (PFNGLVERTEXATTRIB1FVARBPROC)  wglGetProcAddress("glVertexAttrib1fvARB");
	glVertexAttrib1dvARB  = (PFNGLVERTEXATTRIB1DVARBPROC)  wglGetProcAddress("glVertexAttrib1dvARB");
	glVertexAttrib2svARB  = (PFNGLVERTEXATTRIB2SVARBPROC)  wglGetProcAddress("glVertexAttrib2svARB");
	glVertexAttrib2fvARB  = (PFNGLVERTEXATTRIB2FVARBPROC)  wglGetProcAddress("glVertexAttrib2fvARB");
	glVertexAttrib2dvARB  = (PFNGLVERTEXATTRIB2DVARBPROC)  wglGetProcAddress("glVertexAttrib2dvARB");
	glVertexAttrib3svARB  = (PFNGLVERTEXATTRIB3SVARBPROC)  wglGetProcAddress("glVertexAttrib3svARB");
	glVertexAttrib3fvARB  = (PFNGLVERTEXATTRIB3FVARBPROC)  wglGetProcAddress("glVertexAttrib3fvARB");
	glVertexAttrib3dvARB  = (PFNGLVERTEXATTRIB3DVARBPROC)  wglGetProcAddress("glVertexAttrib3dvARB");
	glVertexAttrib4bvARB  = (PFNGLVERTEXATTRIB4BVARBPROC)  wglGetProcAddress("glVertexAttrib4bvARB");
	glVertexAttrib4svARB  = (PFNGLVERTEXATTRIB4SVARBPROC)  wglGetProcAddress("glVertexAttrib4svARB");
	glVertexAttrib4ivARB  = (PFNGLVERTEXATTRIB4IVARBPROC)  wglGetProcAddress("glVertexAttrib4ivARB");
	glVertexAttrib4ubvARB = (PFNGLVERTEXATTRIB4UBVARBPROC) wglGetProcAddress("glVertexAttrib4ubvARB");
	glVertexAttrib4usvARB = (PFNGLVERTEXATTRIB4USVARBPROC) wglGetProcAddress("glVertexAttrib4usvARB");
	glVertexAttrib4uivARB = (PFNGLVERTEXATTRIB4UIVARBPROC) wglGetProcAddress("glVertexAttrib4uivARB");
	glVertexAttrib4fvARB  = (PFNGLVERTEXATTRIB4FVARBPROC)  wglGetProcAddress("glVertexAttrib4fvARB");
	glVertexAttrib4dvARB  = (PFNGLVERTEXATTRIB4DVARBPROC)  wglGetProcAddress("glVertexAttrib4dvARB");
	glVertexAttrib4NbvARB = (PFNGLVERTEXATTRIB4NBVARBPROC) wglGetProcAddress("glVertexAttrib4NbvARB");
	glVertexAttrib4NsvARB = (PFNGLVERTEXATTRIB4NSVARBPROC) wglGetProcAddress("glVertexAttrib4NsvARB");
	glVertexAttrib4NivARB = (PFNGLVERTEXATTRIB4NIVARBPROC) wglGetProcAddress("glVertexAttrib4NivARB");
	glVertexAttrib4NubvARB = (PFNGLVERTEXATTRIB4NUBVARBPROC) wglGetProcAddress("glVertexAttrib4NubvARB");
	glVertexAttrib4NusvARB = (PFNGLVERTEXATTRIB4NUSVARBPROC) wglGetProcAddress("glVertexAttrib4NusvARB");
	glVertexAttrib4NuivARB = (PFNGLVERTEXATTRIB4NUIVARBPROC) wglGetProcAddress("glVertexAttrib4NuivARB");
	glVertexAttribPointerARB = (PFNGLVERTEXATTRIBPOINTERARBPROC) wglGetProcAddress("glVertexAttribPointerARB");
	glEnableVertexAttribArrayARB = (PFNGLENABLEVERTEXATTRIBARRAYARBPROC) wglGetProcAddress("glEnableVertexAttribArrayARB");
	glDisableVertexAttribArrayARB = (PFNGLDISABLEVERTEXATTRIBARRAYARBPROC) wglGetProcAddress("glDisableVertexAttribArrayARB");

	glGetVertexAttribdvARB = (PFNGLGETVERTEXATTRIBDVARBPROC) wglGetProcAddress("glGetVertexAttribdvARB");
	glGetVertexAttribfvARB = (PFNGLGETVERTEXATTRIBFVARBPROC) wglGetProcAddress("glGetVertexAttribfvARB");
	glGetVertexAttribivARB = (PFNGLGETVERTEXATTRIBIVARBPROC) wglGetProcAddress("glGetVertexAttribivARB");
	glGetVertexAttribPointervARB = (PFNGLGETVERTEXATTRIBPOINTERVARBPROC) wglGetProcAddress("glGetVertexAttribPointervARB");

	if(strstr(extensions, "GL_ARB_vertex_shader"))
	{
		bSupportGLSL = TRUE;

		glCreateProgramObjectARB = (PFNGLCREATEPROGRAMOBJECTARBPROC) wglGetProcAddress("glCreateProgramObjectARB");
		glCreateShaderObjectARB = (PFNGLCREATESHADEROBJECTARBPROC) wglGetProcAddress("glCreateShaderObjectARB");
		glDeleteObjectARB = (PFNGLDELETEOBJECTARBPROC) wglGetProcAddress("glDeleteObjectARB");
		glDetachObjectARB = (PFNGLDETACHOBJECTARBPROC) wglGetProcAddress("glDetachObjectARB");
		glAttachObjectARB = (PFNGLATTACHOBJECTARBPROC) wglGetProcAddress("glAttachObjectARB");
		glShaderSourceARB= (PFNGLSHADERSOURCEARBPROC) wglGetProcAddress("glShaderSourceARB");
		glCompileShaderARB = (PFNGLCOMPILESHADERARBPROC) wglGetProcAddress("glCompileShaderARB");
		glLinkProgramARB = (PFNGLLINKPROGRAMARBPROC) wglGetProcAddress("glLinkProgramARB");
		glGetInfoLogARB = (PFNGLGETINFOLOGARBPROC) wglGetProcAddress("glGetInfoLogARB");
		glUseProgramObjectARB = (PFNGLUSEPROGRAMOBJECTARBPROC) wglGetProcAddress("glUseProgramObjectARB");
		glGetObjectParameterivARB = (PFNGLGETOBJECTPARAMETERIVARBPROC) wglGetProcAddress("glGetObjectParameterivARB");
		glGetObjectParameterfvARB = (PFNGLGETOBJECTPARAMETERFVARBPROC) wglGetProcAddress("glGetObjectParameterfvARB");
		glGetUniformLocationARB = (PFNGLGETUNIFORMLOCATIONARBPROC) wglGetProcAddress("glGetUniformLocationARB");
//		glGetAttribLocationARB = (PFNGLGETATTRIBLOCATIONARBPROC) wglGetProcAddress("glGetAttribLocationARB");
//		glBindAttribLocationARB = (PFNGLBINDATTRIBLOCATIONARBPROC) wglGetProcAddress("glBindAttribLocationARB");
		glUniform1fARB = (PFNGLUNIFORM1FARBPROC) wglGetProcAddress("glUniform1fARB");
		glUniform2fARB = (PFNGLUNIFORM2FARBPROC) wglGetProcAddress("glUniform2fARB");
		glUniform3fARB = (PFNGLUNIFORM3FARBPROC) wglGetProcAddress("glUniform3fARB");
		glUniform4fARB = (PFNGLUNIFORM4FARBPROC) wglGetProcAddress("glUniform4fARB");
		glUniform1iARB = (PFNGLUNIFORM1IARBPROC) wglGetProcAddress("glUniform1iARB");
		glUniform2iARB = (PFNGLUNIFORM2IARBPROC) wglGetProcAddress("glUniform2iARB");
		glUniform3iARB = (PFNGLUNIFORM3IARBPROC) wglGetProcAddress("glUniform3iARB");
		glUniform4iARB = (PFNGLUNIFORM4IARBPROC) wglGetProcAddress("glUniform4iARB");
		glUniform1fvARB = (PFNGLUNIFORM1FVARBPROC) wglGetProcAddress("glUniform1fvARB");
		glUniform2fvARB = (PFNGLUNIFORM2FVARBPROC) wglGetProcAddress("glUniform2fvARB");
		glUniform3fvARB = (PFNGLUNIFORM3FVARBPROC) wglGetProcAddress("glUniform3fvARB");
		glUniform4fvARB = (PFNGLUNIFORM4FVARBPROC) wglGetProcAddress("glUniform4fvARB");
		glUniform1ivARB = (PFNGLUNIFORM1IVARBPROC) wglGetProcAddress("glUniform1ivARB");
		glUniform2ivARB = (PFNGLUNIFORM2IVARBPROC) wglGetProcAddress("glUniform2ivARB");
		glUniform3ivARB = (PFNGLUNIFORM3IVARBPROC) wglGetProcAddress("glUniform3ivARB");
		glUniform4ivARB = (PFNGLUNIFORM4IVARBPROC) wglGetProcAddress("glUniform4ivARB");
		glUniform1bARB = (PFNGLUNIFORM1BARBPROC) wglGetProcAddress("glUniform1bARB");
		glVertexAttrib4fvARB = (PFNGLVERTEXATTRIB4FVARBPROC) wglGetProcAddress("glVertexAttrib1fvARB");
		glVertexAttrib3fvARB = (PFNGLVERTEXATTRIB3FVARBPROC) wglGetProcAddress("glVertexAttrib2fvARB");
		glVertexAttrib2fvARB = (PFNGLVERTEXATTRIB2FVARBPROC) wglGetProcAddress("glVertexAttrib3fvARB");
		glVertexAttrib1fvARB = (PFNGLVERTEXATTRIB1FVARBPROC) wglGetProcAddress("glVertexAttrib4fvARB");
		glVertexAttrib4fARB = (PFNGLVERTEXATTRIB4FARBPROC) wglGetProcAddress("glVertexAttrib1fARB");
		glVertexAttrib3fARB = (PFNGLVERTEXATTRIB3FARBPROC) wglGetProcAddress("glVertexAttrib2fARB");
		glVertexAttrib2fARB = (PFNGLVERTEXATTRIB2FARBPROC) wglGetProcAddress("glVertexAttrib3fARB");
		glVertexAttrib1fARB = (PFNGLVERTEXATTRIB1FARBPROC) wglGetProcAddress("glVertexAttrib4fARB");
	}

	return TRUE;
}