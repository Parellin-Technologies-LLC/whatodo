class Generics {
public:
    std::string id;
    int value;
    SomeClass() {};
    SomeClass( std::string id, int value ): id( id ), value( value ) {};

    static SomeClass *unpack(v8::Isolate *isolate, const v8::Local<v8::Object> object) {
        using namespace v8;

        SomeClass *p = new SomeClass();
        Local<Value> _id = object->Get(String::NewFromUtf8(isolate, "id"));

        String::Utf8Value utf8String(_id);
        p->id = std::string(*utf8String);
        p->value = static_cast<int>(object->Get(String::NewFromUtf8(isolate, "value"))->NumberValue());

        return p;
    }

    static v8::Local<v8::Object> pack(v8::Isolate *isolate, const SomeClass *object) {
        using namespace v8;
        Local<Object> obj = Object::New(isolate);

        obj->Set(String::NewFromUtf8(isolate, "id"), String::NewFromUtf8(isolate, object->id.c_str()));
        obj->Set(String::NewFromUtf8(isolate, "value"), Integer::New(isolate, object->value));

        return obj;
    };
}