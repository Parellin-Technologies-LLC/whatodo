#include <node_api.h>

class Todo {
	public:
		static napi_status Init( napi_env env );
		static void Destructor( napi_env env, void* nativeObject, void* finalize_hint );
		static napi_status NewInstance( napi_env env, napi_value arg, napi_value* instance );

		napi_value getFileName() const { return fileName; }

	private:
		Todo();
		~Todo();

		static napi_ref constructor;
		static napi_value New( napi_env env, napi_callback_info info );
		napi_value fileName;
		napi_env env_;
		napi_ref wrapper_;
};
