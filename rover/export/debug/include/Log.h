// Generated by Haxe 4.3.1
#ifndef INCLUDED_Log
#define INCLUDED_Log

#ifndef HXCPP_H
#include <hxcpp.h>
#endif

HX_DECLARE_CLASS0(Log)



class HXCPP_CLASS_ATTRIBUTES Log_obj : public ::hx::Object
{
	public:
		typedef ::hx::Object super;
		typedef Log_obj OBJ_;
		Log_obj();

	public:
		enum { _hx_ClassId = 0x7ede50f8 };

		void __construct();
		inline void *operator new(size_t inSize, bool inContainer=false,const char *inName="Log")
			{ return ::hx::Object::operator new(inSize,inContainer,inName); }
		inline void *operator new(size_t inSize, int extra)
			{ return ::hx::Object::operator new(inSize+extra,false,"Log"); }

		inline static ::hx::ObjectPtr< Log_obj > __new() {
			::hx::ObjectPtr< Log_obj > __this = new Log_obj();
			__this->__construct();
			return __this;
		}

		inline static ::hx::ObjectPtr< Log_obj > __alloc(::hx::Ctx *_hx_ctx) {
			Log_obj *__this = (Log_obj*)(::hx::Ctx::alloc(_hx_ctx, sizeof(Log_obj), false, "Log"));
			*(void **)__this = Log_obj::_hx_vtable;
			return __this;
		}

		static void * _hx_vtable;
		static Dynamic __CreateEmpty();
		static Dynamic __Create(::hx::DynamicArray inArgs);
		//~Log_obj();

		HX_DO_RTTI_ALL;
		static bool __GetStatic(const ::String &inString, Dynamic &outValue, ::hx::PropertyAccess inCallProp);
		static void __register();
		bool _hx_isInstanceOf(int inClassId);
		::String __ToString() const { return HX_("Log",64,0c,3a,00); }

		static void warning(::String message);
		static ::Dynamic warning_dyn();

};


#endif /* INCLUDED_Log */ 
