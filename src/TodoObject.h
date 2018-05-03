#ifndef TODOOBJECT_H
#define TODOOBJECT_H

#include <napi.h>
#include <string>

class TodoObject : public Napi::ObjectWrap< TodoObject > {
	public:
		static void Init( Napi::Env env );
		static Napi::Object New( Napi::Value arg );

		TodoObject( const Napi::CallbackInfo &info );


		void setTodoPattern( const Napi::Value& n );

		Napi::Value ToObject( Napi::Env env, const TodoObject *tobj );

	private:
		static Napi::FunctionReference constructor;

		Napi::Value getFilename( const Napi::CallbackInfo &info );
		Napi::Value getTodoPattern( const Napi::CallbackInfo &info );

		std::string _filename;
		std::string _todo_pattern;

		int _high;
		int _mid;
		int _low;
		int _unknown;
};

#endif