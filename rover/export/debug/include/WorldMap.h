// Generated by Haxe 4.3.1
#ifndef INCLUDED_WorldMap
#define INCLUDED_WorldMap

#ifndef HXCPP_H
#include <hxcpp.h>
#endif

HX_DECLARE_CLASS0(Player)
HX_DECLARE_CLASS0(WorldMap)



class HXCPP_CLASS_ATTRIBUTES WorldMap_obj : public ::hx::Object
{
	public:
		typedef ::hx::Object super;
		typedef WorldMap_obj OBJ_;
		WorldMap_obj();

	public:
		enum { _hx_ClassId = 0x30afffae };

		void __construct(::String serverFile);
		inline void *operator new(size_t inSize, bool inContainer=true,const char *inName="WorldMap")
			{ return ::hx::Object::operator new(inSize,inContainer,inName); }
		inline void *operator new(size_t inSize, int extra)
			{ return ::hx::Object::operator new(inSize+extra,true,"WorldMap"); }
		static ::hx::ObjectPtr< WorldMap_obj > __new(::String serverFile);
		static ::hx::ObjectPtr< WorldMap_obj > __alloc(::hx::Ctx *_hx_ctx,::String serverFile);
		static void * _hx_vtable;
		static Dynamic __CreateEmpty();
		static Dynamic __Create(::hx::DynamicArray inArgs);
		//~WorldMap_obj();

		HX_DO_RTTI_ALL;
		::hx::Val __Field(const ::String &inString, ::hx::PropertyAccess inCallProp);
		::hx::Val __SetField(const ::String &inString,const ::hx::Val &inValue, ::hx::PropertyAccess inCallProp);
		void __GetFields(Array< ::String> &outFields);
		static void __register();
		void __Mark(HX_MARK_PARAMS);
		void __Visit(HX_VISIT_PARAMS);
		bool _hx_isInstanceOf(int inClassId);
		::String __ToString() const { return HX_("WorldMap",8a,27,3d,7e); }

		int width;
		int height;
		::String content;
		::Array< ::Dynamic> matrix;
		 ::Dynamic base;
		 ::Player player;
		int length;
		int iron;
		int osmium;
		void refresh();
		::Dynamic refresh_dyn();

};


#endif /* INCLUDED_WorldMap */ 
