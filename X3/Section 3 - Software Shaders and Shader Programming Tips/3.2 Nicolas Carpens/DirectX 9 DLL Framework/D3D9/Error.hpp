#ifndef D3D9_Error_hpp
#define D3D9_Error_hpp

namespace D3D9
{
	class Error
	{
	public:
		Error(const char *format, ...);

		const char *getString() const;

		Error &operator<<(const Error &error);
		Error &operator>>(const Error &error);

	private:
		char string[256];
	};

	#ifndef __FUNCSIG__
		#define	__FUNCSIG__ "<function signature unavailable>"
	#endif

	#ifndef NDEBUG
		#define INTERNAL_ERROR Error("%s(%d):\n\tInternal error in '%s'", __FILE__, __LINE__, __FUNCSIG__)
		#define EXCEPTION      Error("%s(%d):\n\t", __FILE__, __LINE__) << Error
	#else
		#define INTERNAL_ERROR Error("Internal error in '%s'", __FUNCSIG__)
		#define EXCEPTION      Error	
	#endif
}

#endif   // D3D9_Error_hpp
